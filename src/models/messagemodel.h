// Headers/models/messagemodel.h
#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QVariantMap>

class MessageModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        MessageIdRole = Qt::UserRole + 1,
        InternalMessageIdRole,
        ServerMessageIdRole,
        ContentRole,
        SenderIdRole,
        TimestampRole,
        IsSelfRole,
        TypeRole,
        StatusRole,
        FileIdRole,
        FileNameRole,
        FileSizeRole,
        FileUrlRole,
        ThumbnailUrlRole,
        RecalledRole,
        OfflineRole,
        ErrorTextRole,
    };
    Q_ENUM(Roles)

    explicit MessageModel(const QString &conversationId, QObject *parent = nullptr);
    ~MessageModel() override = default;

    void setCurrentUserId(const QString &userId) { m_currentUserId = userId; }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void addMessage(const QVariantMap &message);
    void upsertMessage(const QVariantMap &message);
    void clearMessages();
    QList<QVariantMap> messages() const;
    void setConversationId(const QString &conversationId) { m_conversationId = conversationId; }

//    void updateMessage(int index, const QVariantMap &updates);

private:
    qint64 normalizedTimestamp(const QVariantMap &message) const;
    int findMessageIndex(const QVariantMap &message) const;
    int findInsertIndex(qint64 timestamp) const;

    QList<QVariantMap> m_messages;
    QString m_conversationId;
    QString m_currentUserId;

};
