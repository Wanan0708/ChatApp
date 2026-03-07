#include "userdao.h"
#include "../network/networkclient.h"
#include <QDebug>
#include <QJsonArray>

UserDAO::UserDAO(QObject *parent)
    : QObject(parent)
{
}

void UserDAO::validateUserLogin(const QString &username, const QString &password)
{
    qDebug() << "[UserDAO] validateUserLogin called with username:" << username;
    
    QJsonObject data;
    data["username"] = username;
    data["password"] = password;

    // 注意：path 不需要 /api 前缀，因为 NetworkClient 的 baseUrl 已经包含了 /api
    NetworkClient::instance()->post("/auth/login", data, [this](const QJsonObject &res) {
        qDebug() << "[UserDAO] Login response received:" << QJsonDocument(res).toJson();
        
        QString userId = res["userId"].toString();
        QString token = res["accessToken"].toString();

        if (!token.isEmpty()) {
            NetworkClient::instance()->setToken(token);
            qDebug() << "[UserDAO] Login successful, userId:" << userId;
            emit loginResult(true, userId, token);
        } else {
            qDebug() << "[UserDAO] Login failed: Invalid response from server";
            emit loginResult(false, "", "", "Invalid response from server");
        }
    }, [this](const QString &error) {
        qDebug() << "[UserDAO] Login error:" << error;
        emit loginResult(false, "", "", error);
    });
}

void UserDAO::registerNewUser(const QString &username, const QString &password)
{
    qDebug() << "[UserDAO] registerNewUser called with username:" << username;
    
    QJsonObject data;
    data["username"] = username;
    data["password"] = password;

    NetworkClient::instance()->post("/auth/register", data, [this](const QJsonObject &res) {
        qDebug() << "[UserDAO] Register response received:" << QJsonDocument(res).toJson();
        
        QString userId = res["userId"].toString();
        if (!userId.isEmpty()) {
            emit registerResult(true, userId);
        } else {
            emit registerResult(false, "", "Registration failed: No User ID returned");
        }
    }, [this](const QString &error) {
        qDebug() << "[UserDAO] Register error:" << error;
        emit registerResult(false, "", error);
    });
}

void UserDAO::getUserInfo(const QString &userId)
{
    qDebug() << "[UserDAO] getUserInfo called with userId:" << userId;
    
    NetworkClient::instance()->get("/users/profile", [this](const QJsonObject &res) {
        qDebug() << "[UserDAO] User info response received:" << QJsonDocument(res).toJson();
        
        QString username = res["username"].toString();
        emit userInfoResult(true, username);
    }, [this](const QString &error) {
        qDebug() << "[UserDAO] getUserInfo error:" << error;
        emit userInfoResult(false, "", error);
    });
}

void UserDAO::getMyProfile()
{
    NetworkClient::instance()->get("/users/profile", [this](const QJsonObject &res) {
        emit userProfileResult(true, res.toVariantMap());
    }, [this](const QString &error) {
        emit userProfileResult(false, QVariantMap(), error);
    });
}

void UserDAO::updateMyProfile(const QVariantMap &profile)
{
    qDebug() << "[UserDAO] updateMyProfile called with profile:" << profile;
    
    const QJsonObject payload = QJsonObject::fromVariantMap(profile);
    qDebug() << "[UserDAO] Sending payload:" << QJsonDocument(payload).toJson();
    
    NetworkClient::instance()->put("/users/profile", payload, [this](const QJsonObject &res) {
        qDebug() << "[UserDAO] Profile update response:" << QJsonDocument(res).toJson();
        const QString message = res.value("message").toString();
        emit userProfileUpdated(true, message.isEmpty() ? "Profile updated successfully" : message);
    }, [this](const QString &error) {
        qWarning() << "[UserDAO] Profile update error:" << error;
        emit userProfileUpdated(false, error);
    });
}

void UserDAO::searchUsers(const QString &searchTerm)
{
    qDebug() << "[UserDAO] searchUsers called with searchTerm:" << searchTerm;
    
    QString path = "/users/search?q=" + searchTerm;
    NetworkClient::instance()->getArray(path, [this](const QJsonArray &res) {
        QVector<UserSearchResult> results;
        for (const QJsonValue &val : res) {
            QJsonObject obj = val.toObject();
            results.append({obj["user_id"].toString(), obj["username"].toString()});
        }
        emit searchResult(results);
    }, [this](const QString &error) {
        qWarning() << "[UserDAO] Search failed:" << error;
        emit searchResult(QVector<UserSearchResult>());
    });
}

// 密码重置相关方法 - 异步实现
void UserDAO::requestPasswordReset(const QString &username)
{
    qDebug() << "[UserDAO] requestPasswordReset called with username:" << username;
    
    QJsonObject data;
    data["username"] = username;

    NetworkClient::instance()->post("/auth/forgot-password", data, [this](const QJsonObject &res) {
        qDebug() << "[UserDAO] Password reset response received:" << QJsonDocument(res).toJson();
        
        QString resetCode = res["resetCode"].toString();
        if (!resetCode.isEmpty()) {
            emit passwordResetCodeGenerated(true, resetCode);
        } else {
            emit passwordResetCodeGenerated(false, "", "Failed to generate reset code");
        }
    }, [this](const QString &error) {
        qDebug() << "[UserDAO] requestPasswordReset error:" << error;
        emit passwordResetCodeGenerated(false, "", error);
    });
}

void UserDAO::resetPasswordWithCode(const QString &resetCode, const QString &newPassword)
{
    qDebug() << "[UserDAO] resetPasswordWithCode called";
    
    QJsonObject data;
    data["resetCode"] = resetCode;
    data["newPassword"] = newPassword;

    NetworkClient::instance()->post("/auth/reset-password", data, [this](const QJsonObject &res) {
        qDebug() << "[UserDAO] Password reset result:" << QJsonDocument(res).toJson();
        emit passwordResetResult(true);
    }, [this](const QString &error) {
        qDebug() << "[UserDAO] resetPasswordWithCode error:" << error;
        emit passwordResetResult(false, error);
    });
}

void UserDAO::updateUserPassword(const QString &userId, const QString &newPassword)
{
    qDebug() << "[UserDAO] updateUserPassword called with userId:" << userId;
    
    QJsonObject data;
    data["newPassword"] = newPassword;

    QString path = QString("/users/%1/password").arg(userId);
    NetworkClient::instance()->put(path, data, [this](const QJsonObject &res) {
        qDebug() << "[UserDAO] Password update result:" << QJsonDocument(res).toJson();
        emit passwordUpdateResult(true);
    }, [this](const QString &error) {
        qDebug() << "[UserDAO] updateUserPassword error:" << error;
        emit passwordUpdateResult(false, error);
    });
}
