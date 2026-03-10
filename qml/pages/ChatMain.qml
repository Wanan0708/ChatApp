import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import ChatApp.Models 1.0
import ChatApp.Network 1.0
import "../components"

Item {
    id: roootItem

    // 当前页面索引：0=消息, 1=联系人, 2=通知, 3=设置
    property int currentPageIndex: 0
    property bool isLoading: false
    property string loadingText: "加载中..."

    signal addContactRequested()
    function addContact(user) {
        if (contactsPage) {
            contactsPage.addContact(user)
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 1

        // 左侧工具栏
        SideBar {
            id: sideBar
            Layout.preferredWidth: width
            Layout.fillHeight: true
            // 绑定当前页面索引（确保侧边栏高亮与主页面一致）
            currentPageIndex: roootItem.currentPageIndex
            // 工具栏按钮点击事件——更新根项的索引以触发页面切换
            onMessagesClicked: {
                roootItem.currentPageIndex = 0
            }
            onContactsClicked: {
                roootItem.currentPageIndex = 1
            }
            onMailClicked: {
                roootItem.currentPageIndex = 2
            }
            onMoreClicked: {
                roootItem.currentPageIndex = 3
            }
        }

        // 中间面板 - 使用 StackLayout 实现页面切换
        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: currentPageIndex

            // 第 0 页：消息 (原 ChatMain 布局)
            Item {
                RowLayout {
                    anchors.fill: parent
                    spacing: 0

                    ConversationsPage {
                        Layout.preferredWidth: parent.width * 0.30
                        Layout.maximumWidth: parent.width * 0.30
                        Layout.fillHeight: true
                        onConversationSelected: {
                            chatPage.currentConversationId = conversationId
                            chatPage.currentTitle = title
                            chatPage.currentAvatar = avatar
                            console.log("main.qml: Received conversation selected - ID:", conversationId, "Title:", title)
                        }
                    }

                    // 会话列表与聊天区域的分割线
                    Rectangle {
                        Layout.preferredWidth: 1
                        Layout.fillHeight: true
                        color: "#e0e0e0"
                    }

                    ChatPage {
                        id: chatPage
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        currentConversationId: ""
                        currentTitle: ""
                    }
                }
            }

            // 第 1 页：联系人
            ContactsPage {
                id: contactsPage
                onContactSelected: {
                    // contactId 是用户 ID（如 user_xxx）
                    var userId = contactId;
                    var userName = contactName;

                    // 查找是否已存在与该用户的会话，如果没有则返回用户 ID（后端会创建新会话）
                    var conversationId = ChatService.conversationModel.findOrStartConversation(userId, userName);

                    // 确保在会话列表中存在该联系人的项
                    ChatService.conversationModel.updateConversation(conversationId, {"title": userName, "avatar": contactAvatar, "type": "single"})

                    // 设置为当前会话
                    ChatService.setCurrentConversation(conversationId)

                    // 同步聊天页面属性
                    chatPage.currentConversationId = conversationId
                    chatPage.currentTitle = userName
                    chatPage.currentAvatar = contactAvatar

                    // 切换到消息页面并更新侧边栏状态
                    roootItem.currentPageIndex = 0
                }
            }

            // 第 2 页：通知
            NotificationsPage {
                id: notificationsPage
            }

            // 第 3 页：设置
            SettingsPage {
                id: settingsPage
            }
        }
    }

    Connections {
        target: ws
        // ChatService 暴露的信号为 connectedChanged(bool) 和 messageReceived(conversationId, message)
        function onConnectedChanged(connected) {
            console.log("WebSocket状态:", connected ? "✓ 已连接" : "✗ 已断开")
        }
    }

    // 连接联系人页面的信号
    Connections {
        target: contactsPage
        function onAddContactRequested() {
            roootItem.addContactRequested()
        }
    }

    // 全局加载遮罩
    LoadingOverlay {
        id: globalLoadingOverlay
        loading: roootItem.isLoading
        loadingText: roootItem.loadingText
    }
}
