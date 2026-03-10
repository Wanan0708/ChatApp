// Sources/services/chatservice.cpp
#include "chatservice.h"
#include "../models/conversationmodel.h"
#include "../models/messagemodel.h"
#include "../network/websocketclient.h"
#include "../network/networkclient.h"
#include "../dao/conversationdao.h"
#include "../dao/messagedao.h"
#include "../dao/userdao.h"
#include "../database/databaseconfig.h"
#include "../utils/timeformatter.h"
#include "../utils/messagepreview.h"
#include "../utils/messagecache.h"
#include "../core/securestorage.h"
#include "../core/logger.h"
#include <QDateTime>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QImageReader>
#include <QImageWriter>
#include <QTimer>
#include <QCoreApplication>
#include <QRandomGenerator>
#include <QSettings>
#include <QStandardPaths>

namespace {

QString buildLocalMessageId(const QString &prefix)
{
    return QString("local-%1-%2-%3")
        .arg(prefix)
        .arg(QDateTime::currentMSecsSinceEpoch())
        .arg(QRandomGenerator::global()->generate());
}

void syncSingleConversationProfile(ConversationModel *conversationModel,
                                   const QString &userId,
                                   const QString &userName,
                                   const QString &avatar)
{
    if (!conversationModel || userId.isEmpty()) {
        return;
    }

    const int total = conversationModel->count();
    for (int index = 0; index < total; ++index) {
        const QVariantMap conversation = conversationModel->get(index);
        const QString conversationId = conversation.value("id").toString();
        const QString conversationType = conversation.value("type").toString();
        const bool isDirectConversation = (conversationId == userId)
            || ((conversationType == "single" || conversationType == "user") && conversationId.contains(userId));

        if (!isDirectConversation) {
            continue;
        }

        QVariantMap updates;
        if (!userName.isEmpty()) {
            updates.insert("title", userName);
        }
        updates.insert("avatar", avatar);

        conversationModel->updateConversation(conversationId, updates);
    }
}

}

ChatService* ChatService::instance()
{
    static ChatService instance; // 栈上对象，自动管理生命周期
    return &instance;
}

// helper to ensure network is available (placeholder if needed)
bool ChatService::ensureDatabaseConnected()
{
    return true; // 现在由 NetworkClient 维护连接状态
}

ChatService::ChatService(QObject *parent)
    : QObject(parent)
    , m_conversationModel(new ConversationModel(this))
    , m_webSocketClient(new WebSocketClient(this))
    , m_conversationDAO(new ConversationDAO(this))
    , m_messageDAO(new MessageDAO(this))
    , m_userDAO(new UserDAO(this))
    , m_friendRequestDAO(new FriendRequestDAO(this))
{
    // 加载通用设置
    QSettings settings;
    m_notificationsEnabled = settings.value("settings/notifications", true).toBool();
    m_soundEnabled = settings.value("settings/sound", true).toBool();
    m_autoLoginEnabled = settings.value("settings/autoLogin", false).toBool();
    m_rememberPasswordEnabled = settings.value("settings/rememberPassword", false).toBool();
    m_lastUsername = settings.value("settings/lastUsername", "").toString();
    m_lastPassword = settings.value("settings/lastPassword", "").toString();

    // 移除直接数据库连接
    qDebug() << "[ChatService] Constructor - Network Driven Architecture";

    // 当前用户信息在初始化时为空，等待外部设置
    // m_conversationModel->setCurrentUserId(m_currentUserId) 将在 setCurrentUserId 中调用

    connect(m_userDAO, &UserDAO::loginResult, this, &ChatService::handleLoginResult);
    connect(m_userDAO, &UserDAO::registerResult, this, &ChatService::handleRegisterResult);
    connect(m_userDAO, &UserDAO::searchResult, this, &ChatService::handleSearchUserResult);
    connect(m_userDAO, &UserDAO::userProfileResult, this, &ChatService::handleUserProfileResult);
    connect(m_userDAO, &UserDAO::userProfileUpdated, this, &ChatService::handleUserProfileUpdated);
    connect(m_userDAO, &UserDAO::passwordResetCodeGenerated, this, &ChatService::passwordResetCodeGenerated);
    connect(m_userDAO, &UserDAO::passwordResetResult, this, &ChatService::passwordResetResult);
    connect(m_userDAO, &UserDAO::passwordUpdateResult, this, &ChatService::passwordUpdateResult);
    connect(m_conversationDAO, &ConversationDAO::conversationsLoaded, this, &ChatService::handleConversationsLoaded);
    connect(m_messageDAO, &MessageDAO::messagesLoaded, this, &ChatService::handleMessagesLoaded);
    connect(m_messageDAO, &MessageDAO::messagesMarkedRead, this, &ChatService::handleMessagesMarkedRead);
    connect(m_friendRequestDAO, &FriendRequestDAO::pendingRequestsLoaded, this, &ChatService::handleFriendRequestsLoaded);
    connect(m_friendRequestDAO, &FriendRequestDAO::friendsLoaded, this, &ChatService::handleFriendListLoaded);
    connect(m_friendRequestDAO, &FriendRequestDAO::friendProfileLoaded, this, &ChatService::handleFriendProfileLoaded);
    connect(m_friendRequestDAO, &FriendRequestDAO::requestSent, this, &ChatService::handleFriendRequestSent);
    connect(m_friendRequestDAO, &FriendRequestDAO::requestStatusUpdated, this, &ChatService::handleRequestStatusUpdated);

    // 连接 WebSocket 信号
    connect(m_webSocketClient, &WebSocketClient::connected, this, &ChatService::onWebSocketConnected);
    connect(m_webSocketClient, &WebSocketClient::disconnected, this, &ChatService::onWebSocketDisconnected);
    connect(m_webSocketClient, &WebSocketClient::reconnectFailed, this, &ChatService::onWebSocketReconnectFailed);
    connect(m_webSocketClient, &WebSocketClient::messageReceived, this, &ChatService::onWebSocketMessageReceived);
    connect(m_webSocketClient, &WebSocketClient::connectionStateChanged, this, [this]() {
        emit connectionStateChanged();
    });
    connect(m_webSocketClient, &WebSocketClient::errorOccurred, this, [](const QString &error) {
        qWarning() << "[ChatService] WebSocket error:" << error;
    });

    // 初始时不加载数据，等待登录成功
    // initializeSampleData();

    // 自动连接服务器（统一走配置，支持环境变量覆盖）
    QTimer::singleShot(1000, this, [this]() {
        connectToDefaultChatServer();
    });
}

ChatService::~ChatService()
{
    qDeleteAll(m_messageModels);
    m_messageModels.clear();
    
    // 清理 DAO 对象
    // DAO 已作为子对象自动清理
}

void ChatService::initializeSampleData()
{
    if (m_currentUserId.isEmpty()) return;
    qDebug() << "[ChatService] Loading account data via API...";
    
    // 异步拉取所有会话
    m_conversationDAO->getUserConversations();
}

void ChatService::handleConversationsLoaded(const QVector<Conversation> &conversations)
{
    qDebug() << "[ChatService] Received" << conversations.size() << "conversations";
    
    for (const Conversation &conv : conversations) {
        // 1. 更新对话列表模型
        m_conversationModel->updateConversation(conv.id, {
            {"title", conv.title},
            {"avatar", conv.avatar},
            {"lastMessage", conv.lastMessage},
            {"time", conv.time},
            {"type", conv.type},
            {"unreadCount", conv.unreadCount}
        });

        // 2. 异步拉取每个对话的消息历史
        m_messageDAO->getConversationMessages(conv.id, 50);
    }
}

