#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "src/singleton.h"
#include "src/models/messagemodel.h"
#include "src/models/conversationmodel.h"
#include "src/network/websocketclient.h"
#include "src/services/chatservice.h"
#include "src/network/networkclient.h"
#include "src/database/databaseconfig.h"
#include "src/core/errorhandler.h"
#include "src/core/logger.h"
#include "src/core/servicelocator.h"
#include "src/network/requestbatcher.h"
#include "src/utils/imagecache.h"
#include "src/utils/messagecache.h"
#include "src/core/thememanager.h"
#include "src/core/translationmanager.h"
#include "src/core/notificationmanager.h"
#include "src/utils/inputvalidator.h"
#include "src/utils/messagesearch.h"
#include "src/utils/emojiprovider.h"
#include "src/models/messagelistproxy.h"

// 注册单例到 QML
static QObject *chatServiceSingletonProvider(QQmlEngine *, QJSEngine *)
{
    return ChatService::instance();
}

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray(""));

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    app.setApplicationName("XWChat");
    app.setOrganizationName("XWChat");
    app.setOrganizationDomain("xwchat.com");

    // 现在使用 NetworkClient 进行后端通信，不再需要本地数据库初始化
    Logger::instance()->info("Application started", "main");
    Logger::instance()->debug(QString("Backend host default: %1").arg(DatabaseConfig::defaultHost()), "main");
    Logger::instance()->debug(QString("Backend API URL: %1").arg(DatabaseConfig::apiUrl()), "main");
    Logger::instance()->debug(QString("Backend WS URL: %1").arg(DatabaseConfig::wsUrl()), "main");

    // 注册 C++ 类型到 QML
    // 注册为不可创建类型，避免 QML 试图直接 new，但让 QML 知道返回类型
    qmlRegisterUncreatableType<MessageModel>("ChatApp.Models", 1, 0, "MessageModel",
                                             "Use ChatService.getMessageModel()");
    qmlRegisterUncreatableType<ConversationModel>("ChatApp.Models", 1, 0, "ConversationModel",
                                                  "Use ChatService.conversationModel");
    qmlRegisterSingletonType<ChatService>(
        "ChatApp.Network", 1, 0, "ChatService", chatServiceSingletonProvider);
    
    // 注册错误处理器
    qmlRegisterSingletonType<ErrorHandler>("ChatApp.Core", 1, 0, "ErrorHandler",
        [](QQmlEngine *, QJSEngine *) -> QObject * {
            return ErrorHandler::instance();
        });
    
    // 注册日志器
    qmlRegisterSingletonType<Logger>("ChatApp.Core", 1, 0, "Logger",
        [](QQmlEngine *, QJSEngine *) -> QObject * {
            return Logger::instance();
        });
    
    // 注册服务定位器
    qmlRegisterSingletonType<ServiceLocator>("ChatApp.Core", 1, 0, "ServiceLocator",
        [](QQmlEngine *, QJSEngine *) -> QObject * {
            return ServiceLocator::instance();
        });
    
    // 注册请求批处理器
    qmlRegisterSingletonType<RequestBatcher>("ChatApp.Network", 1, 0, "RequestBatcher",
        [](QQmlEngine *, QJSEngine *) -> QObject * {
            return RequestBatcher::instance();
        });
    
    // 注册图片缓存
    qmlRegisterSingletonType<ImageCache>("ChatApp.Utils", 1, 0, "ImageCache",
        [](QQmlEngine *, QJSEngine *) -> QObject * {
            return ImageCache::instance();
        });
    
    // 注册消息缓存
    qmlRegisterSingletonType<MessageCache>("ChatApp.Utils", 1, 0, "MessageCache",
        [](QQmlEngine *, QJSEngine *) -> QObject * {
            return MessageCache::instance();
        });
    
    // 注册主题管理器
    qmlRegisterSingletonType<ThemeManager>("ChatApp.Core", 1, 0, "ThemeManager",
        [](QQmlEngine *, QJSEngine *) -> QObject * {
            return ThemeManager::instance();
        });
    
    // 注册翻译管理器
    qmlRegisterSingletonType<TranslationManager>("ChatApp.Core", 1, 0, "TranslationManager",
        [](QQmlEngine *, QJSEngine *) -> QObject * {
            return TranslationManager::instance();
        });
    
    // 注册通知管理器
    qmlRegisterSingletonType<NotificationManager>("ChatApp.Core", 1, 0, "NotificationManager",
        [](QQmlEngine *, QJSEngine *) -> QObject * {
            return NotificationManager::instance();
        });
    
    // 注册输入验证器
    qmlRegisterSingletonType<InputValidator>("ChatApp.Utils", 1, 0, "InputValidator",
        [](QQmlEngine *, QJSEngine *) -> QObject * {
            return InputValidator::instance();
        });
    
    // 注册消息搜索器
    qmlRegisterSingletonType<MessageSearch>("ChatApp.Utils", 1, 0, "MessageSearch",
        [](QQmlEngine *, QJSEngine *) -> QObject * {
            return MessageSearch::instance();
        });
    
    // 注册表情提供者
    qmlRegisterSingletonType<EmojiProvider>("ChatApp.Utils", 1, 0, "EmojiProvider",
        [](QQmlEngine *, QJSEngine *) -> QObject * {
            return EmojiProvider::instance();
        });
    
    // 注册消息列表代理
    qmlRegisterType<MessageListProxy>("ChatApp.Models", 1, 0, "MessageListProxy");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
