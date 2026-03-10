// Sources/models/conversationmodel.cpp
#include "conversationmodel.h"
#include <QDateTime>
#include <QDebug>

ConversationModel::ConversationModel(QObject *parent)
    : QAbstractListModel(parent)
{
    qDebug() << "[ConversationModel] Created";
}

int ConversationModel::rowCount(const QModelIndex &) const
{
    return m_conversations.count();
}

QVariant ConversationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_conversations.count())
        return QVariant();

    const auto &conv = m_conversations.at(index.row());

    switch (role) {
    case IdRole: return conv.value("id");
    case TitleRole: return conv.value("title");
    case AvatarRole: return conv.value("avatar");
    case LastMessageRole: return conv.value("lastMessage");
    case TimeRole: return conv.value("time");
    case UnreadCountRole: return conv.value("unreadCount", 0);
    case TypeRole: return conv.value("type");
    case IsCurrentRole: return conv.value("isCurrent", false);
    default: return QVariant();
    }
}

QHash<int, QByteArray> ConversationModel::roleNames() const
{
    return {
        {IdRole, "id"},
        {TitleRole, "title"},
        {AvatarRole, "avatar"},
        {LastMessageRole, "lastMessage"},
        {TimeRole, "time"},
        {UnreadCountRole, "unreadCount"},
        {TypeRole, "type"},
        {IsCurrentRole, "isCurrent"}
    };
}

int ConversationModel::indexById(const QString &id) const
{
    return m_idToIndex.value(id, -1);
}

int ConversationModel::getUnreadCount(const QString &id) const
{
    int idx = indexById(id);
    return (idx != -1) ? m_conversations.at(idx).value("unreadCount", 0).toInt() : 0;
}

void ConversationModel::rebuildIdIndex()
{
    m_idToIndex.clear();
    for (int index = 0; index < m_conversations.count(); ++index) {
        m_idToIndex[m_conversations.at(index).value("id").toString()] = index;
    }
}

void ConversationModel::insertConversation(const QVariantMap &conv)
{
    int idx = m_conversations.count();
    beginInsertRows(QModelIndex(), idx, idx);
    m_conversations.append(conv);
    m_idToIndex[conv.value("id").toString()] = idx;
    endInsertRows();
    emit countChanged();
}

void ConversationModel::updateConversationInternal(int rowIndex, const QVariantMap &updates)
{
    if (rowIndex < 0 || rowIndex >= m_conversations.count())
        return;

    auto &conv = m_conversations[rowIndex];
    bool changed = false;

    qDebug() << "[ConversationModel] updateConversationInternal:" << updates;

    for (auto it = updates.constBegin(); it != updates.constEnd(); ++it) {
        if (conv.value(it.key()) != it.value()) {
            conv[it.key()] = it.value();
            changed = true;
            qDebug() << "[ConversationModel] Updated" << it.key() << "to" << it.value();
        }
    }

    if (changed) {
        emit dataChanged(this->index(rowIndex, 0), this->index(rowIndex, 0));
        emit modelDataChanged();
        qDebug() << "[ConversationModel] Conversation updated, lastMessage:" << conv.value("lastMessage");
    }
}

void ConversationModel::updateConversation(const QString &id, const QVariantMap &updates)
{
    int idx = indexById(id);
    if (idx == -1) {
        // 创建新会话
        QVariantMap conv;
        conv["id"] = id;
        conv["title"] = updates.value("title", "New Chat");
        conv["avatar"] = updates.value("avatar", "");
        conv["type"] = updates.value("type", "user");
        conv["lastMessage"] = "";
        conv["time"] = "";
        conv["unreadCount"] = 0;
        conv["isCurrent"] = false;

        insertConversation(conv);
        idx = m_conversations.count() - 1;
    }

    updateConversationInternal(idx, updates);
}

