#ifndef CONVERSATIONDAO_H
#define CONVERSATIONDAO_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QJsonArray>
#include "../models/conversation.h"

// 对话数据访问对象 - 已重构为异步网络模式
class ConversationDAO : public QObject
{
    Q_OBJECT
public:
    explicit ConversationDAO(QObject *parent = nullptr);

    // 创建对话 (异步)
    void createConversation(const QString &conversationName, const QVector<QString> &memberIds);

    // 获取用户的所有对话 (异步)
    void getUserConversations();

signals:
    void conversationsLoaded(const QVector<Conversation> &conversations);
    void conversationCreated(bool success, const QString &conversationId, const QString &error = "");

private:
};

#endif // CONVERSATIONDAO_H
