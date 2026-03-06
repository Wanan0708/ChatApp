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
#include <QDateTime>
#include <QDebug>
#include <QTimer>
#include <QCoreApplication>
#include <QRandomGenerator>
#include <QSettings>

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
    connect(m_friendRequestDAO, &FriendRequestDAO::pendingRequestsLoaded, this, &ChatService::handleFriendRequestsLoaded);
    connect(m_friendRequestDAO, &FriendRequestDAO::friendsLoaded, this, &ChatService::handleFriendListLoaded);
    connect(m_friendRequestDAO, &FriendRequestDAO::requestSent, this, &ChatService::handleFriendRequestSent);
    connect(m_friendRequestDAO, &FriendRequestDAO::requestStatusUpdated, this, &ChatService::handleRequestStatusUpdated);

    // 连接 WebSocket 信号
    connect(m_webSocketClient, &WebSocketClient::connected, this, &ChatService::onWebSocketConnected);
    connect(m_webSocketClient, &WebSocketClient::disconnected, this, &ChatService::onWebSocketDisconnected);
    connect(m_webSocketClient, &WebSocketClient::messageReceived, this, &ChatService::onWebSocketMessageReceived);
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
            {"lastMessage", conv.lastMessage},
            {"time", conv.time},
            {"type", conv.type},
            {"unreadCount", conv.unreadCount}
        });

        // 2. 异步拉取每个对话的消息历史
        m_messageDAO->getConversationMessages(conv.id, 50);
    }
}

void ChatService::handleMessagesLoaded(const QString &conversationId, const QVector<Message> &messages)
{
    qDebug() << "[ChatService] Loaded" << messages.size() << "messages for" << conversationId;
    MessageModel *msgModel = getMessageModel(conversationId);

    // 先清除旧消息，避免重复
    msgModel->clearMessages();

    for (const Message &msg : messages) {
        QVariantMap msgMap;
        msgMap["messageId"] = msg.messageId;
        msgMap["conversationId"] = msg.conversationId;
        msgMap["content"] = msg.content;
        msgMap["senderId"] = msg.senderId;
        msgMap["timestamp"] = msg.timestamp;
        msgModel->addMessage(msgMap);
    }
    
    // 发出消息刷新完成信号，通知 QML 滚动到底部
    emit messagesRefreshed(conversationId);
}

MessageModel* ChatService::getMessageModel(const QString &conversationId)
{
    if (!m_messageModels.contains(conversationId)) {
            MessageModel *model = new MessageModel(conversationId, this);
            model->setCurrentUserId(m_currentUserId);
            m_messageModels[conversationId] = model;
        }
    return m_messageModels.value(conversationId);
}

