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
    QByteArrayData data[135];
    char stringdata0[2378];
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
QT_MOC_LITERAL(8, 126, 16), // "connectedChanged"
QT_MOC_LITERAL(9, 143, 9), // "connected"
QT_MOC_LITERAL(10, 153, 20), // "currentUserIdChanged"
QT_MOC_LITERAL(11, 174, 22), // "currentUserNameChanged"
QT_MOC_LITERAL(12, 197, 24), // "currentUserAvatarChanged"
QT_MOC_LITERAL(13, 222, 24), // "currentUserGenderChanged"
QT_MOC_LITERAL(14, 247, 24), // "currentUserRegionChanged"
QT_MOC_LITERAL(15, 272, 23), // "currentUserPhoneChanged"
QT_MOC_LITERAL(16, 296, 23), // "currentUserEmailChanged"
QT_MOC_LITERAL(17, 320, 25), // "currentUserContactChanged"
QT_MOC_LITERAL(18, 346, 21), // "currentUserBioChanged"
QT_MOC_LITERAL(19, 368, 21), // "userProfileSaveResult"
QT_MOC_LITERAL(20, 390, 7), // "success"
QT_MOC_LITERAL(21, 398, 27), // "notificationsEnabledChanged"
QT_MOC_LITERAL(22, 426, 19), // "soundEnabledChanged"
QT_MOC_LITERAL(23, 446, 23), // "autoLoginEnabledChanged"
QT_MOC_LITERAL(24, 470, 30), // "rememberPasswordEnabledChanged"
QT_MOC_LITERAL(25, 501, 19), // "lastUsernameChanged"
QT_MOC_LITERAL(26, 521, 19), // "lastPasswordChanged"
QT_MOC_LITERAL(27, 541, 21), // "friendRequestReceived"
QT_MOC_LITERAL(28, 563, 7), // "request"
QT_MOC_LITERAL(29, 571, 21), // "friendshipEstablished"
QT_MOC_LITERAL(30, 593, 8), // "friendId"
QT_MOC_LITERAL(31, 602, 10), // "friendName"
QT_MOC_LITERAL(32, 613, 23), // "friendRequestSentResult"
QT_MOC_LITERAL(33, 637, 5), // "error"
QT_MOC_LITERAL(34, 643, 12), // "searchResult"
QT_MOC_LITERAL(35, 656, 7), // "results"
QT_MOC_LITERAL(36, 664, 21), // "pendingRequestsLoaded"
QT_MOC_LITERAL(37, 686, 8), // "requests"
QT_MOC_LITERAL(38, 695, 16), // "friendListLoaded"
QT_MOC_LITERAL(39, 712, 7), // "friends"
QT_MOC_LITERAL(40, 720, 14), // "isFriendResult"
QT_MOC_LITERAL(41, 735, 6), // "userId"
QT_MOC_LITERAL(42, 742, 8), // "isFriend"
QT_MOC_LITERAL(43, 751, 11), // "loginResult"
QT_MOC_LITERAL(44, 763, 14), // "registerResult"
QT_MOC_LITERAL(45, 778, 26), // "passwordResetCodeGenerated"
QT_MOC_LITERAL(46, 805, 9), // "resetCode"
QT_MOC_LITERAL(47, 815, 19), // "passwordResetResult"
QT_MOC_LITERAL(48, 835, 20), // "passwordUpdateResult"
QT_MOC_LITERAL(49, 856, 26), // "onWebSocketMessageReceived"
QT_MOC_LITERAL(50, 883, 20), // "onWebSocketConnected"
QT_MOC_LITERAL(51, 904, 23), // "onWebSocketDisconnected"
QT_MOC_LITERAL(52, 928, 17), // "handleLoginResult"
QT_MOC_LITERAL(53, 946, 5), // "token"
QT_MOC_LITERAL(54, 952, 20), // "handleRegisterResult"
QT_MOC_LITERAL(55, 973, 25), // "handleConversationsLoaded"
QT_MOC_LITERAL(56, 999, 21), // "QVector<Conversation>"
QT_MOC_LITERAL(57, 1021, 13), // "conversations"
QT_MOC_LITERAL(58, 1035, 20), // "handleMessagesLoaded"
QT_MOC_LITERAL(59, 1056, 16), // "QVector<Message>"
QT_MOC_LITERAL(60, 1073, 8), // "messages"
QT_MOC_LITERAL(61, 1082, 22), // "handleSearchUserResult"
QT_MOC_LITERAL(62, 1105, 34), // "QVector<UserDAO::UserSearchRe..."
QT_MOC_LITERAL(63, 1140, 23), // "handleUserProfileResult"
QT_MOC_LITERAL(64, 1164, 7), // "profile"
QT_MOC_LITERAL(65, 1172, 24), // "handleUserProfileUpdated"
QT_MOC_LITERAL(66, 1197, 26), // "handleFriendRequestsLoaded"
QT_MOC_LITERAL(67, 1224, 40), // "QVector<FriendRequestDAO::Fri..."
QT_MOC_LITERAL(68, 1265, 22), // "handleFriendListLoaded"
QT_MOC_LITERAL(69, 1288, 37), // "QVector<FriendRequestDAO::Fri..."
QT_MOC_LITERAL(70, 1326, 23), // "handleFriendRequestSent"
QT_MOC_LITERAL(71, 1350, 26), // "handleRequestStatusUpdated"
QT_MOC_LITERAL(72, 1377, 9), // "requestId"
QT_MOC_LITERAL(73, 1387, 6), // "status"
QT_MOC_LITERAL(74, 1394, 15), // "getMessageModel"
QT_MOC_LITERAL(75, 1410, 13), // "MessageModel*"
QT_MOC_LITERAL(76, 1424, 16), // "setCurrentUserId"
QT_MOC_LITERAL(77, 1441, 18), // "setCurrentUserName"
QT_MOC_LITERAL(78, 1460, 8), // "userName"
QT_MOC_LITERAL(79, 1469, 20), // "setCurrentUserAvatar"
QT_MOC_LITERAL(80, 1490, 6), // "avatar"
QT_MOC_LITERAL(81, 1497, 11), // "sendMessage"
QT_MOC_LITERAL(82, 1509, 7), // "content"
QT_MOC_LITERAL(83, 1517, 20), // "markConversationRead"
QT_MOC_LITERAL(84, 1538, 22), // "setCurrentConversation"
QT_MOC_LITERAL(85, 1561, 20), // "refreshConversations"
QT_MOC_LITERAL(86, 1582, 6), // "logout"
QT_MOC_LITERAL(87, 1589, 14), // "changePassword"
QT_MOC_LITERAL(88, 1604, 11), // "newPassword"
QT_MOC_LITERAL(89, 1616, 26), // "connectToDefaultChatServer"
QT_MOC_LITERAL(90, 1643, 19), // "connectToChatServer"
QT_MOC_LITERAL(91, 1663, 9), // "serverUrl"
QT_MOC_LITERAL(92, 1673, 24), // "disconnectFromChatServer"
QT_MOC_LITERAL(93, 1698, 16), // "totalUnreadCount"
QT_MOC_LITERAL(94, 1715, 17), // "hasUnreadMessages"
QT_MOC_LITERAL(95, 1733, 11), // "searchUsers"
QT_MOC_LITERAL(96, 1745, 10), // "searchTerm"
QT_MOC_LITERAL(97, 1756, 15), // "loadUserProfile"
QT_MOC_LITERAL(98, 1772, 15), // "saveUserProfile"
QT_MOC_LITERAL(99, 1788, 17), // "sendFriendRequest"
QT_MOC_LITERAL(100, 1806, 8), // "toUserId"
QT_MOC_LITERAL(101, 1815, 22), // "respondToFriendRequest"
QT_MOC_LITERAL(102, 1838, 6), // "accept"
QT_MOC_LITERAL(103, 1845, 24), // "getPendingFriendRequests"
QT_MOC_LITERAL(104, 1870, 13), // "getFriendList"
QT_MOC_LITERAL(105, 1884, 13), // "checkIsFriend"
QT_MOC_LITERAL(106, 1898, 17), // "validateUserLogin"
QT_MOC_LITERAL(107, 1916, 8), // "username"
QT_MOC_LITERAL(108, 1925, 8), // "password"
QT_MOC_LITERAL(109, 1934, 15), // "registerNewUser"
QT_MOC_LITERAL(110, 1950, 20), // "requestPasswordReset"
QT_MOC_LITERAL(111, 1971, 13), // "resetPassword"
QT_MOC_LITERAL(112, 1985, 16), // "adminSetPassword"
QT_MOC_LITERAL(113, 2002, 15), // "setLastUsername"
QT_MOC_LITERAL(114, 2018, 15), // "setLastPassword"
QT_MOC_LITERAL(115, 2034, 19), // "isDatabaseConnected"
QT_MOC_LITERAL(116, 2054, 17), // "conversationModel"
QT_MOC_LITERAL(117, 2072, 18), // "ConversationModel*"
QT_MOC_LITERAL(118, 2091, 21), // "currentConversationId"
QT_MOC_LITERAL(119, 2113, 13), // "currentUserId"
QT_MOC_LITERAL(120, 2127, 15), // "currentUserName"
QT_MOC_LITERAL(121, 2143, 17), // "currentUserAvatar"
QT_MOC_LITERAL(122, 2161, 17), // "currentUserGender"
QT_MOC_LITERAL(123, 2179, 17), // "currentUserRegion"
QT_MOC_LITERAL(124, 2197, 16), // "currentUserPhone"
QT_MOC_LITERAL(125, 2214, 16), // "currentUserEmail"
QT_MOC_LITERAL(126, 2231, 18), // "currentUserContact"
QT_MOC_LITERAL(127, 2250, 14), // "currentUserBio"
QT_MOC_LITERAL(128, 2265, 11), // "isConnected"
QT_MOC_LITERAL(129, 2277, 20), // "notificationsEnabled"
QT_MOC_LITERAL(130, 2298, 12), // "soundEnabled"
QT_MOC_LITERAL(131, 2311, 16), // "autoLoginEnabled"
QT_MOC_LITERAL(132, 2328, 23), // "rememberPasswordEnabled"
QT_MOC_LITERAL(133, 2352, 12), // "lastUsername"
QT_MOC_LITERAL(134, 2365, 12) // "lastPassword"

    },
    "ChatService\0conversationModelChanged\0"
    "\0currentConversationIdChanged\0"
    "messageReceived\0conversationId\0message\0"
    "conversationUpdated\0connectedChanged\0"
    "connected\0currentUserIdChanged\0"
    "currentUserNameChanged\0currentUserAvatarChanged\0"
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
    "sendMessage\0content\0markConversationRead\0"
    "setCurrentConversation\0refreshConversations\0"
    "logout\0changePassword\0newPassword\0"
    "connectToDefaultChatServer\0"
    "connectToChatServer\0serverUrl\0"
    "disconnectFromChatServer\0totalUnreadCount\0"
    "hasUnreadMessages\0searchUsers\0searchTerm\0"
    "loadUserProfile\0saveUserProfile\0"
    "sendFriendRequest\0toUserId\0"
    "respondToFriendRequest\0accept\0"
    "getPendingFriendRequests\0getFriendList\0"
    "checkIsFriend\0validateUserLogin\0"
    "username\0password\0registerNewUser\0"
    "requestPasswordReset\0resetPassword\0"
    "adminSetPassword\0setLastUsername\0"
    "setLastPassword\0isDatabaseConnected\0"
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
      78,   14, // methods
      18,  638, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      33,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  404,    2, 0x06 /* Public */,
       3,    0,  405,    2, 0x06 /* Public */,
       4,    2,  406,    2, 0x06 /* Public */,
       7,    1,  411,    2, 0x06 /* Public */,
       8,    1,  414,    2, 0x06 /* Public */,
      10,    0,  417,    2, 0x06 /* Public */,
      11,    0,  418,    2, 0x06 /* Public */,
      12,    0,  419,    2, 0x06 /* Public */,
      13,    0,  420,    2, 0x06 /* Public */,
      14,    0,  421,    2, 0x06 /* Public */,
      15,    0,  422,    2, 0x06 /* Public */,
      16,    0,  423,    2, 0x06 /* Public */,
      17,    0,  424,    2, 0x06 /* Public */,
      18,    0,  425,    2, 0x06 /* Public */,
      19,    2,  426,    2, 0x06 /* Public */,
      21,    0,  431,    2, 0x06 /* Public */,
      22,    0,  432,    2, 0x06 /* Public */,
      23,    0,  433,    2, 0x06 /* Public */,
      24,    0,  434,    2, 0x06 /* Public */,
      25,    0,  435,    2, 0x06 /* Public */,
      26,    0,  436,    2, 0x06 /* Public */,
      27,    1,  437,    2, 0x06 /* Public */,
      29,    2,  440,    2, 0x06 /* Public */,
      32,    2,  445,    2, 0x06 /* Public */,
      34,    1,  450,    2, 0x06 /* Public */,
      36,    1,  453,    2, 0x06 /* Public */,
      38,    1,  456,    2, 0x06 /* Public */,
      40,    2,  459,    2, 0x06 /* Public */,
      43,    2,  464,    2, 0x06 /* Public */,
      44,    2,  469,    2, 0x06 /* Public */,
      45,    3,  474,    2, 0x06 /* Public */,
      47,    2,  481,    2, 0x06 /* Public */,
      48,    2,  486,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      49,    1,  491,    2, 0x08 /* Private */,
      50,    0,  494,    2, 0x08 /* Private */,
      51,    0,  495,    2, 0x08 /* Private */,
      52,    4,  496,    2, 0x08 /* Private */,
      54,    3,  505,    2, 0x08 /* Private */,
      55,    1,  512,    2, 0x08 /* Private */,
      58,    2,  515,    2, 0x08 /* Private */,
      61,    1,  520,    2, 0x08 /* Private */,
      63,    3,  523,    2, 0x08 /* Private */,
      65,    2,  530,    2, 0x08 /* Private */,
      66,    1,  535,    2, 0x08 /* Private */,
      68,    1,  538,    2, 0x08 /* Private */,
      70,    2,  541,    2, 0x08 /* Private */,
      71,    3,  546,    2, 0x08 /* Private */,

 // methods: name, argc, parameters, tag, flags
      74,    1,  553,    2, 0x02 /* Public */,
      76,    1,  556,    2, 0x02 /* Public */,
      77,    1,  559,    2, 0x02 /* Public */,
      79,    1,  562,    2, 0x02 /* Public */,
      81,    2,  565,    2, 0x02 /* Public */,
      83,    1,  570,    2, 0x02 /* Public */,
      84,    1,  573,    2, 0x02 /* Public */,
      85,    0,  576,    2, 0x02 /* Public */,
      86,    0,  577,    2, 0x02 /* Public */,
      87,    1,  578,    2, 0x02 /* Public */,
      89,    0,  581,    2, 0x02 /* Public */,
      90,    1,  582,    2, 0x02 /* Public */,
      92,    0,  585,    2, 0x02 /* Public */,
      93,    0,  586,    2, 0x02 /* Public */,
      94,    0,  587,    2, 0x02 /* Public */,
      95,    1,  588,    2, 0x02 /* Public */,
      97,    0,  591,    2, 0x02 /* Public */,
      98,    1,  592,    2, 0x02 /* Public */,
      99,    1,  595,    2, 0x02 /* Public */,
     101,    2,  598,    2, 0x02 /* Public */,
     103,    0,  603,    2, 0x02 /* Public */,
     104,    0,  604,    2, 0x02 /* Public */,
     105,    1,  605,    2, 0x02 /* Public */,
     106,    2,  608,    2, 0x02 /* Public */,
     109,    2,  613,    2, 0x02 /* Public */,
     110,    1,  618,    2, 0x02 /* Public */,
     111,    2,  621,    2, 0x02 /* Public */,
     112,    2,  626,    2, 0x02 /* Public */,
     113,    1,  631,    2, 0x02 /* Public */,
     114,    1,  634,    2, 0x02 /* Public */,
     115,    0,  637,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QVariantMap,    5,    6,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::Bool,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   20,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QVariantMap,   28,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   30,   31,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   20,   33,
    QMetaType::Void, QMetaType::QVariantList,   35,
    QMetaType::Void, QMetaType::QVariantList,   37,
    QMetaType::Void, QMetaType::QVariantList,   39,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,   41,   42,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   20,   33,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   20,   33,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString, QMetaType::QString,   20,   46,   33,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   20,   33,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   20,   33,

 // slots: parameters
    QMetaType::Void, QMetaType::QVariantMap,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString, QMetaType::QString, QMetaType::QString,   20,   41,   53,   33,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString, QMetaType::QString,   20,   41,   33,
    QMetaType::Void, 0x80000000 | 56,   57,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 59,    5,   60,
    QMetaType::Void, 0x80000000 | 62,   35,
    QMetaType::Void, QMetaType::Bool, QMetaType::QVariantMap, QMetaType::QString,   20,   64,   33,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   20,    6,
    QMetaType::Void, 0x80000000 | 67,   37,
    QMetaType::Void, 0x80000000 | 69,   39,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   20,   33,
    QMetaType::Void, QMetaType::Bool, QMetaType::Int, QMetaType::QString,   20,   72,   73,

 // methods: parameters
    0x80000000 | 75, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,   41,
    QMetaType::Void, QMetaType::QString,   78,
    QMetaType::Void, QMetaType::QString,   80,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    5,   82,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   88,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   91,
    QMetaType::Void,
    QMetaType::Int,
    QMetaType::Bool,
    QMetaType::Void, QMetaType::QString,   96,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QVariantMap,   64,
    QMetaType::Void, QMetaType::QString,  100,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,   72,  102,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   41,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,  107,  108,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,  107,  108,
    QMetaType::Void, QMetaType::QString,  107,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   46,   88,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   41,   88,
    QMetaType::Void, QMetaType::QString,  107,
    QMetaType::Void, QMetaType::QString,  108,
    QMetaType::Bool,

 // properties: name, type, flags
     116, 0x80000000 | 117, 0x00095409,
     118, QMetaType::QString, 0x00495103,
     119, QMetaType::QString, 0x00495103,
     120, QMetaType::QString, 0x00495103,
     121, QMetaType::QString, 0x00495103,
     122, QMetaType::QString, 0x00495103,
     123, QMetaType::QString, 0x00495103,
     124, QMetaType::QString, 0x00495103,
     125, QMetaType::QString, 0x00495103,
     126, QMetaType::QString, 0x00495103,
     127, QMetaType::QString, 0x00495103,
     128, QMetaType::Bool, 0x00495001,
     129, QMetaType::Bool, 0x00495103,
     130, QMetaType::Bool, 0x00495103,
     131, QMetaType::Bool, 0x00495103,
     132, QMetaType::Bool, 0x00495103,
     133, QMetaType::QString, 0x00495103,
     134, QMetaType::QString, 0x00495103,

 // properties: notify_signal_id
       0,
       1,
       5,
       6,
       7,
       8,
       9,
      10,
      11,
      12,
      13,
       4,
      15,
      16,
      17,
      18,
      19,
      20,

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
        case 4: _t->connectedChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->currentUserIdChanged(); break;
        case 6: _t->currentUserNameChanged(); break;
        case 7: _t->currentUserAvatarChanged(); break;
        case 8: _t->currentUserGenderChanged(); break;
        case 9: _t->currentUserRegionChanged(); break;
        case 10: _t->currentUserPhoneChanged(); break;
        case 11: _t->currentUserEmailChanged(); break;
        case 12: _t->currentUserContactChanged(); break;
        case 13: _t->currentUserBioChanged(); break;
        case 14: _t->userProfileSaveResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 15: _t->notificationsEnabledChanged(); break;
        case 16: _t->soundEnabledChanged(); break;
        case 17: _t->autoLoginEnabledChanged(); break;
        case 18: _t->rememberPasswordEnabledChanged(); break;
        case 19: _t->lastUsernameChanged(); break;
        case 20: _t->lastPasswordChanged(); break;
        case 21: _t->friendRequestReceived((*reinterpret_cast< const QVariantMap(*)>(_a[1]))); break;
        case 22: _t->friendshipEstablished((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 23: _t->friendRequestSentResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 24: _t->searchResult((*reinterpret_cast< const QVariantList(*)>(_a[1]))); break;
        case 25: _t->pendingRequestsLoaded((*reinterpret_cast< const QVariantList(*)>(_a[1]))); break;
        case 26: _t->friendListLoaded((*reinterpret_cast< const QVariantList(*)>(_a[1]))); break;
        case 27: _t->isFriendResult((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 28: _t->loginResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 29: _t->registerResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 30: _t->passwordResetCodeGenerated((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 31: _t->passwordResetResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 32: _t->passwordUpdateResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 33: _t->onWebSocketMessageReceived((*reinterpret_cast< const QVariantMap(*)>(_a[1]))); break;
        case 34: _t->onWebSocketConnected(); break;
        case 35: _t->onWebSocketDisconnected(); break;
        case 36: _t->handleLoginResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 37: _t->handleRegisterResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 38: _t->handleConversationsLoaded((*reinterpret_cast< const QVector<Conversation>(*)>(_a[1]))); break;
        case 39: _t->handleMessagesLoaded((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVector<Message>(*)>(_a[2]))); break;
        case 40: _t->handleSearchUserResult((*reinterpret_cast< const QVector<UserDAO::UserSearchResult>(*)>(_a[1]))); break;
        case 41: _t->handleUserProfileResult((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QVariantMap(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 42: _t->handleUserProfileUpdated((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 43: _t->handleFriendRequestsLoaded((*reinterpret_cast< const QVector<FriendRequestDAO::FriendRequest>(*)>(_a[1]))); break;
        case 44: _t->handleFriendListLoaded((*reinterpret_cast< const QVector<FriendRequestDAO::FriendInfo>(*)>(_a[1]))); break;
        case 45: _t->handleFriendRequestSent((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 46: _t->handleRequestStatusUpdated((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 47: { MessageModel* _r = _t->getMessageModel((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< MessageModel**>(_a[0]) = std::move(_r); }  break;
        case 48: _t->setCurrentUserId((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 49: _t->setCurrentUserName((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 50: _t->setCurrentUserAvatar((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 51: _t->sendMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 52: _t->markConversationRead((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 53: _t->setCurrentConversation((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 54: _t->refreshConversations(); break;
        case 55: _t->logout(); break;
        case 56: _t->changePassword((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 57: _t->connectToDefaultChatServer(); break;
        case 58: _t->connectToChatServer((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 59: _t->disconnectFromChatServer(); break;
        case 60: { int _r = _t->totalUnreadCount();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 61: { bool _r = _t->hasUnreadMessages();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 62: _t->searchUsers((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 63: _t->loadUserProfile(); break;
        case 64: _t->saveUserProfile((*reinterpret_cast< const QVariantMap(*)>(_a[1]))); break;
        case 65: _t->sendFriendRequest((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 66: _t->respondToFriendRequest((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 67: _t->getPendingFriendRequests(); break;
        case 68: _t->getFriendList(); break;
        case 69: _t->checkIsFriend((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 70: _t->validateUserLogin((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 71: _t->registerNewUser((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 72: _t->requestPasswordReset((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 73: _t->resetPassword((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 74: _t->adminSetPassword((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 75: _t->setLastUsername((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 76: _t->setLastPassword((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 77: { bool _r = _t->isDatabaseConnected();
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
            using _t = void (ChatService::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::connectedChanged)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::currentUserIdChanged)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::currentUserNameChanged)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::currentUserAvatarChanged)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::currentUserGenderChanged)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::currentUserRegionChanged)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::currentUserPhoneChanged)) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::currentUserEmailChanged)) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::currentUserContactChanged)) {
                *result = 12;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::currentUserBioChanged)) {
                *result = 13;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::userProfileSaveResult)) {
                *result = 14;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::notificationsEnabledChanged)) {
                *result = 15;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::soundEnabledChanged)) {
                *result = 16;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::autoLoginEnabledChanged)) {
                *result = 17;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::rememberPasswordEnabledChanged)) {
                *result = 18;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::lastUsernameChanged)) {
                *result = 19;
                return;
            }
        }
        {
            using _t = void (ChatService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::lastPasswordChanged)) {
                *result = 20;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(const QVariantMap & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::friendRequestReceived)) {
                *result = 21;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::friendshipEstablished)) {
                *result = 22;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::friendRequestSentResult)) {
                *result = 23;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(const QVariantList & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::searchResult)) {
                *result = 24;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(const QVariantList & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::pendingRequestsLoaded)) {
                *result = 25;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(const QVariantList & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::friendListLoaded)) {
                *result = 26;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(const QString & , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::isFriendResult)) {
                *result = 27;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::loginResult)) {
                *result = 28;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::registerResult)) {
                *result = 29;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(bool , const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::passwordResetCodeGenerated)) {
                *result = 30;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::passwordResetResult)) {
                *result = 31;
                return;
            }
        }
        {
            using _t = void (ChatService::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ChatService::passwordUpdateResult)) {
                *result = 32;
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
        if (_id < 78)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 78;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 78)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 78;
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
void ChatService::connectedChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ChatService::currentUserIdChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void ChatService::currentUserNameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void ChatService::currentUserAvatarChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void ChatService::currentUserGenderChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void ChatService::currentUserRegionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void ChatService::currentUserPhoneChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void ChatService::currentUserEmailChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void ChatService::currentUserContactChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}

// SIGNAL 13
void ChatService::currentUserBioChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 13, nullptr);
}

// SIGNAL 14
void ChatService::userProfileSaveResult(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void ChatService::notificationsEnabledChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 15, nullptr);
}

// SIGNAL 16
void ChatService::soundEnabledChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 16, nullptr);
}

// SIGNAL 17
void ChatService::autoLoginEnabledChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 17, nullptr);
}

// SIGNAL 18
void ChatService::rememberPasswordEnabledChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 18, nullptr);
}

// SIGNAL 19
void ChatService::lastUsernameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 19, nullptr);
}

// SIGNAL 20
void ChatService::lastPasswordChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 20, nullptr);
}

// SIGNAL 21
void ChatService::friendRequestReceived(const QVariantMap & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 21, _a);
}

// SIGNAL 22
void ChatService::friendshipEstablished(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 22, _a);
}

// SIGNAL 23
void ChatService::friendRequestSentResult(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 23, _a);
}

// SIGNAL 24
void ChatService::searchResult(const QVariantList & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 24, _a);
}

// SIGNAL 25
void ChatService::pendingRequestsLoaded(const QVariantList & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 25, _a);
}

// SIGNAL 26
void ChatService::friendListLoaded(const QVariantList & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 26, _a);
}

// SIGNAL 27
void ChatService::isFriendResult(const QString & _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 27, _a);
}

// SIGNAL 28
void ChatService::loginResult(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 28, _a);
}

// SIGNAL 29
void ChatService::registerResult(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 29, _a);
}

// SIGNAL 30
void ChatService::passwordResetCodeGenerated(bool _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 30, _a);
}

// SIGNAL 31
void ChatService::passwordResetResult(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 31, _a);
}

// SIGNAL 32
void ChatService::passwordUpdateResult(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 32, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
