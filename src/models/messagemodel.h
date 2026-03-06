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
        ContentRole = Qt::UserRole + 1,
        SenderIdRole,
        TimestampRole,
        IsSelfRole,
    };
    Q_ENUM(Roles)

    explicit MessageModel(const QString &conversationId, QObject *parent = nullptr);
    ~MessageModel() override = default;

    void setCurrentUserId(const QString &userId) { m_currentUserId = userId; }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void addMessage(const QVariantMap &message);
    void clearMessages();

//    void updateMessage(int index, const QVariantMap &updates);

private:
    QList<QVariantMap> m_messages;
    QString m_conversationId;
    QString m_currentUserId;

};
