#include "controller/chatpage.h"
#include "utils/logging.h"
#include "core/errorcode.h"
#include "core/securestorage.h"
#include "core/errorhandler.h"
#include "core/settingsmanager.h"
#include <QQmlComponent>
#include <qqmlcontext.h>
#include <QJsonArray>
ChatPage::ChatPage(QObject *parent) {
    m_chatmgr = new ChatManager(this);
    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setInterval(2000);
    m_reconnectTimer->setSingleShot(false);
    connect(m_reconnectTimer, &QTimer::timeout, [this]() {
        if (!m_chatmgr->isConnected()) {
            Logging::instance().log(Logging::Debug, "Пробуем переподключиться к Chat Service...");
            m_chatmgr->connectToChat();
        }
    });
    connect(m_chatmgr, &ChatManager::userServersReceived, this, [this](const QJsonArray &servers){
        qDebug() << "Серверы пользователя:" << servers;
    });

    connect(m_chatmgr, &ChatManager::connected, [this]() {
        m_reconnectTimer->stop();
        if(m_showingLoading)
            m_showingLoading = false;
            emit connectedToChat();
    });
    connect(m_chatmgr, &ChatManager::disconnected, [this]() {
        Logging::instance().log(Logging::Debug, "Отключено от Chat Service.");

        if (!m_showingLoading) {
            QObject* root = getRootObject();
            if (!root) return;
            QObject* loadingPage = root->findChild<QObject*>("loadingPage");
            if (loadingPage) {
                QMetaObject::invokeMethod(loadingPage, "show");
                m_showingLoading = true;
                ErrorHandler::instance().setHiden(true);
            }
        }

        if (!m_reconnectTimer->isActive()) m_reconnectTimer->start();
    });
}

void ChatPage::init()
{
    QObject* root = getRootObject();
    if (!root) {
        LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x06, ErrorCode::ChatPage), "");
        return;
    }

    QObject* editPopup = root->findChild<QObject*>("editPopup");
    if(editPopup) {
        connect(editPopup, SIGNAL(logoutRequested()),
                this, SLOT(logoutRedirect()));
    }

    QObject* addServerOverlay = root->findChild<QObject*>("addServerOverlay");
    if (addServerOverlay) {
        connect(addServerOverlay, SIGNAL(joinServerRequested(QString)),
                this, SLOT(joinServer(QString)));
        connect(addServerOverlay, SIGNAL(createServerRequested(QString)),
                this, SLOT(createServer(QString)));
    }
    else {
        qWarning() << "addServerOverlay не найден";
    }


    QObject* loadingPage = root->findChild<QObject*>("loadingPage");

    auto &settings = SettingsManager::instance();
    setTheme(settings.theme() == "light");

    ErrorHandler::instance().setHiden(true);
    connect(this, &ChatPage::connectedToChat, [loadingPage, this]() {
        ErrorHandler::instance().setHiden(false);
        if (loadingPage) {
            QMetaObject::invokeMethod(loadingPage, "hide");
        }
        else {
            LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x0A, ErrorCode::ChatPage), "loadingPage не объявлен");
        }
        m_chatmgr->requestUserServers();
    });
    m_chatmgr->connectToChat();
    QString username;
    auto optUsername = SecureStorage::instance().getValue("username");
    if(!optUsername) LOG(Logging::Fatal, ErrorCode::make(ErrorCode::System, 0x02, ErrorCode::ChatPage), "");
    else {
        username = optUsername.value();
    }
    root->setProperty("username", username);
}


void ChatPage::cleanup() {
    if (m_chatmgr) {
        m_chatmgr->deleteLater();
        m_chatmgr = nullptr;
    }
    if (m_reconnectTimer) {
        m_reconnectTimer->stop();
        m_reconnectTimer->deleteLater();
        m_reconnectTimer = nullptr;
    }
}


void ChatPage::setTheme(bool useLightTheme) {
    QObject* rootItem = getRootObject();
    if (!rootItem) return;

    QQmlEngine* engine = QQmlEngine::contextForObject(rootItem)->engine();

    QQmlComponent component(engine, QUrl(useLightTheme
                                             ? "qrc:/kanemoot/ui/themes/Light.qml"
                                             : "qrc:/kanemoot/ui/themes/Dark.qml"));
    QObject* theme = component.create();
    if (!theme) {
        LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x05, ErrorCode::ChatPage), "");
        return;
    }

    rootItem->setProperty("currentTheme", QVariant::fromValue(theme));
}

QObject* ChatPage::currentTheme() const {
    QObject* rootItem = getRootObject();
    if (!rootItem) return nullptr;
    return rootItem->property("currentTheme").value<QObject*>();
}


QString ChatPage::qmlPath() const
{
    return "qrc:/kanemoot/ui/pages/ChatPage.qml";
}

void ChatPage::joinServer(const QString &inviteLink) {
    if (m_chatmgr)
        m_chatmgr->joinServer(inviteLink);
}

void ChatPage::createServer(const QString &name) {
    if (m_chatmgr)
        m_chatmgr->createServer(name);
}

void ChatPage::logoutRedirect()
{
    qDebug() << "C++ LOGOUT";

    QObject* root = getRootObject();
    if (!root) return;
    SecureStorage::instance().removeValue("access-token");
    SecureStorage::instance().removeValue("jwt-token");
    emit callSkipAnim();
    emit requestPageChange(Page_Login);
}
