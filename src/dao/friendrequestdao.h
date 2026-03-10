#ifndef FRIENDREQUESTDAO_H
#define FRIENDREQUESTDAO_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QJsonArray>
#include <QJsonObject>

// 好友申请数据访问对象 - 已重构为异步网络模式
class FriendRequestDAO : public QObject
{
    Q_OBJECT
public:
    struct FriendRequest {
        int requestId;
        QString fromId;
        QString fromUsername;
        QString toId;
        QString status;
        QString createdAt;
    };

    struct FriendInfo {
        QString userId;
        QString username;
        QString avatar;
        QString status;
        QString signature;
        bool isMale;
        int age;
        QString region;
    };

    explicit FriendRequestDAO(QObject *parent = nullptr);

    // 发送好友申请 (异步)
    void sendRequest(const QString &toId);

    // 获取待处理申请 (异步)
    void getPendingRequests();

    // 更新申请状态 (异步)
    void updateRequestStatus(int requestId, const QString &status);

    // 获取好友列表 (异步)
    void getFriends();

    // 获取单个好友资料 (异步)
    void getFriendProfile(const QString &friendId);

signals:
    void requestSent(bool success, const QString &error = "");
    void pendingRequestsLoaded(const QVector<FriendRequest> &requests);
    void requestStatusUpdated(bool success, int requestId, const QString &status);
    void friendsLoaded(const QVector<FriendInfo> &friends);
    void friendProfileLoaded(const FriendInfo &friendInfo);

private:
};

#endif // FRIENDREQUESTDAO_H
