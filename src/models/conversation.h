// Headers/models/conversation.h
#pragma once

#include <QString>

struct Conversation {
    QString id;
    QString title;
    QString avatar;
    QString lastMessage;
    QString time;
    int unreadCount = 0;
    QString type;
    bool isCurrent = false;

    Conversation() = default;
    Conversation(const QString &id, const QString &title, const QString &type)
        : id(id), title(title), type(type) {}
};
