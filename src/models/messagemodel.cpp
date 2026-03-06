// Sources/models/messagemodel.cpp
#include "messagemodel.h"
#include <QDebug>

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

    switch (role) {
    case ContentRole: return msg.value("content");
    case SenderIdRole: return msg.value("senderId");
    case TimestampRole: return msg.value("timestamp");
    case IsSelfRole:
        return (msg.value("senderId").toString() == m_currentUserId);
    default: return QVariant();
    }
}

QHash<int, QByteArray> MessageModel::roleNames() const
{
    return {
        {ContentRole, "content"},
        {SenderIdRole, "senderId"},
        {TimestampRole, "timestamp"},
        {IsSelfRole, "isSelf"}
    };
}

void MessageModel::addMessage(const QVariantMap &message)
{
    beginInsertRows(QModelIndex(), m_messages.count(), m_messages.count());
    m_messages.append(message);
    endInsertRows();
}

void MessageModel::clearMessages()
{
    if (m_messages.isEmpty()) return;

    beginResetModel();
    m_messages.clear();
    endResetModel();
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
