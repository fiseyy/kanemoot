#include "controller/appcontroller.h"
#include "controller/loginpage.h"
#include "controller/registrationpage.h"
#include "controller/chatpage.h"
#include "utils/logging.h"
#include "core/errorcode.h"
#include "core/securestorage.h"
#include "domain/authmanager.h"

AppController::AppController(QObject *parent, QQmlApplicationEngine* engine)
    : QObject(parent), engine(engine)
{}

void AppController::setCurrentPage(BasePage *newPage)
{
    if (currentPage) {
        currentPage->cleanup();
        currentPage->deleteLater();
        currentPage = nullptr;
    }

    connect(newPage, &BasePage::requestPageChange, this, &AppController::onPageChangeRequested);

    if (!engine) {
        LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x01, ErrorCode::AppController), "Engine QML не инициализирован");
        return;
    }

    QQmlComponent component(engine, QUrl(newPage->qmlPath()));
    if (component.status() != QQmlComponent::Ready) {
        LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x02, ErrorCode::AppController), component.errorString());
        return;
    }

    QObject* obj = component.create();
    if (!obj) {
        LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x03, ErrorCode::AppController), "");
        return;
    }

    QQuickItem* pageItem = qobject_cast<QQuickItem*>(obj);
    QObject* rootWindowObj = engine->rootObjects().value(0);
    QQuickWindow* rootWindow = qobject_cast<QQuickWindow*>(rootWindowObj);

    if (!pageItem || !rootWindow) {
        LOG(Logging::Warning, ErrorCode::make(ErrorCode::UI, 0x04, ErrorCode::AppController), "");
        return;
    }

    // ищем контейнер
    QObject* containerObj = rootWindow->findChild<QObject*>("pageContainer");
    QQuickItem* containerItem = qobject_cast<QQuickItem*>(containerObj);
    if (!containerItem) {
        LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x07, ErrorCode::AppController), "pageContainer не найден");
        return;
    }

    // цепляем страницу в контейнер
    pageItem->setParentItem(containerItem);

    newPage->setRootObject(pageItem);
    currentPage = newPage;
    currentPage->init();
}

void AppController::start()
{
    auto jwt_opt = SecureStorage::instance().getValue("jwt-token");
    QString jwt_token;

    if (jwt_opt.has_value()) {
        jwt_token = jwt_opt.value();
    }

    if (!jwt_token.isEmpty()) {
        Logging::instance().log(Logging::Info, "Сессия найдена. Попытка восстановить пользователя по сохранённой сессии...");

        AuthManager* authmgr = new AuthManager(this);
        connect(authmgr, &AuthManager::authSucceeded, this, [this](){
            // qDebug() << "Пользователь вошел в аккаунт автоматически.";
            Logging::instance().log(Logging::Info, "Пользователь восстановлен по JWT.");
            ChatPage* chatPage = new ChatPage(this);
            setCurrentPage(chatPage);
        });
        connect(authmgr, &AuthManager::authFailed, this, [this](){
            // qDebug() << "Пользователь не вошел в аккаунт автоматически.";
            Logging::instance().log(Logging::Info, "Не удалось восстановить пользователя по JWT.");
            LoginPage* loginPage = new LoginPage(this);
            setCurrentPage(loginPage);
        });
        authmgr->tryAutoLogin(jwt_token);
        return;
    }
    LoginPage* loginPage = new LoginPage(this);
    setCurrentPage(loginPage);
}


void AppController::onPageChangeRequested(int newPageId)
{
    BasePage* newPage = nullptr;
    switch(newPageId) {
        case Page_Login: newPage = new LoginPage(this); break;
        case Page_Register: newPage = new RegistrationPage(this); break;
        case Page_Chat: newPage = new ChatPage(this); break;
    }
    if (newPage != nullptr) setCurrentPage(newPage);
}