void ConversationModel::replaceConversationId(const QString &oldId, const QString &newId, const QVariantMap &updates)
{
    if (oldId.isEmpty() || newId.isEmpty() || oldId == newId) {
        if (!newId.isEmpty() && !updates.isEmpty()) {
            updateConversation(newId, updates);
        }
        return;
    }

    const int oldIndex = indexById(oldId);
    if (oldIndex == -1) {
        updateConversation(newId, updates);
        return;
    }

    const int newIndex = indexById(newId);
    if (newIndex != -1) {
        QVariantMap mergedUpdates = m_conversations.at(oldIndex);
        mergedUpdates["id"] = newId;
        for (auto it = updates.constBegin(); it != updates.constEnd(); ++it) {
            mergedUpdates[it.key()] = it.value();
        }

        updateConversationInternal(newIndex, mergedUpdates);

        beginRemoveRows(QModelIndex(), oldIndex, oldIndex);
        m_conversations.removeAt(oldIndex);
        endRemoveRows();
        rebuildIdIndex();
        emit countChanged();
        emit modelDataChanged();
        return;
    }

    QVariantMap mergedConversation = m_conversations.at(oldIndex);
    mergedConversation["id"] = newId;
    for (auto it = updates.constBegin(); it != updates.constEnd(); ++it) {
        mergedConversation[it.key()] = it.value();
    }

    m_conversations[oldIndex] = mergedConversation;
    rebuildIdIndex();
    emit dataChanged(index(oldIndex, 0), index(oldIndex, 0));
    emit modelDataChanged();
}

QString ConversationModel::findOrStartConversation(const QString &userId, const QString &/*userName*/)
{
    // 遍历现有会话，查找是否已经与该用户存在会话
    for (int i = 0; i < m_conversations.count(); ++i) {
        const auto &conv = m_conversations[i];
        QString convId = conv.value("id").toString();

        // 检查会话 ID 是否包含该用户 ID（格式：conv_user1_user2）
        if (convId.contains(userId) && conv.value("type").toString() == "single") {
            qDebug() << "[ConversationModel] Found existing conversation:" << convId << "for user:" << userId;
            return convId;
        }
    }

    // 如果没有找到现有会话，使用用户 ID 作为会话 ID（后端会自动创建）
    qDebug() << "[ConversationModel] No existing conversation found, using userId:" << userId;
    return userId;
}

void ConversationModel::setCurrentConversation(const QString &id)
{
    bool changed = false;
    for (int i = 0; i < m_conversations.count(); ++i) {
        bool current = (m_conversations[i].value("id").toString() == id);
        if (m_conversations[i].value("isCurrent").toBool() != current) {
            m_conversations[i]["isCurrent"] = current;
            emit dataChanged(this->index(i, 0), this->index(i, 0));
            changed = true;
        }
    }

    if (changed && !id.isEmpty()) {
        markRead(id);
    }
}

void ConversationModel::markRead(const QString &id)
{
    int idx = indexById(id);
    if (idx != -1 && m_conversations[idx].value("unreadCount", 0).toInt() > 0) {
        m_conversations[idx]["unreadCount"] = 0;
        emit dataChanged(this->index(idx, 0), this->index(idx, 0));
    }
}

void ConversationModel::clearAllUnread()
{
    bool changed = false;
    for (int i = 0; i < m_conversations.count(); ++i) {
        if (m_conversations[i].value("unreadCount", 0).toInt() > 0) {
            m_conversations[i]["unreadCount"] = 0;
            changed = true;
        }
    }
    if (changed) {
        emit dataChanged(this->index(0, 0), this->index(m_conversations.count() - 1, 0));
    }
}

QVariantMap ConversationModel::get(int index) const
{
    if (index >= 0 && index < m_conversations.count()) {
        auto conv = m_conversations.at(index);
        qDebug() << "[ConversationModel] get(" << index << ") lastMessage:" << conv.value("lastMessage");
        return conv;
    }
    qDebug() << "[ConversationModel] get(" << index << ") - out of range";
    return QVariantMap();
}

QVariantMap ConversationModel::getById(const QString &id) const
{
    const int idx = indexById(id);
    if (idx >= 0 && idx < m_conversations.count()) {
        return m_conversations.at(idx);
    }

    return QVariantMap();
}
