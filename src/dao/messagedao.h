#ifndef MESSAGEDAO_H
#define MESSAGEDAO_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QJsonArray>
#include "../models/message.h"

// 消息数据访问对象 - 已重构为异步网络模式
class MessageDAO : public QObject
{
    Q_OBJECT
public:
    explicit MessageDAO(QObject *parent = nullptr);

    // 获取对话的所有消息 (异步)
    void getConversationMessages(const QString &conversationId, int limit = 50, int offset = 0);

    // 标记消息为已读 (异步)
    void markMessagesAsRead(const QString &conversationId);

signals:
    void messagesLoaded(const QString &conversationId, const QVector<Message> &messages);
    void messagesMarkedRead(const QString &conversationId, bool success);

private:
};

#endif // MESSAGEDAO_H
