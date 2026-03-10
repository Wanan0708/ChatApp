#include "friendrequestdao.h"
#include "../database/databaseconfig.h"
#include "../network/networkclient.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

FriendRequestDAO::FriendRequestDAO(QObject *parent)
    : QObject(parent)
{
}

void FriendRequestDAO::sendRequest(const QString &toId)
{
    qDebug() << "[FriendRequestDAO] sendRequest to:" << toId;
    
    QJsonObject data;
    data["toUserId"] = toId;

    NetworkClient::instance()->post("/friends/request", data, [this](const QJsonObject &res) {
        qDebug() << "[FriendRequestDAO] Friend request sent";
        emit requestSent(true);
    }, [this](const QString &error) {
        qWarning() << "Failed to send friend request:" << error;
        emit requestSent(false, error);
    });
}

void FriendRequestDAO::getPendingRequests()
{
    qDebug() << "[FriendRequestDAO] getPendingRequests";
    
    // 需要后端实现 GET /api/friends/requests
    // 目前先假设这个端点存在并返回待处理申请
    NetworkClient::instance()->getArray("/friends/requests", [this](const QJsonArray &res) {
        QVector<FriendRequest> requests;
        for (const QJsonValue &val : res) {
            QJsonObject obj = val.toObject();
            FriendRequest req;
            req.requestId = obj["request_id"].toInt();
            req.fromId = obj["from_id"].toString();
            req.fromUsername = obj["from_username"].toString();
            req.status = obj["status"].toString();
            req.createdAt = obj["created_at"].toString();
            requests.append(req);
        }
        emit pendingRequestsLoaded(requests);
    }, [this](const QString &error) {
        qWarning() << "Failed to load pending requests:" << error;
        emit pendingRequestsLoaded(QVector<FriendRequest>());
    });
}

void FriendRequestDAO::updateRequestStatus(int requestId, const QString &status)
{
    qDebug() << "[FriendRequestDAO] updateRequestStatus:" << requestId << "status:" << status;
    
    QJsonObject data;
    data["status"] = status;

    QString path = QString("/friends/requests/%1").arg(requestId);
    NetworkClient::instance()->put(path, data, [this, requestId, status](const QJsonObject &res) {
        qDebug() << "[FriendRequestDAO] Request status updated";
        emit requestStatusUpdated(true, requestId, status);
    }, [this, requestId, status](const QString &error) {
        qWarning() << "Failed to update request status:" << error;
        emit requestStatusUpdated(false, requestId, status);
    });
}

void FriendRequestDAO::getFriends()
{
    qDebug() << "[FriendRequestDAO] getFriends";

    NetworkClient::instance()->getArray("/friends", [this](const QJsonArray &res) {
        QVector<FriendInfo> friends;
        for (const QJsonValue &val : res) {
            QJsonObject obj = val.toObject();
            FriendInfo info;
            info.userId = obj["user_id"].toString();
            info.username = obj["username"].toString();
            info.avatar = DatabaseConfig::resolveServerUrl(obj["avatar"].toString());
            info.status = obj["status"].toString("offline");
            info.signature = obj["signature"].toString("");
            info.isMale = obj["isMale"].toBool(true);
            info.age = obj["age"].toVariant().toInt();
            info.region = obj["region"].toString("");
            friends.append(info);
        }
        emit friendsLoaded(friends);
    }, [this](const QString &error) {
        qWarning() << "Failed to load friend list:" << error;
        emit friendsLoaded(QVector<FriendInfo>());
    });
}

void FriendRequestDAO::getFriendProfile(const QString &friendId)
{
    qDebug() << "[FriendRequestDAO] getFriendProfile:" << friendId;

    NetworkClient::instance()->get("/friends/" + friendId, [this](const QJsonObject &res) {
        FriendInfo info;
        info.userId = res["user_id"].toString();
        info.username = res["username"].toString();
        info.avatar = DatabaseConfig::resolveServerUrl(res["avatar"].toString());
        info.status = res["status"].toString("offline");
        info.signature = res["signature"].toString("");
        info.isMale = res["isMale"].toBool(true);
        info.age = res["age"].toVariant().toInt();
        info.region = res["region"].toString("");
        
        qDebug() << "[FriendRequestDAO] Friend profile loaded:" << info.username;
        emit friendProfileLoaded(info);
    }, [this](const QString &error) {
        qWarning() << "[FriendRequestDAO] Failed to load friend profile:" << error;
        // 发送空对象表示失败
        emit friendProfileLoaded(FriendInfo());
    });
}
