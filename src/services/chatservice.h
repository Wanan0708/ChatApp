// Headers/services/chatservice.h
#pragma once

#include <QObject>
#include <QHash>
#include <QVariantMap>
#include <QVector>
#include "../models/conversation.h"
#include "../models/message.h"
#include "../dao/userdao.h"
#include "../dao/friendrequestdao.h"

class ConversationModel;
class MessageModel;
class WebSocketClient;
class ConversationDAO;
class MessageDAO;

class ChatService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ConversationModel* conversationModel READ conversationModel CONSTANT)
    Q_PROPERTY(QString currentConversationId READ currentConversationId WRITE setCurrentConversationId NOTIFY currentConversationIdChanged)
    Q_PROPERTY(QString currentUserId READ currentUserId WRITE setCurrentUserId NOTIFY currentUserIdChanged)
    Q_PROPERTY(QString currentUserName READ currentUserName WRITE setCurrentUserName NOTIFY currentUserNameChanged)
    Q_PROPERTY(QString currentUserAvatar READ currentUserAvatar WRITE setCurrentUserAvatar NOTIFY currentUserAvatarChanged)
    Q_PROPERTY(QString currentUserGender READ currentUserGender WRITE setCurrentUserGender NOTIFY currentUserGenderChanged)
    Q_PROPERTY(QString currentUserRegion READ currentUserRegion WRITE setCurrentUserRegion NOTIFY currentUserRegionChanged)
    Q_PROPERTY(QString currentUserPhone READ currentUserPhone WRITE setCurrentUserPhone NOTIFY currentUserPhoneChanged)
    Q_PROPERTY(QString currentUserEmail READ currentUserEmail WRITE setCurrentUserEmail NOTIFY currentUserEmailChanged)
    Q_PROPERTY(QString currentUserContact READ currentUserContact WRITE setCurrentUserContact NOTIFY currentUserContactChanged)
    Q_PROPERTY(QString currentUserBio READ currentUserBio WRITE setCurrentUserBio NOTIFY currentUserBioChanged)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(bool notificationsEnabled READ notificationsEnabled WRITE setNotificationsEnabled NOTIFY notificationsEnabledChanged)
    Q_PROPERTY(bool soundEnabled READ soundEnabled WRITE setSoundEnabled NOTIFY soundEnabledChanged)
    Q_PROPERTY(bool autoLoginEnabled READ autoLoginEnabled WRITE setAutoLoginEnabled NOTIFY autoLoginEnabledChanged)
    Q_PROPERTY(bool rememberPasswordEnabled READ rememberPasswordEnabled WRITE setRememberPasswordEnabled NOTIFY rememberPasswordEnabledChanged)
    Q_PROPERTY(QString lastUsername READ lastUsername WRITE setLastUsername NOTIFY lastUsernameChanged)
    Q_PROPERTY(QString lastPassword READ lastPassword WRITE setLastPassword NOTIFY lastPasswordChanged)

public:
    static ChatService* instance();

    ConversationModel* conversationModel() const { return m_conversationModel; }
    Q_INVOKABLE MessageModel* getMessageModel(const QString &conversationId);

    QString currentUserId() const { return m_currentUserId; }
    QString currentUserName() const { return m_currentUserName; }
    QString currentUserAvatar() const { return m_currentUserAvatar; }
    QString currentUserGender() const { return m_currentUserGender; }
    QString currentUserRegion() const { return m_currentUserRegion; }
    QString currentUserPhone() const { return m_currentUserPhone; }
    QString currentUserEmail() const { return m_currentUserEmail; }
    QString currentUserContact() const { return m_currentUserContact; }
    QString currentUserBio() const { return m_currentUserBio; }

    Q_INVOKABLE void setCurrentUserId(const QString &userId);
    Q_INVOKABLE void setCurrentUserName(const QString &userName);
    Q_INVOKABLE void setCurrentUserAvatar(const QString &avatar);
    void setCurrentUserGender(const QString &gender);
    void setCurrentUserRegion(const QString &region);
    void setCurrentUserPhone(const QString &phone);
    void setCurrentUserEmail(const QString &email);
    void setCurrentUserContact(const QString &contact);
    void setCurrentUserBio(const QString &bio);

    QString currentConversationId() const { return m_currentConversationId; }
    void setCurrentConversationId(const QString &id);

    Q_INVOKABLE void sendMessage(const QString &conversationId, const QString &content);
    Q_INVOKABLE void markConversationRead(const QString &conversationId);
    Q_INVOKABLE void setCurrentConversation(const QString &conversationId);
    Q_INVOKABLE void refreshConversations();
    Q_INVOKABLE void refreshMessages(const QString &conversationId);

    Q_INVOKABLE void logout();
    Q_INVOKABLE void changePassword(const QString &newPassword);
    Q_INVOKABLE void connectToDefaultChatServer();
    Q_INVOKABLE void connectToChatServer(const QString &serverUrl);
    Q_INVOKABLE void disconnectFromChatServer();
    Q_INVOKABLE int totalUnreadCount() const;
    Q_INVOKABLE bool hasUnreadMessages() const;
    Q_INVOKABLE void searchUsers(const QString &searchTerm);
    Q_INVOKABLE void loadUserProfile();
    Q_INVOKABLE void saveUserProfile(const QVariantMap &profile);
    bool isConnected() const;

    // 好友申请相关
    Q_INVOKABLE void sendFriendRequest(const QString &toUserId);
    Q_INVOKABLE void respondToFriendRequest(int requestId, bool accept);
    Q_INVOKABLE void getPendingFriendRequests();
    Q_INVOKABLE void getFriendList();
    Q_INVOKABLE void checkIsFriend(const QString &userId);
    Q_INVOKABLE void validateUserLogin(const QString &username, const QString &password);
    Q_INVOKABLE void registerNewUser(const QString &username, const QString &password);

    // 忘记/重置密码流程 (异步)
    Q_INVOKABLE void requestPasswordReset(const QString &username);
    Q_INVOKABLE void resetPassword(const QString &resetCode, const QString &newPassword);
    Q_INVOKABLE void adminSetPassword(const QString &userId, const QString &newPassword);

    // --- 数据库直连相关已移除 ---

    // 通用设置属性访问器
    bool notificationsEnabled() const { return m_notificationsEnabled; }
    void setNotificationsEnabled(bool enabled);
    bool soundEnabled() const { return m_soundEnabled; }
    void setSoundEnabled(bool enabled);
    bool autoLoginEnabled() const { return m_autoLoginEnabled; }
    void setAutoLoginEnabled(bool enabled);
    bool rememberPasswordEnabled() const { return m_rememberPasswordEnabled; }
    void setRememberPasswordEnabled(bool enabled);
    QString lastUsername() const { return m_lastUsername; }
    Q_INVOKABLE void setLastUsername(const QString &username);
    QString lastPassword() const { return m_lastPassword; }
    Q_INVOKABLE void setLastPassword(const QString &password);

    // 数据库连接状态检查
    Q_INVOKABLE bool isDatabaseConnected() const;