void ChatService::handleMessagesLoaded(const QString &conversationId, const QVector<Message> &messages, int offset, int limit)
{
    qDebug() << "[ChatService] Loaded" << messages.size() << "messages for" << conversationId << "offset:" << offset;
    MessageModel *msgModel = getMessageModel(conversationId);
    const bool isRefresh = (offset == 0);
    const int previousCount = msgModel->rowCount();

    if (isRefresh) {
        msgModel->clearMessages();
    }

    QList<QVariantMap> cachedMessages;
    for (const Message &msg : messages) {
        QVariantMap msgMap;
        msgMap["messageId"] = msg.messageId;
        msgMap["serverMessageId"] = msg.messageId;
        msgMap["conversationId"] = msg.conversationId;
        msgMap["content"] = msg.content;
        msgMap["senderId"] = msg.senderId;
        msgMap["timestamp"] = msg.timestamp;
        msgMap["type"] = static_cast<int>(msg.type);
        msgMap["status"] = static_cast<int>(msg.status);
        msgMap["fileId"] = msg.fileId;
        msgMap["fileName"] = msg.fileName;
        msgMap["fileSize"] = msg.fileSize;
        msgMap["fileUrl"] = msg.fileUrl;
        msgMap["thumbnailUrl"] = msg.thumbnailUrl;
        msgMap["recalled"] = msg.recalled;
        msgModel->upsertMessage(msgMap);
        cachedMessages.append(msgMap);
    }

    if (!cachedMessages.isEmpty()) {
        MessageCache::instance()->cacheMessages(conversationId, cachedMessages);
    }

    if (conversationId == m_currentConversationId) {
        syncConversationReadState(conversationId);
    }

    m_messageOffsets[conversationId] = offset + messages.size();
    m_hasMoreMessages[conversationId] = (messages.size() >= limit);
    m_loadingMessages[conversationId] = false;
    
    if (isRefresh) {
        emit messagesRefreshed(conversationId);
    } else {
        const int addedCount = qMax(0, msgModel->rowCount() - previousCount);
        emit olderMessagesLoaded(conversationId, addedCount, m_hasMoreMessages.value(conversationId, false));
    }
}

MessageModel* ChatService::getMessageModel(const QString &conversationId)
{
    if (!m_messageModels.contains(conversationId)) {
            MessageModel *model = new MessageModel(conversationId, this);
            model->setCurrentUserId(m_currentUserId);
            m_messageModels[conversationId] = model;

            const QList<QVariantMap> cachedMessages = MessageCache::instance()->getMessages(conversationId, 100);
            for (const QVariantMap &cachedMessage : cachedMessages) {
                model->upsertMessage(cachedMessage);
            }
        }
    return m_messageModels.value(conversationId);
}

void ChatService::setCurrentConversationId(const QString &id)
{
    if (m_currentConversationId != id) {
        m_currentConversationId = id;
        m_conversationModel->setCurrentConversation(id);
        syncConversationReadState(id);
        emit currentConversationIdChanged();
    }
}

void ChatService::setCurrentConversation(const QString &conversationId)
{
    setCurrentConversationId(conversationId);
}

void ChatService::refreshConversations()
{
    qDebug() << "[ChatService] Refreshing conversations...";
    m_conversationDAO->getUserConversations();
}

int ChatService::totalUnreadCount() const
{
    int total = 0;
    for (int i = 0; i < m_conversationModel->rowCount(); ++i) {
        total += m_conversationModel->data(m_conversationModel->index(i, 0), ConversationModel::UnreadCountRole).toInt();
    }
    return total;
}

bool ChatService::hasUnreadMessages() const
{
    return totalUnreadCount() > 0;
}

void ChatService::sendMessage(const QString &conversationId, const QString &content)
{
    sendTextMessageInternal(conversationId, content);
}

void ChatService::sendTextMessageInternal(const QString &conversationId,
                                         const QString &content,
                                         const QString &messageId,
                                         qint64 timestamp,
                                         bool updateExisting)
{
    if (content.trimmed().isEmpty()) return;

    const QString trimmedContent = content.trimmed();
    const QString localMessageId = messageId.isEmpty() ? buildLocalMessageId("text") : messageId;
    const qint64 currentTimestamp = timestamp > 0 ? timestamp : QDateTime::currentMSecsSinceEpoch();
    const bool queuedBeforeDispatch = !m_webSocketClient->isConnected();

    QVariantMap message;
    message["messageId"] = localMessageId;
    message["type"] = 0;
    message["conversationId"] = conversationId;
    message["content"] = trimmedContent;
    message["senderId"] = m_currentUserId;
    message["timestamp"] = currentTimestamp;
    message["status"] = static_cast<int>(MessageStatus::Sending);
    message["isOffline"] = queuedBeforeDispatch;

    getMessageModel(conversationId)->upsertMessage(message);
    if (!updateExisting) {
        qDebug() << "[ChatService] Message added to local model optimistically";
    }

    const QString displayContent = MessagePreview::normalizeConversationPreview(trimmedContent, 0);
    QString displayTime = TimeFormatter::formatChatTime(message["timestamp"].toLongLong());

    m_conversationModel->updateConversation(conversationId, {
        {"lastMessage", displayContent},
        {"time", displayTime},
        {"unreadCount", 0}
    });

    QVariantMap wsMessage;
    wsMessage["type"] = "message";
    wsMessage["messageType"] = 0;
    wsMessage["conversationId"] = conversationId;
    wsMessage["content"] = trimmedContent;
    wsMessage["senderId"] = m_currentUserId;
    wsMessage["timestamp"] = currentTimestamp;
    wsMessage["status"] = 1;
    wsMessage["clientMessageId"] = localMessageId;

    QVariantMap retryInfo;
    retryInfo["conversationId"] = conversationId;
    retryInfo["messageType"] = 0;
    retryInfo["content"] = trimmedContent;
    retryInfo["timestamp"] = currentTimestamp;
    rememberRetryableMessage(localMessageId, retryInfo);

    dispatchOutgoingMessage(conversationId, localMessageId, wsMessage, queuedBeforeDispatch);

    emit messageReceived(conversationId, message);
}

void ChatService::markConversationRead(const QString &conversationId)
{
    syncConversationReadState(conversationId);
}

void ChatService::syncConversationReadState(const QString &conversationId, bool notifyServer)
{
    if (conversationId.isEmpty()) {
        return;
    }

    m_conversationModel->markRead(conversationId);

    if (notifyServer) {
        m_messageDAO->markMessagesAsRead(conversationId);
    }
}

void ChatService::handleMessagesMarkedRead(const QString &conversationId, bool success)
{
    if (!success) {
        qWarning() << "[ChatService] Failed to sync read state for conversation:" << conversationId;
        return;
    }

    if (!conversationId.isEmpty()) {
        m_conversationModel->markRead(conversationId);
    }
}

void ChatService::refreshMessages(const QString &conversationId)
{
    qDebug() << "[ChatService] Refreshing messages for:" << conversationId;
    m_loadingMessages[conversationId] = true;
    m_messageOffsets[conversationId] = 0;
    m_hasMoreMessages[conversationId] = true;
    m_messageDAO->getConversationMessages(conversationId, 50);
}

bool ChatService::loadOlderMessages(const QString &conversationId)
{
    if (conversationId.isEmpty()) {
        return false;
    }

    if (m_loadingMessages.value(conversationId, false) || !m_hasMoreMessages.value(conversationId, true)) {
        return false;
    }

    const int offset = m_messageOffsets.value(conversationId, 0);
    m_loadingMessages[conversationId] = true;
    m_messageDAO->getConversationMessages(conversationId, 50, offset);
    return true;
}

bool ChatService::hasMoreMessages(const QString &conversationId) const
{
    return m_hasMoreMessages.value(conversationId, true);
}

bool ChatService::isLoadingMessages(const QString &conversationId) const
{
    return m_loadingMessages.value(conversationId, false);
}

void ChatService::searchUsers(const QString &searchTerm)
{
    if (searchTerm.trimmed().isEmpty()) return;
    m_userDAO->searchUsers(searchTerm);
}

void ChatService::loadUserProfile()
{
    if (m_currentUserId.isEmpty()) {
        return;
    }
    m_userDAO->getMyProfile();
}

