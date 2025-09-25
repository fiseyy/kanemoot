#include "controller/chatpage.h"
#include "utils/logging.h"
#include "core/errorcode.h"
#include "core/securestorage.h"
#include "core/errorhandler.h"
#include "core/settingsmanager.h"
#include "core/serverdata.h"
#include <QQmlComponent>
#include <qqmlcontext.h>
#include <QJsonArray>
#include <QQmlContext>
#include <QJsonObject>
#include <QQuickItem>
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
        qDebug() << "[ChatPage] userServersReceived, серверов пришло:" << servers.size();

        QObject* root = getRootObject();
        if(!root) return;

        QObject* chatPageContent = root->findChild<QObject*>("chatPageContent");
        if(!chatPageContent) return;

        QObject* userServersColumn = chatPageContent->findChild<QObject*>("userServersColumn");
        if(!userServersColumn) return;

        QQuickItem* columnItem = qobject_cast<QQuickItem*>(userServersColumn);
        if (columnItem) {
            const auto items = columnItem->childItems();
            for (QQuickItem* item : items) {
                item->setParentItem(nullptr);
                delete item;
            }
        }

        QQmlEngine* engine = QQmlEngine::contextForObject(root)->engine();

        for(const auto &s_val : servers){
            QJsonObject s = s_val.toObject();

            QVariantList channels;
            for (const auto &ch_val : s.value("channels").toArray()) {
                QJsonObject chObj = ch_val.toObject();
                QVariantMap chMap;
                chMap["id"] = chObj.value("id").toInt();
                chMap["name"] = chObj.value("name").toString();
                chMap["type"] = chObj.value("type").toString();
                channels.append(chMap);
            }

            QList<QObject*> channelsList;
            for (const auto &ch_val : s.value("channels").toArray()) {
                QJsonObject chObj = ch_val.toObject();
                channelsList.append(new ChannelData(
                    chObj.value("id").toInt(),
                    chObj.value("name").toString(),
                    chObj.value("type").toString(),
                    this
                    ));
            }

            ServerData* server = new ServerData(
                s.value("id").toInt(),
                s.value("name").toString(),
                s.value("avatar_url").toString(),
                s.value("role").toString(),
                channelsList,
                this
                );

            QQmlComponent comp(engine, QUrl("qrc:///kanemoot/ui/components/ServerItem.qml"));
            if (comp.status() != QQmlComponent::Ready) {
                qWarning() << "ServerItem component is not ready:" << comp.errorString();
                continue;
            }

            QQmlContext* context = new QQmlContext(engine->rootContext());
            context->setContextProperty("chatPage", root);

            QObject* serverItemObj = comp.create(context);
            if (!serverItemObj) {
                qWarning() << "Не удалось создать ServerItem:" << comp.errorString();
                continue;
            }

            QQuickItem* serverItem = qobject_cast<QQuickItem*>(serverItemObj);
            if (serverItem && columnItem) {
                serverItem->setParentItem(columnItem);
                serverItem->setProperty("serverData", QVariant::fromValue<QObject*>(server));
                serverItem->setObjectName(QString("server_%1").arg(s["id"].toInt()));

                QObject::connect(serverItemObj, SIGNAL(serverClicked(QVariant)),
                                 this, SLOT(onServerSelected(QVariant)));
            }
        }
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

void ChatPage::onServerSelected(const QVariant &serverDataVar) {
    ServerData* server = qobject_cast<ServerData*>(serverDataVar.value<QObject*>());
    if (!server) return;

    qDebug() << "Сервер выбран:" << server->name();

    QObject* root = getRootObject();
    if (!root) return;

    QObject* chatPageContent = root->findChild<QObject*>("chatPageContent");
    if (!chatPageContent) return;

    chatPageContent->setProperty("openedChatType", "server");
    chatPageContent->setProperty("serverData", QVariant::fromValue<QObject*>(server));

    QObject* chatAreaObj = root->findChild<QObject*>("chatArea");
    if (chatAreaObj) {
        chatAreaObj->setProperty("serverData", QVariant::fromValue<QObject*>(server));
        chatAreaObj->setProperty("chatType", "server");
    }
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
