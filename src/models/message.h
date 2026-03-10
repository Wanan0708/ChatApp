#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
#include <QVariantMap>
#include <QDateTime>

/**
 * @brief 消息类型枚举
 */
enum class MessageType {
    Text = 0,      ///< 文本消息
    Image = 1,     ///< 图片消息
    File = 2,      ///< 文件消息
    System = 3     ///< 系统消息
};

/**
 * @brief 消息发送状态
 */
enum class MessageStatus {
    Sending = 0,   ///< 发送中
    Sent = 1,      ///< 已发送
    Read = 2,      ///< 已读
    Failed = 3     ///< 发送失败
};

/**
 * @brief 消息数据结构（纯数据，无业务逻辑）
 *
 * 存储单条消息的所有数据
 */
struct Message {
    QString messageId;      ///< 唯一消息 ID
    QString conversationId; ///< 所属对话 ID
    QString content;        ///< 消息内容
    QString senderId;       ///< 发送者 ID
    qint64 timestamp;       ///< 时间戳 (毫秒)
    MessageType type;       ///< 消息类型
    MessageStatus status;   ///< 发送状态
    QString fileId;         ///< 文件 ID（图片/文件消息）
    QString fileName;       ///< 文件名称
    QString fileSize;       ///< 文件大小（格式化字符串）
    QString fileUrl;        ///< 文件 URL（用于下载/预览）
    QString thumbnailUrl;   ///< 缩略图 URL（图片消息）
    bool recalled = false;  ///< 是否已撤回

    Message() 
        : type(MessageType::Text)
        , status(MessageStatus::Sent)
        , timestamp(QDateTime::currentMSecsSinceEpoch()) {}

    Message(const QString &messageId, const QString &conversationId, const QString &content, 
            const QString &senderId, qint64 timestamp, MessageType type = MessageType::Text)
        : messageId(messageId), conversationId(conversationId), content(content), 
          senderId(senderId), timestamp(timestamp), type(type), status(MessageStatus::Sent) {}

    /**
     * @brief 转换为 QVariantMap（用于 QML 信号传递）
     */
    QVariantMap toVariantMap() const {
        QVariantMap map;
        map["messageId"] = messageId;
        map["conversationId"] = conversationId;
        map["content"] = content;
        map["senderId"] = senderId;
        map["timestamp"] = timestamp;
        map["type"] = static_cast<int>(type);
        map["status"] = static_cast<int>(status);
        map["fileId"] = fileId;
        map["fileName"] = fileName;
        map["fileSize"] = fileSize;
        map["fileUrl"] = fileUrl;
        map["thumbnailUrl"] = thumbnailUrl;
        map["recalled"] = recalled;
        return map;
    }

    /**
     * @brief 从 QVariantMap 创建 Message
     */
    static Message fromVariantMap(const QVariantMap &map) {
        Message msg;
        msg.messageId = map.value("messageId").toString();
        msg.conversationId = map.value("conversationId").toString();
        msg.content = map.value("content").toString();
        msg.senderId = map.value("senderId").toString();
        msg.timestamp = map.value("timestamp").toLongLong();
        msg.type = static_cast<MessageType>(map.value("type", 0).toInt());
        msg.status = static_cast<MessageStatus>(map.value("status", 1).toInt());
        msg.fileId = map.value("fileId").toString();
        msg.fileName = map.value("fileName").toString();
        msg.fileSize = map.value("fileSize").toString();
        msg.fileUrl = map.value("fileUrl").toString();
        msg.thumbnailUrl = map.value("thumbnailUrl").toString();
        msg.recalled = map.value("recalled", false).toBool();
        return msg;
    }

    /**
     * @brief 格式化显示时间
     */
    QString formattedTime() const {
        return QDateTime::fromMSecsSinceEpoch(timestamp).toString("hh:mm:ss");
    }

    /**
     * @brief 格式化显示日期
     */
    QString formattedDate() const {
        return QDateTime::fromMSecsSinceEpoch(timestamp).toString("yyyy-MM-dd");
    }
};

#endif // MESSAGE_H