void ChatService::saveUserProfile(const QVariantMap &profile)
{
    if (m_currentUserId.isEmpty()) {
        emit userProfileSaveResult(false, "未登录，无法保存资料");
        return;
    }

    QVariantMap normalizedProfile = profile;
    const QString avatar = normalizedProfile.value("avatar").toString();
    if (!avatar.isEmpty()) {
        normalizedProfile["avatar"] = DatabaseConfig::toServerRelativePath(avatar);
    }

    m_userDAO->updateMyProfile(normalizedProfile);
}

void ChatService::uploadAvatar(const QString &filePath)
{
    if (m_currentUserId.isEmpty()) {
        emit avatarUploadResult(false, "", "未登录，无法上传头像");
        return;
    }

    if (filePath.trimmed().isEmpty()) {
        emit avatarUploadResult(false, "", "请选择头像图片");
        return;
    }

    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        emit avatarUploadResult(false, "", "头像文件不存在");
        return;
    }

    QImageReader reader(filePath);
    reader.setAutoTransform(true);
    if (!reader.canRead()) {
        emit avatarUploadResult(false, "", "请选择有效的图片文件");
        return;
    }

    QImage image = reader.read();
    if (image.isNull()) {
        emit avatarUploadResult(false, "", "头像读取失败");
        return;
    }

    const int cropSize = qMin(image.width(), image.height());
    if (cropSize > 0 && (image.width() != image.height())) {
        const QRect cropRect(
            (image.width() - cropSize) / 2,
            (image.height() - cropSize) / 2,
            cropSize,
            cropSize
        );
        image = image.copy(cropRect);
    }

    const QSize maxAvatarSize(512, 512);
    if (image.width() > maxAvatarSize.width() || image.height() > maxAvatarSize.height()) {
        image = image.scaled(maxAvatarSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    const QString tempDirPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    QDir tempDir(tempDirPath);
    if (!tempDir.exists()) {
        tempDir.mkpath(".");
    }

    const bool keepAlpha = image.hasAlphaChannel();
    const QString suffix = keepAlpha ? "png" : "jpg";
    const QString optimizedAvatarPath = tempDir.filePath(
        QString("chatapp-avatar-%1-%2.%3")
            .arg(m_currentUserId)
            .arg(QDateTime::currentMSecsSinceEpoch())
            .arg(suffix)
    );

    QImageWriter writer(optimizedAvatarPath, suffix.toUpper().toLatin1());
    writer.setOptimizedWrite(true);
    if (!keepAlpha) {
        writer.setQuality(85);
    }

    if (!writer.write(image)) {
        emit avatarUploadResult(false, "", "头像压缩失败");
        return;
    }

    emit avatarUploadProgressChanged(0);

    QNetworkReply *reply = NetworkClient::instance()->uploadFile(
        "/users/profile/avatar",
        optimizedAvatarPath,
        "avatar",
        QJsonObject(),
        [this](const QJsonObject &, qint64 bytesSent, qint64 bytesTotal) {
            const int progress = bytesTotal > 0
                ? static_cast<int>((bytesSent * 100) / bytesTotal)
                : 0;
            emit avatarUploadProgressChanged(progress);
        },
        [this](const QJsonObject &response) {
            emit avatarUploadProgressChanged(100);
            const QString avatarUrl = response.value("avatarUrl").toString();
            const QString avatarPath = response.value("avatarPath").toString();
            const QString message = response.value("message").toString();
            const QString resolvedAvatarUrl = DatabaseConfig::resolveServerUrl(
                avatarUrl.isEmpty() ? avatarPath : avatarUrl
            );

            if (resolvedAvatarUrl.isEmpty()) {
                emit avatarUploadResult(false, "", "头像上传失败");
                return;
            }

            emit avatarUploadResult(true, resolvedAvatarUrl, message.isEmpty() ? "头像上传成功" : message);
        },
        [this](const QString &error) {
            emit avatarUploadResult(false, "", error.isEmpty() ? "头像上传失败" : error);
        }
    );

    if (!reply) {
        QFile::remove(optimizedAvatarPath);
        return;
    }

    connect(reply, &QNetworkReply::finished, this, [optimizedAvatarPath]() {
        QFile::remove(optimizedAvatarPath);
    });
}

// 拉取离线消息
void ChatService::fetchOfflineMessages()
{
    if (m_currentUserId.isEmpty()) {
        qDebug() << "[ChatService] Skip fetching offline messages - user not logged in";
        return;
    }

    if (!m_webSocketClient->isConnected()) {
        qDebug() << "[ChatService] Skip fetching offline messages - WebSocket not connected";
        return;
    }

    qDebug() << "[ChatService] Fetching offline messages...";

    // 通过 WebSocket 发送拉取离线消息请求
    QVariantMap fetchRequest;
    fetchRequest["type"] = "fetch_offline";
    fetchRequest["userId"] = m_currentUserId;
    m_webSocketClient->sendMessage(fetchRequest);
}

// 拉取好友更新（上线后）
void ChatService::fetchFriendsUpdates()
{
    if (m_currentUserId.isEmpty()) {
        return;
    }

    if (!m_webSocketClient->isConnected()) {
        return;
    }

    // 防止重复认证
    if (m_wsAuthenticated) {
        qDebug() << "[ChatService] Already authenticated, skipping friends updates fetch";
        return;
    }

    // 构建认证消息，包含最后同步时间和好友版本
    QVariantMap authRequest;
    authRequest["type"] = "auth";
    
    // 获取 Token
    QString token = NetworkClient::instance()->token();
    if (!token.isEmpty()) {
        authRequest["token"] = token;
    }
    
    // 添加最后同步时间
    if (m_lastSyncTime.isValid()) {
        authRequest["lastSyncTime"] = m_lastSyncTime.toString(Qt::ISODate);
    }
    
    // 添加好友版本信息
    if (!m_friendVersions.isEmpty()) {
        QJsonObject versionsObj;
        for (auto it = m_friendVersions.begin(); it != m_friendVersions.end(); ++it) {
            versionsObj[it.key()] = it.value();
        }
        authRequest["friendVersions"] = QJsonDocument(versionsObj).toJson(QJsonDocument::Compact);
    }
    
    qDebug() << "[ChatService] Fetching friends updates, lastSyncTime:" << m_lastSyncTime
             << "friends count:" << m_friendVersions.size();
    
    // 发送认证请求（服务器会返回好友更新）
    m_webSocketClient->sendMessage(authRequest);
}

// 撤回消息（2 分钟内）
void ChatService::recallMessage(const QString &conversationId, const QString &messageId)
{
    if (conversationId.isEmpty() || messageId.isEmpty()) {
        Logger::instance()->warning("Recall message failed - empty conversationId or messageId", "chatservice");
        return;
    }
    
    MessageModel *model = getMessageModel(conversationId);
    QString resolvedMessageId = messageId;
    if (model) {
        const QList<QVariantMap> messages = model->messages();
        for (const QVariantMap &message : messages) {
            if (message.value("messageId").toString() == messageId
                || message.value("serverMessageId").toString() == messageId) {
                const QString serverMessageId = message.value("serverMessageId").toString();
                if (!serverMessageId.isEmpty()) {
                    resolvedMessageId = serverMessageId;
                }
                break;
            }
        }
    }

    if (resolvedMessageId.startsWith(QStringLiteral("local-"))) {
        Logger::instance()->warning("Recall message failed - message has not been acknowledged by server yet", "chatservice");
        return;
    }

    Logger::instance()->info(QString("Recalling message %1 in conversation %2").arg(resolvedMessageId).arg(conversationId), "chatservice");
    
    // 通过 WebSocket 发送撤回请求
    QVariantMap recallRequest;
    recallRequest["type"] = "recall_message";
    recallRequest["conversationId"] = conversationId;
    recallRequest["messageId"] = resolvedMessageId;
    recallRequest["timestamp"] = QDateTime::currentMSecsSinceEpoch();
    
    m_webSocketClient->sendMessage(recallRequest);
    
    // 乐观更新：立即在本地标记为已撤回
    if (model) {
        QVariantMap updates;
        updates["messageId"] = resolvedMessageId;
        updates["serverMessageId"] = resolvedMessageId;
        updates["conversationId"] = conversationId;
        updates["recalled"] = true;
        model->upsertMessage(updates);
        MessageCache::instance()->cacheMessage(conversationId, updates);
    }
}

void ChatService::handleSearchUserResult(const QVector<UserDAO::UserSearchResult> &results)
{
    QVariantList list;
    for (const auto &match : results) {
        QVariantMap map;
        map["userId"] = match.userId;
        map["username"] = match.username;
        list.append(map);
    }
    qDebug() << "user list: " << list;
    emit searchResult(list);
}

void ChatService::handleUserProfileResult(bool success, const QVariantMap &profile, const QString &error)
{
    if (!success) {
        qWarning() << "[ChatService] Failed to load user profile:" << error;
        return;
    }

    qDebug() << "[ChatService] User profile loaded:" << profile;

    setCurrentUserName(profile.value("username").toString());
    setCurrentUserAvatar(profile.value("avatar").toString());
    setCurrentUserGender(profile.value("gender").toString());
    setCurrentUserRegion(profile.value("region").toString());
    setCurrentUserPhone(profile.value("phone").toString());
    setCurrentUserEmail(profile.value("email").toString());
    setCurrentUserContact(profile.value("contact").toString());
    setCurrentUserBio(profile.value("bio").toString());
    setCurrentUserAge(profile.value("age").toInt());

    qDebug() << "[ChatService] User info updated - Name:" << m_currentUserName
             << "Gender:" << m_currentUserGender
             << "Region:" << m_currentUserRegion
             << "Age:" << m_currentUserAge;
}

void ChatService::handleUserProfileUpdated(bool success, const QString &message)
{
    if (success) {
        // 保存成功后，重新加载用户资料以获取最新数据
        qDebug() << "[ChatService] Profile saved successfully, reloading...";
        loadUserProfile();
        emit userProfileSaveResult(true, message.isEmpty() ? "资料保存成功" : message);
    } else {
        qWarning() << "[ChatService] Profile save failed:" << message;
        emit userProfileSaveResult(false, message.isEmpty() ? "资料保存失败" : message);
    }
}

void ChatService::logout()
{
    qDebug() << "[ChatService] Logging out...";

    // 先调用服务器登出 API，更新用户状态为离线
    if (!m_currentUserId.isEmpty() && !NetworkClient::instance()->token().isEmpty()) {
        NetworkClient::instance()->post("/auth/logout", QJsonObject{}, [](const QJsonObject &res) {
            qDebug() << "[ChatService] Logout API response:" << QJsonDocument(res).toJson();
        }, [](const QString &error) {
            qWarning() << "[ChatService] Logout API error:" << error;
        });
    }

    // 断开聊天服务器连接
    disconnectFromChatServer();

    // 清除当前用户信息
    m_currentUserId = "";
    m_currentUserName = "";
    m_currentUserAvatar = "";
    m_currentUserGender = "";
    m_currentUserRegion = "";
    m_currentUserPhone = "";
    m_currentUserEmail = "";
    m_currentUserContact = "";
    m_currentUserBio = "";

    // 清除 NetworkClient 中的 Token
    NetworkClient::instance()->setToken("");
    // 清除安全存储的 Token
    clearToken();

    emit currentUserIdChanged();
    emit currentUserNameChanged();
    emit currentUserAvatarChanged();
    emit currentUserGenderChanged();
    emit currentUserRegionChanged();
    emit currentUserPhoneChanged();
    emit currentUserEmailChanged();
    emit currentUserContactChanged();
    emit currentUserBioChanged();

    // 清理消息模型缓存
    qDeleteAll(m_messageModels);
    m_messageModels.clear();
    m_pendingOutgoingMessages.clear();
    m_retryableMessages.clear();

    // 通知对话模型更新
    if (m_conversationModel) {
        m_conversationModel->setCurrentUserId("");
    }

    qDebug() << "[ChatService] Logout completed";
}

void ChatService::changePassword(const QString &newPassword)
{
    if (m_currentUserId.isEmpty()) return;
    adminSetPassword(m_currentUserId, newPassword);
}

void ChatService::connectToDefaultChatServer()
{
    connectToChatServer(DatabaseConfig::wsUrl());
}

void ChatService::connectToChatServer(const QString &serverUrl)
{
    const QString targetUrl = serverUrl.trimmed().isEmpty()
        ? DatabaseConfig::wsUrl()
        : serverUrl.trimmed();
    qDebug() << "[ChatService] Connecting WebSocket to:" << targetUrl;
    m_webSocketClient->connectToServer(targetUrl);
}

void ChatService::disconnectFromChatServer()
{
    m_webSocketClient->disconnectFromServer();
}

void ChatService::setCurrentUserId(const QString &userId)
{
    if (m_currentUserId != userId) {
        m_currentUserId = userId;
        emit currentUserIdChanged();
        
        // 同时更新模型中的当前用户ID
        if (m_conversationModel) {
            m_conversationModel->setCurrentUserId(userId);
        }
        
        // 用户ID变更时重新加载数据
        initializeSampleData();
    }
}

void ChatService::setCurrentUserName(const QString &userName)
{
    if (m_currentUserName != userName) {
        m_currentUserName = userName;
        emit currentUserNameChanged();
    }
}

void ChatService::setCurrentUserAvatar(const QString &avatar)
{
    const QString resolvedAvatar = DatabaseConfig::resolveServerUrl(avatar);
    if (m_currentUserAvatar != resolvedAvatar) {
        m_currentUserAvatar = resolvedAvatar;
        emit currentUserAvatarChanged();
    }
}

void ChatService::setCurrentUserGender(const QString &gender)
{
    if (m_currentUserGender != gender) {
        m_currentUserGender = gender;
        emit currentUserGenderChanged();
    }
}

void ChatService::setCurrentUserRegion(const QString &region)
{
    if (m_currentUserRegion != region) {
        m_currentUserRegion = region;
        emit currentUserRegionChanged();
    }
}

void ChatService::setCurrentUserPhone(const QString &phone)
{
    if (m_currentUserPhone != phone) {
        m_currentUserPhone = phone;
        emit currentUserPhoneChanged();
    }
}

void ChatService::setCurrentUserEmail(const QString &email)
{
    if (m_currentUserEmail != email) {
        m_currentUserEmail = email;
        emit currentUserEmailChanged();
    }
}

void ChatService::setCurrentUserContact(const QString &contact)
{
    if (m_currentUserContact != contact) {
        m_currentUserContact = contact;
        emit currentUserContactChanged();
    }
}

void ChatService::setCurrentUserBio(const QString &bio)
{
    if (m_currentUserBio != bio) {
        m_currentUserBio = bio;
        emit currentUserBioChanged();
    }
}

void ChatService::setCurrentUserAge(int age)
{
    if (m_currentUserAge != age) {
        m_currentUserAge = age;
        emit currentUserAgeChanged();
    }
}

bool ChatService::isConnected() const
{
    return m_webSocketClient->isConnected();
}

QString ChatService::connectionState() const
{
    return m_webSocketClient->connectionState();
}

bool ChatService::isReconnecting() const
{
    return m_webSocketClient->isReconnecting();
}

void ChatService::onWebSocketMessageReceived(const QVariantMap &rawMessage)
{
    QString msgType = rawMessage.value("type").toString();

    if (msgType == "message") {
        processIncomingMessage(rawMessage);
    } else if (msgType == "message_ack") {
        const QString clientMessageId = rawMessage.value("clientMessageId").toString();
        const QString conversationId = rawMessage.value("conversationId").toString();
        const QString originalConversationId = m_pendingOutgoingMessages.value(clientMessageId);
        const QString serverMessageId = rawMessage.value("messageId").toString();
        const bool success = rawMessage.value("success", true).toBool();
        const QString errorText = rawMessage.value("error").toString();

        if (clientMessageId.isEmpty()) {
            qWarning() << "[ChatService] Received message_ack without clientMessageId";
            return;
        }

        const QString resolvedConversationId = !originalConversationId.isEmpty()
            ? originalConversationId
            : conversationId;

        QVariantMap updates;
        updates["messageId"] = clientMessageId;
        updates["conversationId"] = resolvedConversationId;
        updates["serverMessageId"] = serverMessageId;

        if (success) {
            if (!conversationId.isEmpty() && !originalConversationId.isEmpty() && conversationId != originalConversationId) {
                migrateConversationContext(originalConversationId, conversationId);
            }

            updates["status"] = static_cast<int>(MessageStatus::Sent);
            updates["isOffline"] = false;
            updates["errorText"] = QString();
            getMessageModel(resolvedConversationId)->upsertMessage(updates);
            MessageCache::instance()->cacheMessage(resolvedConversationId, updates);
            m_pendingOutgoingMessages.remove(clientMessageId);
            clearRetryableMessage(clientMessageId);
            qDebug() << "[ChatService] Message acknowledged by server:" << clientMessageId << serverMessageId;
        } else {
            updates["status"] = static_cast<int>(MessageStatus::Failed);
            updates["isOffline"] = true;
            updates["errorText"] = errorText.isEmpty() ? QStringLiteral("消息发送失败，点击重试") : errorText;
            getMessageModel(resolvedConversationId)->upsertMessage(updates);
            MessageCache::instance()->cacheMessage(resolvedConversationId, updates);
            m_pendingOutgoingMessages.remove(clientMessageId);
            qWarning() << "[ChatService] Message rejected by server:" << clientMessageId << errorText;
        }
    } else if (msgType == "message_recalled") {
        const QString conversationId = rawMessage.value("conversationId").toString();
        const QString messageId = rawMessage.value("messageId").toString();
        const bool success = rawMessage.value("success", true).toBool();
        const QString errorText = rawMessage.value("error").toString();

        if (conversationId.isEmpty() || messageId.isEmpty()) {
            qWarning() << "[ChatService] Received invalid message_recalled payload";
            return;
        }

        if (!success) {
            qWarning() << "[ChatService] Message recall failed:" << messageId << errorText;
            return;
        }

        QVariantMap updates;
        updates["messageId"] = messageId;
        updates["serverMessageId"] = messageId;
        updates["conversationId"] = conversationId;
        updates["recalled"] = true;
        getMessageModel(conversationId)->upsertMessage(updates);
        MessageCache::instance()->cacheMessage(conversationId, updates);
    } else if (msgType == "system") {
        QString subtype = rawMessage.value("subtype").toString();
        if (subtype == "conversation_created") {
            QString convId = rawMessage.value("conversationId").toString();
            QString title = rawMessage.value("title").toString();
            QString conversationType = rawMessage.value("conversationType",
                                                        rawMessage.value("chatType", "user")).toString();
            QString avatar = DatabaseConfig::resolveServerUrl(rawMessage.value("avatar").toString());

            m_conversationModel->updateConversation(convId, {
                {"title", title},
                {"type", conversationType},
                {"avatar", avatar}
            });
            qDebug() << "[ChatService] Created new conversation:" << convId << title;
        }
    } else if (msgType == "friend_request") {
        // 收到好友申请
        emit friendRequestReceived(rawMessage);
        qDebug() << "[ChatService] Received friend request from:" << rawMessage.value("fromName").toString();
    } else if (msgType == "friend_response") {
        // 对方同意了好友申请
        if (rawMessage.value("accepted").toBool()) {
            QString friendId = rawMessage.value("fromId").toString();
            QString friendName = rawMessage.value("fromName").toString();
            emit friendshipEstablished(friendId, friendName);

            // 刷新数据（会话列表等）
            initializeSampleData();
        }
    } else if (msgType == "profile_updated") {
        // 收到好友信息更新通知
        QString userId = rawMessage.value("userId").toString();
        qDebug() << "[ChatService] Received profile update from:" << userId;
        qDebug() << "[ChatService] Profile update data:" << rawMessage;

        // 刷新该好友的信息（通知 QML 更新）
        QVariantMap updatedProfile;
        updatedProfile["userId"] = userId;
        updatedProfile["username"] = rawMessage.value("username").toString();
        updatedProfile["avatar"] = DatabaseConfig::resolveServerUrl(rawMessage.value("avatar").toString());
        updatedProfile["gender"] = rawMessage.value("gender").toString();
        updatedProfile["region"] = rawMessage.value("region").toString();
        updatedProfile["signature"] = rawMessage.value("signature").toString();
        updatedProfile["age"] = rawMessage.value("age").toInt();
        updatedProfile["version"] = rawMessage.value("version").toInt();

        qDebug() << "[ChatService] Emitting friendProfileUpdated with:" << updatedProfile;

        // 更新本地版本缓存
        int version = updatedProfile["version"].toInt();
        if (version > 0) {
            m_friendVersions[userId] = version;
        }

        syncSingleConversationProfile(m_conversationModel,
                                      userId,
                                      updatedProfile.value("username").toString(),
                                      updatedProfile.value("avatar").toString());

        // 发出信号通知 QML 更新
        emit friendProfileUpdated(updatedProfile);
    } else if (msgType == "presence_update") {
        QString userId = rawMessage.value("userId").toString();
        if (userId.isEmpty()) {
            return;
        }

        QVariantMap presenceInfo;
        presenceInfo["userId"] = userId;
        const QString status = rawMessage.value("status").toString();
        presenceInfo["status"] = status.isEmpty() ? QString("offline") : status;

        qDebug() << "[ChatService] Received presence update:" << presenceInfo;
        emit friendPresenceUpdated(presenceInfo);
    } else if (msgType == "friends_updates") {
        // 收到好友批量更新（上线后拉取）
        QVariantList updates = rawMessage.value("updates").toList();
        qDebug() << "[ChatService] Received" << updates.size() << "friend updates";
        
        for (const QVariant &update : updates) {
            QVariantMap updateMap = update.toMap();
            QString userId = updateMap["user_id"].toString();
            
            // 更新本地版本缓存
            int version = updateMap["version"].toInt();
            if (version > 0) {
                m_friendVersions[userId] = version;
            }
            
            // 转换字段名并发送更新
            QVariantMap friendInfo;
            friendInfo["userId"] = userId;
            friendInfo["username"] = updateMap["username"].toString();
            friendInfo["avatar"] = DatabaseConfig::resolveServerUrl(updateMap["avatar"].toString());
            friendInfo["gender"] = updateMap["gender"].toString();
            friendInfo["region"] = updateMap["region"].toString();
            friendInfo["signature"] = updateMap["signature"].toString();
            friendInfo["age"] = updateMap["age"].toInt();
            friendInfo["status"] = updateMap["status"].toString();
            if (friendInfo["status"].toString().isEmpty()) {
                friendInfo["status"] = "offline";
            }
            friendInfo["version"] = version;

            syncSingleConversationProfile(m_conversationModel,
                                          userId,
                                          friendInfo.value("username").toString(),
                                          friendInfo.value("avatar").toString());

            QVariantMap presenceInfo;
            presenceInfo["userId"] = userId;
            presenceInfo["status"] = friendInfo.value("status").toString();

            emit friendProfileUpdated(friendInfo);
            emit friendPresenceUpdated(presenceInfo);
        }
    } else if (msgType == "auth_success") {
        // WebSocket 认证成功
        qDebug() << "[ChatService] WebSocket auth successful";
        m_wsAuthenticated = true;
        // 更新同步时间
        m_lastSyncTime = QDateTime::currentDateTime();
    } else if (msgType == "heartbeat") {
        // 忽略
    } else {
        qDebug() << "[ChatService] Unknown message type:" << msgType;
    }
}

void ChatService::processIncomingMessage(const QVariantMap &message)
{
    QVariantMap normalizedMessage = message;
    if (normalizedMessage.value("serverMessageId").toString().isEmpty()) {
        normalizedMessage["serverMessageId"] = normalizedMessage.value("messageId").toString();
    }

    QString conversationId = normalizedMessage.value("conversationId").toString();
    QString senderId = normalizedMessage.value("senderId").toString();
    QString content = normalizedMessage.value("content").toString();
    qint64 timestamp = normalizedMessage.value("timestamp").toLongLong();

    if (conversationId.isEmpty() || senderId.isEmpty() || content.isEmpty()) {
        qWarning() << "[ChatService] Invalid message format";
        return;
    }

    // ✅ 过滤：跳过自己已发送的消息（已在 sendMessage 中本地添加）
    // 避免重复显示
    if (senderId == m_currentUserId) {
        qDebug() << "[ChatService] Skipping own message (already added locally)";
        return;
    }

    // 1. 添加到消息模型（来自其他用户的消息）
    getMessageModel(conversationId)->addMessage(normalizedMessage);
    MessageCache::instance()->cacheMessage(conversationId, normalizedMessage);

    // 2. 更新会话元数据
    bool isCurrent = (conversationId == m_currentConversationId);

    const int messageType = normalizedMessage.value("type", normalizedMessage.value("messageType", 0)).toInt();
    const QString displayContent = MessagePreview::normalizeConversationPreview(content, messageType);
    // 使用 TimeFormatter 统一时间格式
    QString displayTime = TimeFormatter::formatChatTime(timestamp);

    if (isCurrent) {
        syncConversationReadState(conversationId);
        // ✅ 如果是当前会话，消息已自动读取，未读数复位为 0
        m_conversationModel->updateConversation(conversationId, {
            {"lastMessage", displayContent},
            {"time", displayTime},
            {"unreadCount", 0}
        });
    } else {
        // ✅ 非当前会话：获取当前未读数，然后 +1
        int currentUnread = m_conversationModel->getUnreadCount(conversationId);
        m_conversationModel->updateConversation(conversationId, {
            {"lastMessage", displayContent},
            {"time", displayTime},
            {"unreadCount", currentUnread + 1}
        });
    }

    // 3. 发出信号
    emit messageReceived(conversationId, normalizedMessage);
    emit conversationUpdated(conversationId);

    qDebug() << "[ChatService] Processed incoming message from" << senderId
             << "to conversation" << conversationId;
}

void ChatService::onWebSocketConnected()
{
    qDebug() << "[ChatService] WebSocket connected";
    emit connectedChanged(true);
    emit connectionStateChanged();

    for (auto it = m_pendingOutgoingMessages.constBegin(); it != m_pendingOutgoingMessages.constEnd(); ++it) {
        updateLocalMessageState(it.value(), it.key(), MessageStatus::Sending, false, QString(), true);
    }

    // 重置认证状态
    m_wsAuthenticated = false;

    // 发送认证消息 - 使用 JWT Token 和好友版本信息
    if (!m_currentUserId.isEmpty() && !NetworkClient::instance()->token().isEmpty()) {
        fetchFriendsUpdates();
        qDebug() << "[ChatService] Sent WebSocket auth for user:" << m_currentUserId;

        // 认证成功后拉取离线消息（延迟 500ms 确保认证完成）
        QTimer::singleShot(500, this, [this]() {
            fetchOfflineMessages();
        });
    } else {
        qDebug() << "[ChatService] Skipping WebSocket auth - no userId or token";
    }
}

void ChatService::onWebSocketDisconnected()
{
    qDebug() << "[ChatService] WebSocket disconnected";
    // 重置认证状态
    m_wsAuthenticated = false;

    for (auto it = m_pendingOutgoingMessages.constBegin(); it != m_pendingOutgoingMessages.constEnd(); ++it) {
        updateLocalMessageState(it.value(), it.key(), MessageStatus::Sending, true,
                                QStringLiteral("网络已断开，正在自动重连；点击可立即重试"), true);
    }

    emit connectedChanged(false);
    emit connectionStateChanged();
}

void ChatService::onWebSocketReconnectFailed()
{
    qWarning() << "[ChatService] WebSocket reconnect failed, marking pending messages as failed";

    for (auto it = m_pendingOutgoingMessages.constBegin(); it != m_pendingOutgoingMessages.constEnd(); ++it) {
        updateLocalMessageState(it.value(), it.key(), MessageStatus::Failed, true,
                                QStringLiteral("网络重连失败，点击重试"), true);
    }

    m_pendingOutgoingMessages.clear();
    emit connectionStateChanged();
}

void ChatService::dispatchOutgoingMessage(const QString &conversationId,
                                         const QString &localMessageId,
                                         QVariantMap messagePayload,
                                         bool queuedBeforeDispatch)
{
    if (localMessageId.isEmpty()) {
        m_webSocketClient->sendMessage(messagePayload);
        return;
    }

    messagePayload["clientMessageId"] = localMessageId;
    m_webSocketClient->sendMessage(messagePayload);

    m_pendingOutgoingMessages.insert(localMessageId, conversationId);

    if (queuedBeforeDispatch) {
        updateLocalMessageState(conversationId, localMessageId, MessageStatus::Sending, true, QString(), true);
        qDebug() << "[ChatService] Message queued for resend after reconnect:" << localMessageId;
        return;
    }

    updateLocalMessageState(conversationId, localMessageId, MessageStatus::Sending, false, QString(), true);
    qDebug() << "[ChatService] Message dispatched via WebSocket, awaiting ack:" << localMessageId;
}

void ChatService::updateLocalMessageState(const QString &conversationId,
                                         const QString &messageId,
                                         MessageStatus status,
                                         bool isOffline,
                                         const QString &errorText,
                                         bool overwriteErrorText)
{
    if (conversationId.isEmpty() || messageId.isEmpty()) {
        return;
    }

    QVariantMap updates;
    updates["messageId"] = messageId;
    updates["conversationId"] = conversationId;
    updates["status"] = static_cast<int>(status);
    updates["isOffline"] = isOffline;
    if (overwriteErrorText) {
        updates["errorText"] = errorText;
    }

    getMessageModel(conversationId)->upsertMessage(updates);
    MessageCache::instance()->cacheMessage(conversationId, updates);
}

void ChatService::rememberRetryableMessage(const QString &messageId, const QVariantMap &retryInfo)
{
    if (messageId.isEmpty()) {
        return;
    }

    m_retryableMessages.insert(messageId, retryInfo);
}

void ChatService::clearRetryableMessage(const QString &messageId)
{
    if (messageId.isEmpty()) {
        return;
    }

    m_retryableMessages.remove(messageId);
}

void ChatService::retryMessage(const QString &conversationId, const QString &messageId)
{
    const QVariantMap retryInfo = m_retryableMessages.value(messageId);
    if (retryInfo.isEmpty()) {
        qWarning() << "[ChatService] Retry requested for unknown message:" << messageId;
        return;
    }

    const int messageType = retryInfo.value("messageType").toInt();
    const qint64 timestamp = retryInfo.value("timestamp").toLongLong();
    const bool hasUploadedPayload = !retryInfo.value("fileId").toString().isEmpty()
        && !retryInfo.value("fileUrl").toString().isEmpty();

    updateLocalMessageState(conversationId, messageId, MessageStatus::Sending,
                            !m_webSocketClient->isConnected(), QString(), true);

    if (!m_webSocketClient->isConnected()) {
        connectToDefaultChatServer();
    }

    if (messageType == 1) {
        if (hasUploadedPayload) {
            resendUploadedAttachmentMessage(conversationId, messageId, retryInfo, timestamp);
            return;
        }

        sendImageMessageInternal(conversationId,
                                 retryInfo.value("localFilePath").toString(),
                                 messageId,
                                 timestamp,
                                 true);
        return;
    }

    if (messageType == 2) {
        if (hasUploadedPayload) {
            resendUploadedAttachmentMessage(conversationId, messageId, retryInfo, timestamp);
            return;
        }

        sendFileMessageInternal(conversationId,
                                retryInfo.value("localFilePath").toString(),
                                messageId,
                                timestamp,
                                true);
        return;
    }

    sendTextMessageInternal(conversationId,
                            retryInfo.value("content").toString(),
                            messageId,
                            timestamp,
                            true);
}

void ChatService::resendUploadedAttachmentMessage(const QString &conversationId,
                                                  const QString &messageId,
                                                  const QVariantMap &retryInfo,
                                                  qint64 timestamp)
{
    const int messageType = retryInfo.value("messageType").toInt();
    const bool queuedBeforeDispatch = !m_webSocketClient->isConnected();
    const QString content = retryInfo.value("content").toString();

    QVariantMap localMessage;
    localMessage["messageId"] = messageId;
    localMessage["conversationId"] = conversationId;
    localMessage["type"] = messageType;
    localMessage["content"] = content;
    localMessage["senderId"] = m_currentUserId;
    localMessage["timestamp"] = timestamp;
    localMessage["status"] = static_cast<int>(MessageStatus::Sending);
    localMessage["isOffline"] = queuedBeforeDispatch;
    localMessage["errorText"] = QString();
    localMessage["fileId"] = retryInfo.value("fileId").toString();
    localMessage["fileName"] = retryInfo.value("fileName").toString();
    localMessage["fileUrl"] = retryInfo.value("fileUrl").toString();
    localMessage["thumbnailUrl"] = retryInfo.value("thumbnailUrl").toString();
    localMessage["fileSize"] = retryInfo.value("fileSize").toString();
    getMessageModel(conversationId)->upsertMessage(localMessage);

    QVariantMap wsMessage;
    wsMessage["type"] = "message";
    wsMessage["messageType"] = messageType;
    wsMessage["conversationId"] = conversationId;
    wsMessage["content"] = content;
    wsMessage["senderId"] = m_currentUserId;
    wsMessage["timestamp"] = timestamp;
    wsMessage["status"] = 1;
    wsMessage["fileId"] = retryInfo.value("fileId").toString();
    wsMessage["fileName"] = retryInfo.value("fileName").toString();
    wsMessage["fileUrl"] = retryInfo.value("fileUrl").toString();

    if (messageType == static_cast<int>(MessageType::Image)) {
        wsMessage["thumbnailUrl"] = retryInfo.value("thumbnailUrl").toString();
    }

    if (messageType == static_cast<int>(MessageType::File)) {
        wsMessage["fileSize"] = retryInfo.value("fileSize").toString();
    }

    dispatchOutgoingMessage(conversationId, messageId, wsMessage, queuedBeforeDispatch);
}

void ChatService::migrateConversationContext(const QString &oldConversationId, const QString &newConversationId)
{
    if (oldConversationId.isEmpty() || newConversationId.isEmpty() || oldConversationId == newConversationId) {
        return;
    }

    const QVariantMap oldConversation = m_conversationModel->getById(oldConversationId);
    QVariantMap updates = oldConversation;
    updates.remove("id");
    if (!updates.contains("type") || updates.value("type").toString().isEmpty()) {
        updates["type"] = QStringLiteral("single");
    }

    m_conversationModel->replaceConversationId(oldConversationId, newConversationId, updates);

    MessageModel *oldModel = m_messageModels.value(oldConversationId, nullptr);
    MessageModel *newModel = m_messageModels.value(newConversationId, nullptr);
    if (oldModel) {
        const QList<QVariantMap> existingMessages = oldModel->messages();
        if (newModel && newModel != oldModel) {
            for (QVariantMap message : existingMessages) {
                message["conversationId"] = newConversationId;
                newModel->upsertMessage(message);
            }
            MessageCache::instance()->cacheMessages(newConversationId, existingMessages);
            MessageCache::instance()->clear(oldConversationId);
            m_messageModels.remove(oldConversationId);
            oldModel->deleteLater();
        } else {
            oldModel->setConversationId(newConversationId);
            for (const QVariantMap &message : existingMessages) {
                QVariantMap updatesMessage;
                updatesMessage["messageId"] = message.value("messageId").toString();
                updatesMessage["conversationId"] = newConversationId;
                oldModel->upsertMessage(updatesMessage);
            }
            MessageCache::instance()->cacheMessages(newConversationId, oldModel->messages());
            MessageCache::instance()->clear(oldConversationId);
            m_messageModels.remove(oldConversationId);
            m_messageModels.insert(newConversationId, oldModel);
        }
    }

    auto pendingIt = m_pendingOutgoingMessages.begin();
    while (pendingIt != m_pendingOutgoingMessages.end()) {
        if (pendingIt.value() == oldConversationId) {
            pendingIt.value() = newConversationId;
        }
        ++pendingIt;
    }

    auto retryIt = m_retryableMessages.begin();
    while (retryIt != m_retryableMessages.end()) {
        QVariantMap retryInfo = retryIt.value();
        if (retryInfo.value("conversationId").toString() == oldConversationId) {
            retryInfo["conversationId"] = newConversationId;
            retryIt.value() = retryInfo;
        }
        ++retryIt;
    }

    if (m_currentConversationId == oldConversationId) {
        setCurrentConversationId(newConversationId);
    }

    emit conversationIdResolved(oldConversationId, newConversationId);
}

void ChatService::validateUserLogin(const QString &username, const QString &password)
{
    qDebug() << "[ChatService] validateUserLogin called with username:" << username;
    m_lastUsername = username; // 暂存当前登录的用户名
    m_userDAO->validateUserLogin(username, password);
}

void ChatService::handleLoginResult(bool success, const QString &userId, const QString &token, const QString &error)
{
    qDebug() << "[ChatService] handleLoginResult called, success:" << success << "userId:" << userId;

    if (success) {
        m_currentUserId = userId;
        m_currentUserName = m_lastUsername;
        m_currentUserAvatar.clear();
        m_currentUserGender.clear();
        m_currentUserRegion.clear();
        m_currentUserPhone.clear();
        m_currentUserEmail.clear();
        m_currentUserContact.clear();
        m_currentUserBio.clear();

        // ✅ 登录成功后立即保存用户名和密码（如果启用了记住密码）
        if (m_rememberPasswordEnabled) {
            QSettings settings;
            settings.setValue("settings/lastUsername", m_lastUsername);
            settings.setValue("settings/lastPassword", m_lastPassword);
            // 使用 SecureStorage 加密存储密码
            SecureStorage::instance()->setEncryptedPassword(m_lastUsername, m_lastPassword);
            qDebug() << "[ChatService] Credentials saved to settings securely";
        }

        // 保存 Token - 使用 SecureStorage 安全存储
        saveToken(token);
        NetworkClient::instance()->setToken(token);

        if (m_conversationModel) {
            m_conversationModel->setCurrentUserId(userId);
        }

        emit currentUserIdChanged();
        emit currentUserNameChanged();
        emit currentUserAvatarChanged();
        emit currentUserGenderChanged();
        emit currentUserRegionChanged();
        emit currentUserPhoneChanged();
        emit currentUserEmailChanged();
        emit currentUserContactChanged();
        emit currentUserBioChanged();
        emit loginResult(true, "");

        // 登录成功后重新连接 WebSocket 以进行认证
        qDebug() << "[ChatService] Reconnecting WebSocket after login...";
        m_webSocketClient->disconnectFromServer();
        connectToDefaultChatServer();

        // 登录成功后开始拉取数据
        initializeSampleData();
        loadUserProfile();
    } else {
        emit loginResult(false, error);
    }
}

void ChatService::registerNewUser(const QString &username, const QString &password)
{
    m_userDAO->registerNewUser(username, password);
}

void ChatService::handleRegisterResult(bool success, const QString &/*userId*/, const QString &error)
{
    emit registerResult(success, error);
}

void ChatService::requestPasswordReset(const QString &username)
{
    qDebug() << "[ChatService] requestPasswordReset called for" << username;
    m_userDAO->requestPasswordReset(username);
}

void ChatService::resetPassword(const QString &resetCode, const QString &newPassword)
{
    qDebug() << "[ChatService] resetPassword called";
    m_userDAO->resetPasswordWithCode(resetCode, newPassword);
}

void ChatService::adminSetPassword(const QString &userId, const QString &newPassword)
{
    qDebug() << "[ChatService] adminSetPassword called for" << userId;
    m_userDAO->updateUserPassword(userId, newPassword);
}

bool ChatService::isDatabaseConnected() const
{
    // 应用现在经由 Node.js 服务端进行数据库通信，复用 WebSocket 状态作为后端连通性状态
    return isConnected();
}

void ChatService::setNotificationsEnabled(bool enabled)
{
    if (m_notificationsEnabled != enabled) {
        m_notificationsEnabled = enabled;
        QSettings settings;
        settings.setValue("settings/notifications", enabled);
        emit notificationsEnabledChanged();
        qDebug() << "[ChatService] Notifications setting changed:" << enabled;
    }
}

void ChatService::setSoundEnabled(bool enabled)
{
    if (m_soundEnabled != enabled) {
        m_soundEnabled = enabled;
        QSettings settings;
        settings.setValue("settings/sound", enabled);
        emit soundEnabledChanged();
        qDebug() << "[ChatService] Sound setting changed:" << enabled;
    }
}

void ChatService::setAutoLoginEnabled(bool enabled)
{
    if (m_autoLoginEnabled != enabled) {
        m_autoLoginEnabled = enabled;
        QSettings settings;
        settings.setValue("settings/autoLogin", enabled);
        emit autoLoginEnabledChanged();
        qDebug() << "[ChatService] AutoLogin setting changed:" << enabled;
    }
}

void ChatService::setRememberPasswordEnabled(bool enabled)
{
    if (m_rememberPasswordEnabled != enabled) {
        m_rememberPasswordEnabled = enabled;
        QSettings settings;
        settings.setValue("settings/rememberPassword", enabled);
        if (!enabled) {
            setLastUsername("");
            setLastPassword("");
        }
        emit rememberPasswordEnabledChanged();
        qDebug() << "[ChatService] RememberPassword setting changed:" << enabled;
    }
}

void ChatService::setLastUsername(const QString &username)
{
    if (m_lastUsername != username) {
        m_lastUsername = username;
        QSettings settings;
        settings.setValue("settings/lastUsername", username);
        emit lastUsernameChanged();
    }
}

void ChatService::setLastPassword(const QString &password)
{
    if (m_lastPassword != password) {
        m_lastPassword = password;
        QSettings settings;
        settings.setValue("settings/lastPassword", password);
        emit lastPasswordChanged();
    }
}

void ChatService::sendFriendRequest(const QString &toUserId)
{
    qDebug() << "[ChatService] sendFriendRequest called, toUserId:" << toUserId << "currentUserId:" << m_currentUserId;
    
    if (m_currentUserId.isEmpty() || toUserId.isEmpty()) {
        qDebug() << "[ChatService] sendFriendRequest aborted - empty userId";
        return;
    }
    
    qDebug() << "[ChatService] Calling m_friendRequestDAO->sendRequest";
    m_friendRequestDAO->sendRequest(toUserId);
}

void ChatService::handleFriendRequestSent(bool success, const QString &error)
{
    qDebug() << "[ChatService] Friend request sent result:" << success << "error:" << error;
    // 发出信号通知 QML 关闭对话框
    emit friendRequestSentResult(success, error);
}

void ChatService::respondToFriendRequest(int requestId, bool accept)
{
    m_friendRequestDAO->updateRequestStatus(requestId, accept ? "accepted" : "rejected");
}

void ChatService::handleRequestStatusUpdated(bool success, int /*requestId*/, const QString &status)
{
    if (success && status == "accepted") {
        // 接受成功后刷新会话和好友列表
        initializeSampleData();
        getFriendList();
    }
}

void ChatService::getPendingFriendRequests()
{
    m_friendRequestDAO->getPendingRequests();
}

void ChatService::handleFriendRequestsLoaded(const QVector<FriendRequestDAO::FriendRequest> &requests)
{
    QVariantList list;
    for (const auto &req : requests) {
        QVariantMap map;
        map["requestId"] = req.requestId;
        map["fromId"] = req.fromId;
        map["fromUsername"] = req.fromUsername;
        map["status"] = req.status;
        map["createdAt"] = req.createdAt;
        list.append(map);
    }
    emit pendingRequestsLoaded(list);
}

void ChatService::getFriendList()
{
    m_friendRequestDAO->getFriends();
}

void ChatService::getFriendProfile(const QString &friendId)
{
    if (friendId.isEmpty()) {
        qWarning() << "[ChatService] getFriendProfile called with empty friendId";
        return;
    }
    qDebug() << "[ChatService] Getting friend profile for:" << friendId;
    m_friendRequestDAO->getFriendProfile(friendId);
}

void ChatService::handleFriendListLoaded(const QVector<FriendRequestDAO::FriendInfo> &friends)
{
    QVariantList list;
    for (const auto &friendInfo : friends) {
        QVariantMap map;
        map["userId"]    = friendInfo.userId;
        map["username"]  = friendInfo.username;
        map["avatar"]    = friendInfo.avatar;
        map["status"]    = friendInfo.status.isEmpty() ? QString("在线") : friendInfo.status;
        map["signature"] = friendInfo.signature;
        map["isMale"]    = friendInfo.isMale;
        map["age"]       = friendInfo.age;
        map["region"]    = friendInfo.region;
        list.append(map);
        
        // 保存好友版本号（初始为 1）
        if (!m_friendVersions.contains(friendInfo.userId)) {
            m_friendVersions[friendInfo.userId] = 1;
        }
    }
    emit friendListLoaded(list);
}

void ChatService::handleFriendProfileLoaded(const FriendRequestDAO::FriendInfo &friendInfo)
{
    if (friendInfo.userId.isEmpty()) {
        qWarning() << "[ChatService] handleFriendProfileLoaded: empty friend info";
        return;
    }
    
    QVariantMap map;
    map["userId"]    = friendInfo.userId;
    map["username"]  = friendInfo.username;
    map["avatar"]    = friendInfo.avatar;
    map["status"]    = friendInfo.status.isEmpty() ? QString("在线") : friendInfo.status;
    map["signature"] = friendInfo.signature;
    map["isMale"]    = friendInfo.isMale;
    map["age"]       = friendInfo.age;
    map["region"]    = friendInfo.region;

    syncSingleConversationProfile(m_conversationModel,
                                  friendInfo.userId,
                                  friendInfo.username,
                                  friendInfo.avatar);
    
    qDebug() << "[ChatService] Friend detail loaded:" << friendInfo.username;
    emit friendProfileUpdated(map);
}

void ChatService::checkIsFriend(const QString &userId)
{
    // 目前 DAO 还不支持 checkIsFriend，或者可以从本地好友列表缓存查
    // 这里先占位
    emit isFriendResult(userId, false);
}

// Token 管理方法 - 使用 SecureStorage 安全存储
void ChatService::saveToken(const QString &token)
{
    SecureStorage::instance()->setAuthToken(token);
    qDebug() << "[ChatService] Token saved securely";
}

QString ChatService::getToken() const
{
    QString token = SecureStorage::instance()->authToken();
    qDebug() << "[ChatService] Token retrieved securely, length:" << token.length();
    return token;
}

void ChatService::clearToken()
{
    SecureStorage::instance()->clearAuthToken();
    qDebug() << "[ChatService] Token cleared";
}
