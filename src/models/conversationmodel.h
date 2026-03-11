// Headers/models/conversationmodel.h
#pragma once

#include <QAbstractListModel>
#include <QHash>
#include <QVariantMap>

class ConversationModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    // ✅ 简化角色定义（使用标准命名，Qt 5.14 兼容）
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        AvatarRole,
        LastMessageRole,
        TimeRole,
        UnreadCountRole,
        TypeRole,
        IsCurrentRole,
        IsHiddenRole,
        IsPinnedRole,
        PinnedOrderRole
    };
    Q_ENUM(Roles)

    explicit ConversationModel(QObject *parent = nullptr);
    ~ConversationModel() override = default;

    int count() const { return m_conversations.count(); }

    void setCurrentUserId(const QString &userId) { m_currentUserId = userId; }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void updateConversation(const QString &id, const QVariantMap &updates);
    Q_INVOKABLE QString findOrStartConversation(const QString &userId, const QString &userName);
    Q_INVOKABLE QVariantMap get(int index) const;
    Q_INVOKABLE QVariantMap getById(const QString &id) const;
    Q_INVOKABLE void removeConversation(const QString &id);
    Q_INVOKABLE void clear();
    void replaceConversationId(const QString &oldId, const QString &newId, const QVariantMap &updates = QVariantMap());
    void setCurrentConversation(const QString &id);
    void markRead(const QString &id);
    void clearAllUnread();

    int indexById(const QString &id) const;
    int getUnreadCount(const QString &id) const;

signals:
    void countChanged();
    void modelDataChanged();

private:
    QList<QVariantMap> m_conversations;
    QHash<QString, int> m_idToIndex;
    QString m_currentUserId;

    void rebuildIdIndex();
    void insertConversation(const QVariantMap &conv);
    void updateConversationInternal(int rowIndex, const QVariantMap &updates);
};
