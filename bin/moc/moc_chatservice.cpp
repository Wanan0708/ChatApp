/****************************************************************************
** Meta object code from reading C++ file 'chatservice.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../src/services/chatservice.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'chatservice.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ChatService_t {
    QByteArrayData data[148];
    char stringdata0[2553];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ChatService_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ChatService_t qt_meta_stringdata_ChatService = {
    {
QT_MOC_LITERAL(0, 0, 11), // "ChatService"
QT_MOC_LITERAL(1, 12, 24), // "conversationModelChanged"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 28), // "currentConversationIdChanged"
QT_MOC_LITERAL(4, 67, 15), // "messageReceived"
QT_MOC_LITERAL(5, 83, 14), // "conversationId"
QT_MOC_LITERAL(6, 98, 7), // "message"
QT_MOC_LITERAL(7, 106, 19), // "conversationUpdated"
QT_MOC_LITERAL(8, 126, 17), // "messagesRefreshed"
QT_MOC_LITERAL(9, 144, 16), // "connectedChanged"
QT_MOC_LITERAL(10, 161, 9), // "connected"
QT_MOC_LITERAL(11, 171, 20), // "currentUserIdChanged"
QT_MOC_LITERAL(12, 192, 22), // "currentUserNameChanged"
QT_MOC_LITERAL(13, 215, 24), // "currentUserAvatarChanged"
QT_MOC_LITERAL(14, 240, 24), // "currentUserGenderChanged"
QT_MOC_LITERAL(15, 265, 24), // "currentUserRegionChanged"
QT_MOC_LITERAL(16, 290, 23), // "currentUserPhoneChanged"
QT_MOC_LITERAL(17, 314, 23), // "currentUserEmailChanged"
QT_MOC_LITERAL(18, 338, 25), // "currentUserContactChanged"
QT_MOC_LITERAL(19, 364, 21), // "currentUserBioChanged"
QT_MOC_LITERAL(20, 386, 21), // "userProfileSaveResult"
QT_MOC_LITERAL(21, 408, 7), // "success"
QT_MOC_LITERAL(22, 416, 27), // "notificationsEnabledChanged"
QT_MOC_LITERAL(23, 444, 19), // "soundEnabledChanged"
QT_MOC_LITERAL(24, 464, 23), // "autoLoginEnabledChanged"
QT_MOC_LITERAL(25, 488, 30), // "rememberPasswordEnabledChanged"
QT_MOC_LITERAL(26, 519, 19), // "lastUsernameChanged"
QT_MOC_LITERAL(27, 539, 19), // "lastPasswordChanged"
QT_MOC_LITERAL(28, 559, 21), // "friendRequestReceived"
QT_MOC_LITERAL(29, 581, 7), // "request"
QT_MOC_LITERAL(30, 589, 21), // "friendshipEstablished"
QT_MOC_LITERAL(31, 611, 8), // "friendId"
QT_MOC_LITERAL(32, 620, 10), // "friendName"
QT_MOC_LITERAL(33, 631, 23), // "friendRequestSentResult"
QT_MOC_LITERAL(34, 655, 5), // "error"
QT_MOC_LITERAL(35, 661, 12), // "searchResult"
QT_MOC_LITERAL(36, 674, 7), // "results"
QT_MOC_LITERAL(37, 682, 21), // "pendingRequestsLoaded"
QT_MOC_LITERAL(38, 704, 8), // "requests"
QT_MOC_LITERAL(39, 713, 16), // "friendListLoaded"
QT_MOC_LITERAL(40, 730, 7), // "friends"
QT_MOC_LITERAL(41, 738, 14), // "isFriendResult"
QT_MOC_LITERAL(42, 753, 6), // "userId"
QT_MOC_LITERAL(43, 760, 8), // "isFriend"
QT_MOC_LITERAL(44, 769, 11), // "loginResult"
QT_MOC_LITERAL(45, 781, 14), // "registerResult"
QT_MOC_LITERAL(46, 796, 26), // "passwordResetCodeGenerated"
QT_MOC_LITERAL(47, 823, 9), // "resetCode"
QT_MOC_LITERAL(48, 833, 19), // "passwordResetResult"
QT_MOC_LITERAL(49, 853, 20), // "passwordUpdateResult"
QT_MOC_LITERAL(50, 874, 26), // "onWebSocketMessageReceived"
QT_MOC_LITERAL(51, 901, 20), // "onWebSocketConnected"
QT_MOC_LITERAL(52, 922, 23), // "onWebSocketDisconnected"
QT_MOC_LITERAL(53, 946, 17), // "handleLoginResult"
QT_MOC_LITERAL(54, 964, 5), // "token"
QT_MOC_LITERAL(55, 970, 20), // "handleRegisterResult"
QT_MOC_LITERAL(56, 991, 25), // "handleConversationsLoaded"
QT_MOC_LITERAL(57, 1017, 21), // "QVector<Conversation>"
QT_MOC_LITERAL(58, 1039, 13), // "conversations"
QT_MOC_LITERAL(59, 1053, 20), // "handleMessagesLoaded"
QT_MOC_LITERAL(60, 1074, 16), // "QVector<Message>"
QT_MOC_LITERAL(61, 1091, 8), // "messages"
QT_MOC_LITERAL(62, 1100, 22), // "handleSearchUserResult"
QT_MOC_LITERAL(63, 1123, 34), // "QVector<UserDAO::UserSearchRe..."
QT_MOC_LITERAL(64, 1158, 23), // "handleUserProfileResult"
QT_MOC_LITERAL(65, 1182, 7), // "profile"
QT_MOC_LITERAL(66, 1190, 24), // "handleUserProfileUpdated"
QT_MOC_LITERAL(67, 1215, 26), // "handleFriendRequestsLoaded"
QT_MOC_LITERAL(68, 1242, 40), // "QVector<FriendRequestDAO::Fri..."
QT_MOC_LITERAL(69, 1283, 22), // "handleFriendListLoaded"
QT_MOC_LITERAL(70, 1306, 37), // "QVector<FriendRequestDAO::Fri..."
QT_MOC_LITERAL(71, 1344, 23), // "handleFriendRequestSent"
QT_MOC_LITERAL(72, 1368, 26), // "handleRequestStatusUpdated"
QT_MOC_LITERAL(73, 1395, 9), // "requestId"
QT_MOC_LITERAL(74, 1405, 6), // "status"
QT_MOC_LITERAL(75, 1412, 15), // "getMessageModel"
QT_MOC_LITERAL(76, 1428, 13), // "MessageModel*"
QT_MOC_LITERAL(77, 1442, 16), // "setCurrentUserId"
QT_MOC_LITERAL(78, 1459, 18), // "setCurrentUserName"
QT_MOC_LITERAL(79, 1478, 8), // "userName"
QT_MOC_LITERAL(80, 1487, 20), // "setCurrentUserAvatar"
QT_MOC_LITERAL(81, 1508, 6), // "avatar"
QT_MOC_LITERAL(82, 1515, 11), // "sendMessage"
QT_MOC_LITERAL(83, 1527, 7), // "content"
QT_MOC_LITERAL(84, 1535, 16), // "sendImageMessage"
QT_MOC_LITERAL(85, 1552, 8), // "filePath"
QT_MOC_LITERAL(86, 1561, 15), // "sendFileMessage"
QT_MOC_LITERAL(87, 1577, 13), // "pickLocalFile"
QT_MOC_LITERAL(88, 1591, 9), // "imageOnly"
QT_MOC_LITERAL(89, 1601, 13), // "recallMessage"
QT_MOC_LITERAL(90, 1615, 9), // "messageId"
QT_MOC_LITERAL(91, 1625, 20), // "markConversationRead"
QT_MOC_LITERAL(92, 1646, 22), // "setCurrentConversation"
QT_MOC_LITERAL(93, 1669, 20), // "refreshConversations"
QT_MOC_LITERAL(94, 1690, 15), // "refreshMessages"
QT_MOC_LITERAL(95, 1706, 6), // "logout"
QT_MOC_LITERAL(96, 1713, 14), // "changePassword"
QT_MOC_LITERAL(97, 1728, 11), // "newPassword"
QT_MOC_LITERAL(98, 1740, 26), // "connectToDefaultChatServer"
QT_MOC_LITERAL(99, 1767, 19), // "connectToChatServer"
QT_MOC_LITERAL(100, 1787, 9), // "serverUrl"
QT_MOC_LITERAL(101, 1797, 24), // "disconnectFromChatServer"
QT_MOC_LITERAL(102, 1822, 16), // "totalUnreadCount"
QT_MOC_LITERAL(103, 1839, 17), // "hasUnreadMessages"
QT_MOC_LITERAL(104, 1857, 11), // "searchUsers"
QT_MOC_LITERAL(105, 1869, 10), // "searchTerm"
QT_MOC_LITERAL(106, 1880, 15), // "loadUserProfile"
QT_MOC_LITERAL(107, 1896, 15), // "saveUserProfile"
QT_MOC_LITERAL(108, 1912, 20), // "fetchOfflineMessages"
QT_MOC_LITERAL(109, 1933, 17), // "sendFriendRequest"
QT_MOC_LITERAL(110, 1951, 8), // "toUserId"
QT_MOC_LITERAL(111, 1960, 22), // "respondToFriendRequest"
QT_MOC_LITERAL(112, 1983, 6), // "accept"
QT_MOC_LITERAL(113, 1990, 24), // "getPendingFriendRequests"
QT_MOC_LITERAL(114, 2015, 13), // "getFriendList"
QT_MOC_LITERAL(115, 2029, 13), // "checkIsFriend"
QT_MOC_LITERAL(116, 2043, 17), // "validateUserLogin"
QT_MOC_LITERAL(117, 2061, 8), // "username"
QT_MOC_LITERAL(118, 2070, 8), // "password"
QT_MOC_LITERAL(119, 2079, 15), // "registerNewUser"
QT_MOC_LITERAL(120, 2095, 20), // "requestPasswordReset"
QT_MOC_LITERAL(121, 2116, 13), // "resetPassword"
QT_MOC_LITERAL(122, 2130, 16), // "adminSetPassword"
QT_MOC_LITERAL(123, 2147, 15), // "setLastUsername"
QT_MOC_LITERAL(124, 2163, 15), // "setLastPassword"
QT_MOC_LITERAL(125, 2179, 9), // "saveToken"
QT_MOC_LITERAL(126, 2189, 8), // "getToken"
QT_MOC_LITERAL(127, 2198, 10), // "clearToken"
QT_MOC_LITERAL(128, 2209, 19), // "isDatabaseConnected"
QT_MOC_LITERAL(129, 2229, 17), // "conversationModel"
QT_MOC_LITERAL(130, 2247, 18), // "ConversationModel*"
QT_MOC_LITERAL(131, 2266, 21), // "currentConversationId"
QT_MOC_LITERAL(132, 2288, 13), // "currentUserId"
QT_MOC_LITERAL(133, 2302, 15), // "currentUserName"
QT_MOC_LITERAL(134, 2318, 17), // "currentUserAvatar"
QT_MOC_LITERAL(135, 2336, 17), // "currentUserGender"
QT_MOC_LITERAL(136, 2354, 17), // "currentUserRegion"
QT_MOC_LITERAL(137, 2372, 16), // "currentUserPhone"
QT_MOC_LITERAL(138, 2389, 16), // "currentUserEmail"
QT_MOC_LITERAL(139, 2406, 18), // "currentUserContact"
QT_MOC_LITERAL(140, 2425, 14), // "currentUserBio"
QT_MOC_LITERAL(141, 2440, 11), // "isConnected"
QT_MOC_LITERAL(142, 2452, 20), // "notificationsEnabled"
QT_MOC_LITERAL(143, 2473, 12), // "soundEnabled"
QT_MOC_LITERAL(144, 2486, 16), // "autoLoginEnabled"
QT_MOC_LITERAL(145, 2503, 23), // "rememberPasswordEnabled"
QT_MOC_LITERAL(146, 2527, 12), // "lastUsername"
QT_MOC_LITERAL(147, 2540, 12) // "lastPassword"

    },
    "ChatService\0conversationModelChanged\0"
    "\0currentConversationIdChanged\0"
    "messageReceived\0conversationId\0message\0"
    "conversationUpdated\0messagesRefreshed\0"
    "connectedChanged\0connected\0"
    "currentUserIdChanged\0currentUserNameChanged\0"
    "currentUserAvatarChanged\0"
    "currentUserGenderChanged\0"
    "currentUserRegionChanged\0"
    "currentUserPhoneChanged\0currentUserEmailChanged\0"
    "currentUserContactChanged\0"
    "currentUserBioChanged\0userProfileSaveResult\0"
    "success\0notificationsEnabledChanged\0"
    "soundEnabledChanged\0autoLoginEnabledChanged\0"
    "rememberPasswordEnabledChanged\0"
    "lastUsernameChanged\0lastPasswordChanged\0"
    "friendRequestReceived\0request\0"
    "friendshipEstablished\0friendId\0"
    "friendName\0friendRequestSentResult\0"
    "error\0searchResult\0results\0"
    "pendingRequestsLoaded\0requests\0"
    "friendListLoaded\0friends\0isFriendResult\0"
    "userId\0isFriend\0loginResult\0registerResult\0"
    "passwordResetCodeGenerated\0resetCode\0"
    "passwordResetResult\0passwordUpdateResult\0"
    "onWebSocketMessageReceived\0"
    "onWebSocketConnected\0onWebSocketDisconnected\0"
    "handleLoginResult\0token\0handleRegisterResult\0"
    "handleConversationsLoaded\0"
    "QVector<Conversation>\0conversations\0"
    "handleMessagesLoaded\0QVector<Message>\0"
    "messages\0handleSearchUserResult\0"
    "QVector<UserDAO::UserSearchResult>\0"
    "handleUserProfileResult\0profile\0"
    "handleUserProfileUpdated\0"
    "handleFriendRequestsLoaded\0"
    "QVector<FriendRequestDAO::FriendRequest>\0"
    "handleFriendListLoaded\0"
    "QVector<FriendRequestDAO::FriendInfo>\0"
    "handleFriendRequestSent\0"
    "handleRequestStatusUpdated\0requestId\0"
    "status\0getMessageModel\0MessageModel*\0"
    "setCurrentUserId\0setCurrentUserName\0"
    "userName\0setCurrentUserAvatar\0avatar\0"
    "sendMessage\0content\0sendImageMessage\0"
    "filePath\0sendFileMessage\0pickLocalFile\0"
    "imageOnly\0recallMessage\0messageId\0"
    "markConversationRead\0setCurrentConversation\0"
    "refreshConversations\0refreshMessages\0"
    "logout\0changePassword\0newPassword\0"
    "connectToDefaultChatServer\0"
    "connectToChatServer\0serverUrl\0"
    "disconnectFromChatServer\0totalUnreadCount\0"
    "hasUnreadMessages\0searchUsers\0searchTerm\0"
    "loadUserProfile\0saveUserProfile\0"
    "fetchOfflineMessages\0sendFriendRequest\0"
    "toUserId\0respondToFriendRequest\0accept\0"
    "getPendingFriendRequests\0getFriendList\0"
    "checkIsFriend\0validateUserLogin\0"
    "username\0password\0registerNewUser\0"
    "requestPasswordReset\0resetPassword\0"
    "adminSetPassword\0setLastUsername\0"
    "setLastPassword\0saveToken\0getToken\0"
    "clearToken\0isDatabaseConnected\0"
    "conversationModel\0ConversationModel*\0"
    "currentConversationId\0currentUserId\0"
    "currentUserName\0currentUserAvatar\0"
    "currentUserGender\0currentUserRegion\0"
    "currentUserPhone\0currentUserEmail\0"
    "currentUserContact\0currentUserBio\0"
    "isConnected\0notificationsEnabled\0"
    "soundEnabled\0autoLoginEnabled\0"
    "rememberPasswordEnabled\0lastUsername\0"
    "lastPassword"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ChatService[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      89,   14, // methods
      18,  724, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      34,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  459,    2, 0x06 /* Public */,
       3,    0,  460,    2, 0x06 /* Public */,
       4,    2,  461,    2, 0x06 /* Public */,
       7,    1,  466,    2, 0x06 /* Public */,
       8,    1,  469,    2, 0x06 /* Public */,
       9,    1,  472,    2, 0x06 /* Public */,
      11,    0,  475,    2, 0x06 /* Public */,
      12,    0,  476,    2, 0x06 /* Public */,
      13,    0,  477,    2, 0x06 /* Public */,
      14,    0,  478,    2, 0x06 /* Public */,
      15,    0,  479,    2, 0x06 /* Public */,
      16,    0,  480,    2, 0x06 /* Public */,
      17,    0,  481,    2, 0x06 /* Public */,
      18,    0,  482,    2, 0x06 /* Public */,
      19,    0,  483,    2, 0x06 /* Public */,
      20,    2,  484,    2, 0x06 /* Public */,
      22,    0,  489,    2, 0x06 /* Public */,
      23,    0,  490,    2, 0x06 /* Public */,
      24,    0,  491,    2, 0x06 /* Public */,
      25,    0,  492,    2, 0x06 /* Public */,
      26,    0,  493,    2, 0x06 /* Public */,
      27,    0,  494,    2, 0x06 /* Public */,
      28,    1,  495,    2, 0x06 /* Public */,
      30,    2,  498,    2, 0x06 /* Public */,
      33,    2,  503,    2, 0x06 /* Public */,
      35,    1,  508,    2, 0x06 /* Public */,
      37,    1,  511,    2, 0x06 /* Public */,
      39,    1,  514,    2, 0x06 /* Public */,
      41,    2,  517,    2, 0x06 /* Public */,
      44,    2,  522,    2, 0x06 /* Public */,
      45,    2,  527,    2, 0x06 /* Public */,
      46,    3,  532,    2, 0x06 /* Public */,
      48,    2,  539,    2, 0x06 /* Public */,
      49,    2,  544,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      50,    1,  549,    2, 0x08 /* Private */,
      51,    0,  552,    2, 0x08 /* Private */,
      52,    0,  553,    2, 0x08 /* Private */,
      53,    4,  554,    2, 0x08 /* Private */,
      55,    3,  563,    2, 0x08 /* Private */,
      56,    1,  570,    2, 0x08 /* Private */,
      59,    2,  573,    2, 0x08 /* Private */,
      62,    1,  578,    2, 0x08 /* Private */,
      64,    3,  581,    2, 0x08 /* Private */,
      66,    2,  588,    2, 0x08 /* Private */,
      67,    1,  593,    2, 0x08 /* Private */,
      69,    1,  596,    2, 0x08 /* Private */,
      71,    2,  599,    2, 0x08 /* Private */,
      72,    3,  604,    2, 0x08 /* Private */,

 // methods: name, argc, parameters, tag, flags
      75,    1,  611,    2, 0x02 /* Public */,
      77,    1,  614,    2, 0x02 /* Public */,
      78,    1,  617,    2, 0x02 /* Public */,
      80,    1,  620,    2, 0x02 /* Public */,
      82,    2,  623,    2, 0x02 /* Public */,
      84,    2,  628,    2, 0x02 /* Public */,
      86,    2,  633,    2, 0x02 /* Public */,
      87,    1,  638,    2, 0x02 /* Public */,
      87,    0,  641,    2, 0x22 /* Public | MethodCloned */,
      89,    2,  642,    2, 0x02 /* Public */,
      91,    1,  647,    2, 0x02 /* Public */,
      92,    1,  650,    2, 0x02 /* Public */,
      93,    0,  653,    2, 0x02 /* Public */,
      94,    1,  654,    2, 0x02 /* Public */,
      95,    0,  657,    2, 0x02 /* Public */,
      96,    1,  658,    2, 0x02 /* Public */,
      98,    0,  661,    2, 0x02 /* Public */,
      99,    1,  662,    2, 0x02 /* Public */,
     101,    0,  665,    2, 0x02 /* Public */,
     102,    0,  666,    2, 0x02 /* Public */,
     103,    0,  667,    2, 0x02 /* Public */,
     104,    1,  668,    2, 0x02 /* Public */,
     106,    0,  671,    2, 0x02 /* Public */,
     107,    1,  672,    2, 0x02 /* Public */,
     108,    0,  675,    2, 0x02 /* Public */,
     109,    1,  676,    2, 0x02 /* Public */,
     111,    2,  679,    2, 0x02 /* Public */,
     113,    0,  684,    2, 0x02 /* Public */,
     114,    0,  685,    2, 0x02 /* Public */,
     115,    1,  686,    2, 0x02 /* Public */,
     116,    2,  689,    2, 0x02 /* Public */,
     119,    2,  694,    2, 0x02 /* Public */,
     120,    1,  699,    2, 0x02 /* Public */,
     121,    2,  702,    2, 0x02 /* Public */,
     122,    2,  707,    2, 0x02 /* Public */,
     123,    1,  712,    2, 0x02 /* Public */,
     124,    1,  715,    2, 0x02 /* Public */,
     125,    1,  718,    2, 0x02 /* Public */,
     126,    0,  721,    2, 0x02 /* Public */,
     127,    0,  722,    2, 0x02 /* Public */,
     128,    0,  723,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QVariantMap,    5,    6,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   21,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QVariantMap,   29,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   31,   32,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   21,   34,
    QMetaType::Void, QMetaType::QVariantList,   36,
    QMetaType::Void, QMetaType::QVariantList,   38,
    QMetaType::Void, QMetaType::QVariantList,   40,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,   42,   43,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   21,   34,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   21,   34,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString, QMetaType::QString,   21,   47,   34,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   21,   34,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   21,   34,

 // slots: parameters
    QMetaType::Void, QMetaType::QVariantMap,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString, QMetaType::QString, QMetaType::QString,   21,   42,   54,   34,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString, QMetaType::QString,   21,   42,   34,
    QMetaType::Void, 0x80000000 | 57,   58,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 60,    5,   61,
    QMetaType::Void, 0x80000000 | 63,   36,
    QMetaType::Void, QMetaType::Bool, QMetaType::QVariantMap, QMetaType::QString,   21,   65,   34,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   21,    6,
    QMetaType::Void, 0x80000000 | 68,   38,
    QMetaType::Void, 0x80000000 | 70,   40,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   21,   34,
    QMetaType::Void, QMetaType::Bool, QMetaType::Int, QMetaType::QString,   21,   73,   74,

 // methods: parameters
    0x80000000 | 76, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,   42,
    QMetaType::Void, QMetaType::QString,   79,
    QMetaType::Void, QMetaType::QString,   81,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    5,   83,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    5,   85,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    5,   85,
    QMetaType::QString, QMetaType::Bool,   88,
    QMetaType::QString,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    5,   90,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   97,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,  100,
    QMetaType::Void,
    QMetaType::Int,
    QMetaType::Bool,
    QMetaType::Void, QMetaType::QString,  105,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QVariantMap,   65,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,  110,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,   73,  112,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   42,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,  117,  118,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,  117,  118,
    QMetaType::Void, QMetaType::QString,  117,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   47,   97,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   42,   97,
    QMetaType::Void, QMetaType::QString,  117,
    QMetaType::Void, QMetaType::QString,  118,
    QMetaType::Void, QMetaType::QString,   54,
    QMetaType::QString,
    QMetaType::Void,
    QMetaType::Bool,

 // properties: name, type, flags
     129, 0x80000000 | 130, 0x00095409,
     131, QMetaType::QString, 0x00495103,
     132, QMetaType::QString, 0x00495103,
     133, QMetaType::QString, 0x00495103,
     134, QMetaType::QString, 0x00495103,
     135, QMetaType::QString, 0x00495103,
     136, QMetaType::QString, 0x00495103,
     137, QMetaType::QString, 0x00495103,
     138, QMetaType::QString, 0x00495103,
     139, QMetaType::QString, 0x00495103,
     140, QMetaType::QString, 0x00495103,
     141, QMetaType::Bool, 0x00495001,
     142, QMetaType::Bool, 0x00495103,
     143, QMetaType::Bool, 0x00495103,
     144, QMetaType::Bool, 0x00495103,
     145, QMetaType::Bool, 0x00495103,
     146, QMetaType::QString, 0x00495103,
     147, QMetaType::QString, 0x00495103,

 // properties: notify_signal_id
       0,
       1,
       6,
       7,
       8,
       9,
      10,
      11,
      12,
      13,
      14,
       5,
      16,
      17,
      18,
      19,
      20,
      21,

       0        // eod
};

