#include "messagedao.h"
#include "../network/networkclient.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

MessageDAO::MessageDAO(QObject *parent)
    : QObject(parent)
{
}

void MessageDAO::getConversationMessages(const QString &conversationId, int limit, int offset)
{
    qDebug() << "[MessageDAO] getConversationMessages for:" << conversationId << "limit:" << limit;
    
    QString path = QString("/messages/%1?limit=%2&offset=%3")
                   .arg(conversationId)
                   .arg(limit)
                   .arg(offset);

    NetworkClient::instance()->getArray(path, [this, conversationId](const QJsonArray &res) {
        qDebug() << "[MessageDAO] Messages loaded for:" << conversationId;
        
        QVector<Message> messages;
        for (const QJsonValue &val : res) {
            QJsonObject obj = val.toObject();
            Message msg;
            msg.messageId = obj["message_id"].toString();
            msg.conversationId = obj["conversation_id"].toString();
            msg.senderId = obj["sender_id"].toString();
            msg.content = obj["content"].toString();
            msg.timestamp = obj["timestamp"].toVariant().toLongLong();
            messages.append(msg);
        }
        emit messagesLoaded(conversationId, messages);
    }, [this, conversationId](const QString &error) {
        qWarning() << "Failed to load messages for" << conversationId << ":" << error;
        emit messagesLoaded(conversationId, QVector<Message>());
    });
}

void MessageDAO::markMessagesAsRead(const QString &conversationId)
{
    qDebug() << "[MessageDAO] markMessagesAsRead for:" << conversationId;
    
    QJsonObject data;
    data["conversationId"] = conversationId;

    NetworkClient::instance()->put("/messages/read", data, [this, conversationId](const QJsonObject &res) {
        qDebug() << "[MessageDAO] Messages marked as read for:" << conversationId;
        emit messagesMarkedRead(conversationId, true);
    }, [this, conversationId](const QString &error) {
        qWarning() << "Failed to mark messages as read for" << conversationId << ":" << error;
        emit messagesMarkedRead(conversationId, false);
    });
}
