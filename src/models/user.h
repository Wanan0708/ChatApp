#ifndef USER_H
#define USER_H

#include <QString>

struct User {
    QString id;
    QString username;
    QString passwordHash;
    QString avatar;
    QString status;
    QString createdAt;
    QString updatedAt;

    User() = default;
    
    User(const QString &id, const QString &username, const QString &passwordHash)
        : id(id), username(username), passwordHash(passwordHash) {}
};

#endif // USER_H