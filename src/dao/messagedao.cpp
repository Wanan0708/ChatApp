#include "messagedao.h"
#include "../network/networkclient.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

namespace {
MessageType parseMessageType(const QJsonValue &typeValue)
{
    if (typeValue.isDouble()) {
        return static_cast<MessageType>(typeValue.toInt(static_cast<int>(MessageType::Text)));
    }

    const QString textType = typeValue.toString().trimmed().toLower();
    if (textType == "image") return MessageType::Image;
    if (textType == "file") return MessageType::File;
    if (textType == "system") return MessageType::System;
    return MessageType::Text;
}
}

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

    NetworkClient::instance()->getArray(path, [this, conversationId, offset, limit](const QJsonArray &res) {
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
            msg.type = parseMessageType(obj["type"]);
            msg.status = static_cast<MessageStatus>(obj["status"].toInt(static_cast<int>(MessageStatus::Sent)));
            msg.senderName = obj["sender_name"].toString();
            msg.fileId = obj["file_id"].toString();
            msg.fileName = obj["file_name"].toString();
            msg.fileSize = obj["file_size"].toString();
            msg.fileUrl = obj["file_url"].toString();
            msg.thumbnailUrl = obj["thumbnail_url"].toString();
            msg.recalled = obj["recalled"].toBool(false);
            messages.append(msg);
        }
        emit messagesLoaded(conversationId, messages, offset, limit);
    }, [this, conversationId, offset, limit](const QString &error) {
        qWarning() << "Failed to load messages for" << conversationId << ":" << error;
        emit messagesLoaded(conversationId, QVector<Message>(), offset, limit);
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
