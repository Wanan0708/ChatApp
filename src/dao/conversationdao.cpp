#include "conversationdao.h"
#include "../network/networkclient.h"
#include "../utils/timeformatter.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

ConversationDAO::ConversationDAO(QObject *parent)
    : QObject(parent)
{
}

void ConversationDAO::getUserConversations()
{
    NetworkClient::instance()->getArray("/conversations", [this](const QJsonArray &res) {
        QVector<Conversation> conversations;
        for (const QJsonValue &val : res) {
            QJsonObject obj = val.toObject();
            qDebug() << "[ConversationDAO] Conversation object:" << QJsonDocument(obj).toJson();
            
            Conversation conv;
            conv.id = obj["conversation_id"].toVariant().toString();
            conv.title = obj["name"].toVariant().toString();
            conv.type = obj["type"].toVariant().toString();
            
            // 使用 toVariant().toString() 确保正确获取字符串
            conv.lastMessage = obj["last_message"].toVariant().toString();
            qDebug() << "[ConversationDAO] lastMessage:" << conv.lastMessage;

            // 格式化时间戳
            QString lastMessageTime = obj["last_message_time"].toVariant().toString();
            if (!lastMessageTime.isEmpty()) {
                // 尝试多种格式解析 ISO 日期字符串
                QDateTime dt = QDateTime::fromString(lastMessageTime, Qt::ISODateWithMs);
                if (!dt.isValid()) {
                    // 尝试不带毫秒的格式
                    dt = QDateTime::fromString(lastMessageTime, Qt::ISODate);
                }
                if (dt.isValid()) {
                    // 如果时间是 UTC 时间（带 Z 或 +00），转换为本地时间
                    if (lastMessageTime.contains('Z') || lastMessageTime.contains("+00:00")) {
                        dt = dt.toLocalTime();
                    }
                    conv.time = TimeFormatter::formatChatTime(dt);
                    qDebug() << "[ConversationDAO] Parsed time:" << lastMessageTime << "->" << conv.time;
                } else {
                    qWarning() << "[ConversationDAO] Failed to parse time:" << lastMessageTime;
                    conv.time = lastMessageTime;
                }
            } else {
                conv.time = "";
            }

            conv.unreadCount = 0;
            conversations.append(conv);
        }
        emit conversationsLoaded(conversations);
    }, [this](const QString &error) {
        qWarning() << "Failed to load conversations:" << error;
        emit conversationsLoaded(QVector<Conversation>());
    });
}

void ConversationDAO::createConversation(const QString &conversationName, const QVector<QString> &memberIds)
{
    qDebug() << "[ConversationDAO] createConversation:" << conversationName;

    QJsonObject data;
    data["name"] = conversationName;

    QJsonArray members;
    for (const QString &id : memberIds) {
        members.append(id);
    }
    data["members"] = members;

    NetworkClient::instance()->post("/conversations", data, [this](const QJsonObject &res) {
        qDebug() << "[ConversationDAO] Create conversation response:" << QJsonDocument(res).toJson();

        QString convId = res["conversationId"].toString();
        if (!convId.isEmpty()) {
            emit conversationCreated(true, convId);
        } else {
            emit conversationCreated(false, "", "No conversation ID returned");
        }
    }, [this](const QString &error) {
        qDebug() << "[ConversationDAO] createConversation error:" << error;
        emit conversationCreated(false, "", error);
    });
}
