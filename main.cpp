#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "src/singleton.h"
#include "src/models/messagemodel.h"
#include "src/models/conversationmodel.h"
#include "src/network/websocketclient.h"
#include "src/services/chatservice.h"
#include "src/network/networkclient.h"
#include "src/database/databaseconfig.h"

// 注册单例到QML
static QObject *chatServiceSingletonProvider(QQmlEngine *, QJSEngine *)
{
    return ChatService::instance();
}

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray(""));

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    app.setApplicationName("XWChat");
    app.setOrganizationName("XWChat");
    app.setOrganizationDomain("xwchat.com");

    // 现在使用 NetworkClient 进行后端通信，不再需要本地数据库初始化
    qDebug() << "Application started in" << 
#ifdef QT_DEBUG
                "DEBUG"
#else
                "RELEASE"
#endif
                "mode";
    qDebug() << "Backend host default:" << DatabaseConfig::defaultHost();
    qDebug() << "Backend API URL:" << DatabaseConfig::apiUrl();
    qDebug() << "Backend WS URL:" << DatabaseConfig::wsUrl();

    // 注册C++类型到QML
    // 注册为不可创建类型，避免 QML 试图直接 new，但让 QML 知道返回类型
    qmlRegisterUncreatableType<MessageModel>("ChatApp.Models", 1, 0, "MessageModel",
                                             "Use ChatService.getMessageModel()");
    qmlRegisterUncreatableType<ConversationModel>("ChatApp.Models", 1, 0, "ConversationModel",
                                                  "Use ChatService.conversationModel");
    qmlRegisterSingletonType<ChatService>(
        "ChatApp.Network", 1, 0, "ChatService", chatServiceSingletonProvider);

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