signals:
    void conversationModelChanged();
    void currentConversationIdChanged();
    void messageReceived(const QString &conversationId, const QVariantMap &message);
    void conversationUpdated(const QString &conversationId);
    void connectedChanged(bool connected);
    void currentUserIdChanged();
    void currentUserNameChanged();
    void currentUserAvatarChanged();
    void currentUserGenderChanged();
    void currentUserRegionChanged();
    void currentUserPhoneChanged();
    void currentUserEmailChanged();
    void currentUserContactChanged();
    void currentUserBioChanged();
    void userProfileSaveResult(bool success, const QString &message);
    void notificationsEnabledChanged();
    void soundEnabledChanged();
    void autoLoginEnabledChanged();
    void rememberPasswordEnabledChanged();
    void lastUsernameChanged();
    void lastPasswordChanged();

    // 好友申请信号
    void friendRequestReceived(const QVariantMap &request);
    void friendshipEstablished(const QString &friendId, const QString &friendName);
    void friendRequestSentResult(bool success, const QString &error);
    void searchResult(const QVariantList &results);
    void pendingRequestsLoaded(const QVariantList &requests);
    void friendListLoaded(const QVariantList &friends);
    void isFriendResult(const QString &userId, bool isFriend);

    // 认证结果信号
    void loginResult(bool success, const QString &error);
    void registerResult(bool success, const QString &error);
    
    // 密码重置结果信号
    void passwordResetCodeGenerated(bool success, const QString &resetCode, const QString &error);
    void passwordResetResult(bool success, const QString &error);
    void passwordUpdateResult(bool success, const QString &error);

private slots:
    void onWebSocketMessageReceived(const QVariantMap &message);
    void onWebSocketConnected();
    void onWebSocketDisconnected();

    // DAO 异步回调
    void handleLoginResult(bool success, const QString &userId, const QString &token, const QString &error);
    void handleRegisterResult(bool success, const QString &userId, const QString &error);
    void handleConversationsLoaded(const QVector<Conversation> &conversations);
    void handleMessagesLoaded(const QString &conversationId, const QVector<Message> &messages);
    void handleSearchUserResult(const QVector<UserDAO::UserSearchResult> &results);
    void handleUserProfileResult(bool success, const QVariantMap &profile, const QString &error);
    void handleUserProfileUpdated(bool success, const QString &message);
    void handleFriendRequestsLoaded(const QVector<FriendRequestDAO::FriendRequest> &requests);
    void handleFriendListLoaded(const QVector<FriendRequestDAO::FriendInfo> &friends);
    void handleFriendRequestSent(bool success, const QString &error);
    void handleRequestStatusUpdated(bool success, int requestId, const QString &status);

private:
    explicit ChatService(QObject *parent = nullptr);
    ~ChatService() override;

    // 确保数据库已连接，如果未连接则尝试重新连接
    bool ensureDatabaseConnected();

    ConversationModel *m_conversationModel;
    QHash<QString, MessageModel*> m_messageModels;
    WebSocketClient *m_webSocketClient;  // ✅ 网络客户端
    
    // DAO 对象用于数据库访问
    ConversationDAO *m_conversationDAO;
    MessageDAO *m_messageDAO;
    UserDAO *m_userDAO;
    FriendRequestDAO *m_friendRequestDAO;

    QString m_currentUserId;
    QString m_currentUserName;
    QString m_currentUserAvatar;
    QString m_currentUserGender;
    QString m_currentUserRegion;
    QString m_currentUserPhone;
    QString m_currentUserEmail;
    QString m_currentUserContact;
    QString m_currentUserBio;
    QString m_currentConversationId;

    bool m_notificationsEnabled;
    bool m_soundEnabled;
    bool m_autoLoginEnabled;
    bool m_rememberPasswordEnabled;
    QString m_lastUsername;
    QString m_lastPassword;

    void initializeSampleData();
    void processIncomingMessage(const QVariantMap &message);

    friend class ConversationModel;
    friend class MessageModel;
};