void ChatService::setCurrentConversationId(const QString &id)
{
    if (m_currentConversationId != id) {
        m_currentConversationId = id;
        m_conversationModel->setCurrentConversation(id);
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
    if (content.trimmed().isEmpty()) return;

    QString trimmedContent = content.trimmed();

    // 1. 创建消息对象
    QVariantMap message;
    message["type"] = "message";
    message["conversationId"] = conversationId;
    message["content"] = trimmedContent;
    message["senderId"] = m_currentUserId;
    message["timestamp"] = QDateTime::currentMSecsSinceEpoch();

    // 1.5 乐观更新：立即添加到本地 MessageModel（无论连接状态）
    // 这样 QML 界面立即显示消息
    getMessageModel(conversationId)->addMessage(message);
    qDebug() << "[ChatService] Message added to local model optimistically";

    // 2. 更新会话列表元数据
    QString displayContent = trimmedContent.length() > 30
        ? trimmedContent.left(27) + "..."
        : trimmedContent;
    // 使用 TimeFormatter 统一时间格式
    QString displayTime = TimeFormatter::formatChatTime(message["timestamp"].toLongLong());

    m_conversationModel->updateConversation(conversationId, {
        {"lastMessage", displayContent},
        {"time", displayTime},
        {"unreadCount", 0}
    });

    // 3. 通过 WebSocket 发送（如果已连接）
    if (m_webSocketClient->isConnected()) {
        m_webSocketClient->sendMessage(message);
        qDebug() << "[ChatService] Message sent via WebSocket";
    } else {
        // 标记为离线（供服务器或后续处理识别）
        message["isOffline"] = true;
        qDebug() << "[ChatService] WebSocket not connected, message will be sent when reconnected";
    }

    // 4. 发出信号供QML响应（滚动到底部等）
    emit messageReceived(conversationId, message);
}

void ChatService::markConversationRead(const QString &conversationId)
{
    m_conversationModel->markRead(conversationId);
}

void ChatService::refreshMessages(const QString &conversationId)
{
    qDebug() << "[ChatService] Refreshing messages for:" << conversationId;
    m_messageDAO->getConversationMessages(conversationId, 50);
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
    m_userDAO->updateMyProfile(profile);
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

    setCurrentUserName(profile.value("username").toString());
    setCurrentUserAvatar(profile.value("avatar").toString());
    setCurrentUserGender(profile.value("gender").toString());
    setCurrentUserRegion(profile.value("region").toString());
    setCurrentUserPhone(profile.value("phone").toString());
    setCurrentUserEmail(profile.value("email").toString());
    setCurrentUserContact(profile.value("contact").toString());
    setCurrentUserBio(profile.value("bio").toString());
}

void ChatService::handleUserProfileUpdated(bool success, const QString &message)
{
    if (success) {
        loadUserProfile();
        emit userProfileSaveResult(true, message.isEmpty() ? "资料保存成功" : message);
    } else {
        emit userProfileSaveResult(false, message.isEmpty() ? "资料保存失败" : message);
    }
}

void ChatService::logout()
{
    qDebug() << "[ChatService] Logging out...";

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
    if (m_currentUserAvatar != avatar) {
        m_currentUserAvatar = avatar;
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

bool ChatService::isConnected() const
{
    return m_webSocketClient->isConnected();
}

void ChatService::onWebSocketMessageReceived(const QVariantMap &rawMessage)
{
    QString msgType = rawMessage.value("type").toString();

    if (msgType == "message") {
        processIncomingMessage(rawMessage);
    } else if (msgType == "system") {
        QString subtype = rawMessage.value("subtype").toString();
        if (subtype == "conversation_created") {
            QString convId = rawMessage.value("conversationId").toString();
            QString title = rawMessage.value("title").toString();
            QString type = rawMessage.value("type", "user").toString();

            m_conversationModel->updateConversation(convId, {
                {"title", title},
                {"type", type}
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
    } else if (msgType == "heartbeat") {
        // 忽略
    } else {
        qDebug() << "[ChatService] Unknown message type:" << msgType;
    }
}

void ChatService::processIncomingMessage(const QVariantMap &message)
{
    QString conversationId = message.value("conversationId").toString();
    QString senderId = message.value("senderId").toString();
    QString content = message.value("content").toString();
    qint64 timestamp = message.value("timestamp").toLongLong();

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
    getMessageModel(conversationId)->addMessage(message);

    // 2. 更新会话元数据
    bool isCurrent = (conversationId == m_currentConversationId);

    QString displayContent = content.length() > 30
        ? content.left(27) + "..."
        : content;
    // 使用 TimeFormatter 统一时间格式
    QString displayTime = TimeFormatter::formatChatTime(timestamp);

    if (isCurrent) {
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
    emit messageReceived(conversationId, message);
    emit conversationUpdated(conversationId);

    qDebug() << "[ChatService] Processed incoming message from" << senderId
             << "to conversation" << conversationId;
}

void ChatService::onWebSocketConnected()
{
    qDebug() << "[ChatService] WebSocket connected";
    emit connectedChanged(true);
    
    // 发送认证消息 - 使用 JWT Token
    if (!m_currentUserId.isEmpty() && !NetworkClient::instance()->token().isEmpty()) {
        QVariantMap auth;
        auth["type"] = "auth";
        auth["token"] = NetworkClient::instance()->token();
        m_webSocketClient->sendMessage(auth);
        qDebug() << "[ChatService] Sent WebSocket auth for user:" << m_currentUserId;
    } else {
        qDebug() << "[ChatService] Skipping WebSocket auth - no userId or token";
    }
}

void ChatService::onWebSocketDisconnected()
{
    qDebug() << "[ChatService] WebSocket disconnected";
    emit connectedChanged(false);
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
            qDebug() << "[ChatService] Credentials saved to settings";
        }

        // 保存 Token
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
    }
    emit friendListLoaded(list);
}

void ChatService::checkIsFriend(const QString &userId)
{
    // 目前 DAO 还不支持 checkIsFriend，或者可以从本地好友列表缓存查
    // 这里先占位
    emit isFriendResult(userId, false);
}
