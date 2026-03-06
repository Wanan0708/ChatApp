#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
#include <QVariantMap>
#include <QDateTime>

/**
 * @brief 消息数据结构（纯数据，无业务逻辑）
 *
 * 存储单条消息的所有数据
 */
struct Message {
    QString messageId;  ///< 唯一消息ID
    QString conversationId; ///< 所属对话ID
    QString content;    ///< 消息内容
    QString senderId;   ///< 发送者ID
    qint64 timestamp;   ///< 时间戳 (毫秒)

    Message() = default;
    Message(const QString &messageId, const QString &conversationId, const QString &content, const QString &senderId, qint64 timestamp)
        : messageId(messageId), conversationId(conversationId), content(content), senderId(senderId), timestamp(timestamp) {}

    /**
     * @brief 转换为QVariantMap（用于QML信号传递）
     */
    QVariantMap toVariantMap() const {
        QVariantMap map;
        map["messageId"] = messageId;
        map["conversationId"] = conversationId;
        map["content"] = content;
        map["senderId"] = senderId;
        map["timestamp"] = timestamp;
        return map;
    }

    /**
     * @brief 格式化显示时间
     */
    QString formattedTime() const {
        return QDateTime::fromMSecsSinceEpoch(timestamp).toString("hh:mm:ss");
    }
};

#endif // MESSAGE_H
