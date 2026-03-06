#ifndef USERDAO_H
#define USERDAO_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariantMap>

// 用户数据访问对象 - 已重构为异步网络模式
class UserDAO : public QObject
{
    Q_OBJECT
public:
    struct UserSearchResult {
        QString userId;
        QString username;
    };

    explicit UserDAO(QObject *parent = nullptr);

    // 验证用户登录 (异步)
    void validateUserLogin(const QString &username, const QString &password);

    // 注册新用户 (异步)
    void registerNewUser(const QString &username, const QString &password);

    // 获取用户信息 (异步)
    void getUserInfo(const QString &userId);
    void getMyProfile();
    void updateMyProfile(const QVariantMap &profile);

    // 搜索用户 (异步)
    void searchUsers(const QString &searchTerm);

    // 密码重置相关 (异步)
    void requestPasswordReset(const QString &username);
    void resetPasswordWithCode(const QString &resetCode, const QString &newPassword);
    void updateUserPassword(const QString &userId, const QString &newPassword);

signals:
    void loginResult(bool success, const QString &userId, const QString &token, const QString &error = "");
    void registerResult(bool success, const QString &userId, const QString &error = "");
    void userInfoResult(bool success, const QString &username, const QString &error = "");
    void userProfileResult(bool success, const QVariantMap &profile, const QString &error = "");
    void userProfileUpdated(bool success, const QString &error = "");
    void searchResult(const QVector<UserSearchResult> &results);
    
    // 密码重置信号
    void passwordResetCodeGenerated(bool success, const QString &resetCode, const QString &error = "");
    void passwordResetResult(bool success, const QString &error = "");
    void passwordUpdateResult(bool success, const QString &error = "");

private:
};

#endif // USERDAO_H