void ChatService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ChatService *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->conversationModelChanged(); break;
        case 1: _t->currentConversationIdChanged(); break;
        case 2: _t->messageReceived((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVariantMap(*)>(_a[2]))); break;
        case 3: _t->conversationUpdated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->messagesRefreshed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->connectedChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->currentUserIdChanged(); break;
        case 7: _t->currentUserNameChanged(); break;
        case 8: _t->currentUserAvatarChanged(); break;
        case 9: _t->currentUserGenderChanged(); break;
        case 10: _t->currentUserRegionChanged(); break;
        case 11: _t->currentUserPhoneChanged(); break;
        case 12: _t->currentUserEmailChanged(); break;
        case 13: _t->currentUserContactChanged(); break;
        case 14: _t->currentUserBioChanged(); break;
        case 15: _t->userProfileSaveResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 16: _t->notificationsEnabledChanged(); break;
        case 17: _t->soundEnabledChanged(); break;
        case 18: _t->autoLoginEnabledChanged(); break;
        case 19: _t->rememberPasswordEnabledChanged(); break;
        case 20: _t->lastUsernameChanged(); break;
        case 21: _t->lastPasswordChanged(); break;
        case 22: _t->friendRequestReceived((*reinterpret_cast< const QVariantMap(*)>(_a[1]))); break;
        case 23: _t->friendshipEstablished((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 24: _t->friendRequestSentResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 25: _t->searchResult((*reinterpret_cast< const QVariantList(*)>(_a[1]))); break;
        case 26: _t->pendingRequestsLoaded((*reinterpret_cast< const QVariantList(*)>(_a[1]))); break;
        case 27: _t->friendListLoaded((*reinterpret_cast< const QVariantList(*)>(_a[1]))); break;
        case 28: _t->isFriendResult((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 29: _t->loginResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 30: _t->registerResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 31: _t->passwordResetCodeGenerated((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 32: _t->passwordResetResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 33: _t->passwordUpdateResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 34: _t->onWebSocketMessageReceived((*reinterpret_cast< const QVariantMap(*)>(_a[1]))); break;
        case 35: _t->onWebSocketConnected(); break;
        case 36: _t->onWebSocketDisconnected(); break;
        case 37: _t->handleLoginResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 38: _t->handleRegisterResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 39: _t->handleConversationsLoaded((*reinterpret_cast< const QVector<Conversation>(*)>(_a[1]))); break;
        case 40: _t->handleMessagesLoaded((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVector<Message>(*)>(_a[2]))); break;
        case 41: _t->handleSearchUserResult((*reinterpret_cast< const QVector<UserDAO::UserSearchResult>(*)>(_a[1]))); break;
        case 42: _t->handleUserProfileResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QVariantMap(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 43: _t->handleUserProfileUpdated((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 44: _t->handleFriendRequestsLoaded((*reinterpret_cast< const QVector<FriendRequestDAO::FriendRequest>(*)>(_a[1]))); break;
        case 45: _t->handleFriendListLoaded((*reinterpret_cast< const QVector<FriendRequestDAO::FriendInfo>(*)>(_a[1]))); break;
        case 46: _t->handleFriendRequestSent((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 47: _t->handleRequestStatusUpdated((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 48: { MessageModel* _r = _t->getMessageModel((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< MessageModel**>(_a[0]) = std::move(_r); }  break;
        case 49: _t->setCurrentUserId((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 50: _t->setCurrentUserName((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 51: _t->setCurrentUserAvatar((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 52: _t->sendMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 53: _t->sendImageMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 54: _t->sendFileMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 55: { QString _r = _t->pickLocalFile((*reinterpret_cast< bool(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 56: { QString _r = _t->pickLocalFile();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 57: _t->recallMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 58: _t->markConversationRead((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 59: _t->setCurrentConversation((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 60: _t->refreshConversations(); break;
        case 61: _t->refreshMessages((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 62: _t->logout(); break;
        case 63: _t->changePassword((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 64: _t->connectToDefaultChatServer(); break;
        case 65: _t->connectToChatServer((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 66: _t->disconnectFromChatServer(); break;
        case 67: { int _r = _t->totalUnreadCount();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 68: { bool _r = _t->hasUnreadMessages();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 69: _t->searchUsers((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 70: _t->loadUserProfile(); break;
        case 71: _t->saveUserProfile((*reinterpret_cast< const QVariantMap(*)>(_a[1]))); break;
        case 72: _t->fetchOfflineMessages(); break;
        case 73: _t->sendFriendRequest((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 74: _t->respondToFriendRequest((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 75: _t->getPendingFriendRequests(); break;
        case 76: _t->getFriendList(); break;
        case 77: _t->checkIsFriend((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 78: _t->validateUserLogin((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 79: _t->registerNewUser((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 80: _t->requestPasswordReset((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 81: _t->resetPassword((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 82: _t->adminSetPassword((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 83: _t->setLastUsername((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 84: _t->setLastPassword((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 85: _t->saveToken((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 86: { QString _r = _t->getToken();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 87: _t->clearToken(); break;
        case 88: { bool _r = _t->isDatabaseConnected();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::conversationModelChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::currentConversationIdChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(const QString & , const QVariantMap & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::messageReceived)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::conversationUpdated)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::messagesRefreshed)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::connectedChanged)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::currentUserIdChanged)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::currentUserNameChanged)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::currentUserAvatarChanged)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::currentUserGenderChanged)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::currentUserRegionChanged)) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::currentUserPhoneChanged)) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::currentUserEmailChanged)) {
                *result = 12;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::currentUserContactChanged)) {
                *result = 13;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::currentUserBioChanged)) {
                *result = 14;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::userProfileSaveResult)) {
                *result = 15;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::notificationsEnabledChanged)) {
                *result = 16;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::soundEnabledChanged)) {
                *result = 17;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::autoLoginEnabledChanged)) {
                *result = 18;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::rememberPasswordEnabledChanged)) {
                *result = 19;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::lastUsernameChanged)) {
                *result = 20;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::lastPasswordChanged)) {
                *result = 21;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(const QVariantMap & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::friendRequestReceived)) {
                *result = 22;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::friendshipEstablished)) {
                *result = 23;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::friendRequestSentResult)) {
                *result = 24;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(const QVariantList & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::searchResult)) {
                *result = 25;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(const QVariantList & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::pendingRequestsLoaded)) {
                *result = 26;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(const QVariantList & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::friendListLoaded)) {
                *result = 27;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(const QString & , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::isFriendResult)) {
                *result = 28;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::loginResult)) {
                *result = 29;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::registerResult)) {
                *result = 30;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(bool , const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::passwordResetCodeGenerated)) {
                *result = 31;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::passwordResetResult)) {
                *result = 32;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::passwordUpdateResult)) {
                *result = 33;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<ChatService *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< ConversationModel**>(_v) = _t->conversationModel(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->currentConversationId(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->currentUserId(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->currentUserName(); break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->currentUserAvatar(); break;
        case 5: *reinterpret_cast< QString*>(_v) = _t->currentUserGender(); break;
        case 6: *reinterpret_cast< QString*>(_v) = _t->currentUserRegion(); break;
        case 7: *reinterpret_cast< QString*>(_v) = _t->currentUserPhone(); break;
        case 8: *reinterpret_cast< QString*>(_v) = _t->currentUserEmail(); break;
        case 9: *reinterpret_cast< QString*>(_v) = _t->currentUserContact(); break;
        case 10: *reinterpret_cast< QString*>(_v) = _t->currentUserBio(); break;
        case 11: *reinterpret_cast< bool*>(_v) = _t->isConnected(); break;
        case 12: *reinterpret_cast< bool*>(_v) = _t->notificationsEnabled(); break;
        case 13: *reinterpret_cast< bool*>(_v) = _t->soundEnabled(); break;
        case 14: *reinterpret_cast< bool*>(_v) = _t->autoLoginEnabled(); break;
        case 15: *reinterpret_cast< bool*>(_v) = _t->rememberPasswordEnabled(); break;
        case 16: *reinterpret_cast< QString*>(_v) = _t->lastUsername(); break;
        case 17: *reinterpret_cast< QString*>(_v) = _t->lastPassword(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<ChatService *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 1: _t->setCurrentConversationId(*reinterpret_cast< QString*>(_v)); break;
        case 2: _t->setCurrentUserId(*reinterpret_cast< QString*>(_v)); break;
        case 3: _t->setCurrentUserName(*reinterpret_cast< QString*>(_v)); break;
        case 4: _t->setCurrentUserAvatar(*reinterpret_cast< QString*>(_v)); break;
        case 5: _t->setCurrentUserGender(*reinterpret_cast< QString*>(_v)); break;
        case 6: _t->setCurrentUserRegion(*reinterpret_cast< QString*>(_v)); break;
        case 7: _t->setCurrentUserPhone(*reinterpret_cast< QString*>(_v)); break;
        case 8: _t->setCurrentUserEmail(*reinterpret_cast< QString*>(_v)); break;
        case 9: _t->setCurrentUserContact(*reinterpret_cast< QString*>(_v)); break;
        case 10: _t->setCurrentUserBio(*reinterpret_cast< QString*>(_v)); break;
        case 12: _t->setNotificationsEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 13: _t->setSoundEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 14: _t->setAutoLoginEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 15: _t->setRememberPasswordEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 16: _t->setLastUsername(*reinterpret_cast< QString*>(_v)); break;
        case 17: _t->setLastPassword(*reinterpret_cast< QString*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject ChatService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ChatService.data,
    qt_meta_data_ChatService,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ChatService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ChatService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ChatService.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ChatService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 89)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 89;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 89)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 89;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 18;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 18;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 18;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 18;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 18;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void ChatService::conversationModelChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ChatService::currentConversationIdChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ChatService::messageReceived(const QString & _t1, const QVariantMap & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ChatService::conversationUpdated(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ChatService::messagesRefreshed(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ChatService::connectedChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ChatService::currentUserIdChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void ChatService::currentUserNameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void ChatService::currentUserAvatarChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void ChatService::currentUserGenderChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void ChatService::currentUserRegionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void ChatService::currentUserPhoneChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void ChatService::currentUserEmailChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}

// SIGNAL 13
void ChatService::currentUserContactChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 13, nullptr);
}

// SIGNAL 14
void ChatService::currentUserBioChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 14, nullptr);
}

// SIGNAL 15
void ChatService::userProfileSaveResult(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void ChatService::notificationsEnabledChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 16, nullptr);
}

// SIGNAL 17
void ChatService::soundEnabledChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 17, nullptr);
}

// SIGNAL 18
void ChatService::autoLoginEnabledChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 18, nullptr);
}

// SIGNAL 19
void ChatService::rememberPasswordEnabledChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 19, nullptr);
}

// SIGNAL 20
void ChatService::lastUsernameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 20, nullptr);
}

// SIGNAL 21
void ChatService::lastPasswordChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 21, nullptr);
}

// SIGNAL 22
void ChatService::friendRequestReceived(const QVariantMap & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 22, _a);
}

// SIGNAL 23
void ChatService::friendshipEstablished(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 23, _a);
}

// SIGNAL 24
void ChatService::friendRequestSentResult(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 24, _a);
}

// SIGNAL 25
void ChatService::searchResult(const QVariantList & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 25, _a);
}

// SIGNAL 26
void ChatService::pendingRequestsLoaded(const QVariantList & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 26, _a);
}

// SIGNAL 27
void ChatService::friendListLoaded(const QVariantList & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 27, _a);
}

// SIGNAL 28
void ChatService::isFriendResult(const QString & _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 28, _a);
}

// SIGNAL 29
void ChatService::loginResult(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 29, _a);
}

// SIGNAL 30
void ChatService::registerResult(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 30, _a);
}

// SIGNAL 31
void ChatService::passwordResetCodeGenerated(bool _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 31, _a);
}

// SIGNAL 32
void ChatService::passwordResetResult(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 32, _a);
}

// SIGNAL 33
void ChatService::passwordUpdateResult(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 33, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
