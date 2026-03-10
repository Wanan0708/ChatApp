// Sources/models/messagemodel.cpp
#include "messagemodel.h"
#include <QDebug>

namespace {
int normalizeMessageTypeValue(const QVariantMap &message)
{
    const QVariant explicitType = message.value("messageType");
    if (explicitType.isValid()) {
        bool ok = false;
        const int numericType = explicitType.toInt(&ok);
        if (ok) {
            return numericType;
        }
    }

    const QVariant typeVariant = message.value("type", 0);
    bool ok = false;
    const int numericType = typeVariant.toInt(&ok);
    if (ok) {
        return numericType;
    }

    const QString textType = typeVariant.toString().trimmed().toLower();
    if (textType == "image") return 1;
    if (textType == "file") return 2;
    if (textType == "system") return 3;
    return 0;
}

int normalizeMessageStatusValue(const QVariantMap &message)
{
    const QVariant statusVariant = message.value("status", 1);
    bool ok = false;
    const int numericStatus = statusVariant.toInt(&ok);
    if (ok) {
        return numericStatus;
    }
    return 1;
}
}

MessageModel::MessageModel(const QString &conversationId, QObject *parent)
    : QAbstractListModel(parent)
    , m_conversationId(conversationId)
{
}

int MessageModel::rowCount(const QModelIndex &) const
{
    return m_messages.count();
}

QVariant MessageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_messages.count())
        return QVariant();

    const auto &msg = m_messages.at(index.row());
    const QString serverMessageId = msg.value("serverMessageId").toString();
    const QString internalMessageId = msg.value("messageId").toString();

    switch (role) {
    case MessageIdRole: return serverMessageId.isEmpty() ? internalMessageId : serverMessageId;
    case InternalMessageIdRole: return internalMessageId;
    case ServerMessageIdRole: return serverMessageId;
    case ContentRole: return msg.value("content");
    case SenderIdRole: return msg.value("senderId");
    case TimestampRole: return msg.value("timestamp");
    case IsSelfRole:
        return (msg.value("senderId").toString() == m_currentUserId);
    case TypeRole: return msg.value("type", 0);
    case StatusRole: return msg.value("status", 1);
    case FileIdRole: return msg.value("fileId");
    case FileNameRole: return msg.value("fileName");
    case FileSizeRole: return msg.value("fileSize");
    case FileUrlRole: return msg.value("fileUrl");
    case ThumbnailUrlRole: return msg.value("thumbnailUrl");
    case RecalledRole: return msg.value("recalled", false);
    case OfflineRole: return msg.value("isOffline", false);
    case ErrorTextRole: return msg.value("errorText");
    default: return QVariant();
    }
}

QHash<int, QByteArray> MessageModel::roleNames() const
{
    return {
        {MessageIdRole, "messageId"},
        {InternalMessageIdRole, "internalMessageId"},
        {ServerMessageIdRole, "serverMessageId"},
        {ContentRole, "content"},
        {SenderIdRole, "senderId"},
        {TimestampRole, "timestamp"},
        {IsSelfRole, "isSelf"},
        {TypeRole, "type"},
        {StatusRole, "status"},
        {FileIdRole, "fileId"},
        {FileNameRole, "fileName"},
        {FileSizeRole, "fileSize"},
        {FileUrlRole, "fileUrl"},
        {ThumbnailUrlRole, "thumbnailUrl"},
        {RecalledRole, "recalled"},
        {OfflineRole, "isOffline"},
        {ErrorTextRole, "errorText"}
    };
}

void MessageModel::addMessage(const QVariantMap &message)
{
    upsertMessage(message);
}

void MessageModel::upsertMessage(const QVariantMap &message)
{
    QVariantMap normalizedMessage = message;
    normalizedMessage["timestamp"] = normalizedTimestamp(normalizedMessage);
    normalizedMessage["type"] = normalizeMessageTypeValue(normalizedMessage);
    normalizedMessage["status"] = normalizeMessageStatusValue(normalizedMessage);
    normalizedMessage.remove("messageType");

    const int existingIndex = findMessageIndex(normalizedMessage);
    if (existingIndex != -1) {
        QVariantMap &existingMessage = m_messages[existingIndex];
        bool changed = false;

        for (auto it = normalizedMessage.constBegin(); it != normalizedMessage.constEnd(); ++it) {
            if (existingMessage.value(it.key()) != it.value()) {
                existingMessage[it.key()] = it.value();
                changed = true;
            }
        }

        if (changed) {
            emit dataChanged(index(existingIndex, 0), index(existingIndex, 0));
        }
        return;
    }

    const int insertIndex = findInsertIndex(normalizedMessage.value("timestamp").toLongLong());
    beginInsertRows(QModelIndex(), insertIndex, insertIndex);
    m_messages.insert(insertIndex, normalizedMessage);
    endInsertRows();
}

void MessageModel::clearMessages()
{
    if (m_messages.isEmpty()) return;

    beginResetModel();
    m_messages.clear();
    endResetModel();
}

QList<QVariantMap> MessageModel::messages() const
{
    return m_messages;
}

qint64 MessageModel::normalizedTimestamp(const QVariantMap &message) const
{
    qint64 timestamp = message.value("timestamp").toLongLong();
    if (timestamp > 0 && timestamp < 10000000000LL) {
        timestamp *= 1000;
    }
    return timestamp;
}

int MessageModel::findMessageIndex(const QVariantMap &message) const
{
    const QString messageId = message.value("messageId").toString();
    const QString serverMessageId = message.value("serverMessageId").toString();
    if (!messageId.isEmpty() || !serverMessageId.isEmpty()) {
        for (int index = 0; index < m_messages.count(); ++index) {
            const QVariantMap &existing = m_messages.at(index);
            const QString existingMessageId = existing.value("messageId").toString();
            const QString existingServerMessageId = existing.value("serverMessageId").toString();
            if ((!messageId.isEmpty() && (existingMessageId == messageId || existingServerMessageId == messageId))
                || (!serverMessageId.isEmpty() && (existingMessageId == serverMessageId || existingServerMessageId == serverMessageId))) {
                return index;
            }
        }
        return -1;
    }

    const QString senderId = message.value("senderId").toString();
    const QString content = message.value("content").toString();
    const qint64 timestamp = message.value("timestamp").toLongLong();
    const int type = message.value("type", 0).toInt();
    const QString fileUrl = message.value("fileUrl").toString();
    const QString fileName = message.value("fileName").toString();

    for (int index = 0; index < m_messages.count(); ++index) {
        const QVariantMap &existing = m_messages.at(index);
        if (existing.value("senderId").toString() == senderId
            && normalizedTimestamp(existing) == timestamp
            && existing.value("content").toString() == content
            && existing.value("type", 0).toInt() == type
            && existing.value("fileUrl").toString() == fileUrl
            && existing.value("fileName").toString() == fileName) {
            return index;
        }
    }

    return -1;
}

int MessageModel::findInsertIndex(qint64 timestamp) const
{
    int insertIndex = m_messages.count();
    for (int index = 0; index < m_messages.count(); ++index) {
        if (normalizedTimestamp(m_messages.at(index)) > timestamp) {
            insertIndex = index;
            break;
        }
    }
    return insertIndex;
}

//void MessageModel::updateMessage(int index, const QVariantMap &updates)
//{
//    if (index < 0 || index >= m_messages.count()) return;

//    auto &msg = m_messages[index];
//    for (auto it = updates.begin(); it != updates.end(); ++it) {
//        msg[it.key()] = it.value();
//    }
//    emit dataChanged(this->index(index, 0), this->index(index, 0));
//}
