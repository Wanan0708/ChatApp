import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import ChatApp.Network 1.0

Rectangle {
    id: notificationsPage
    color: "white"

    // [ { id, type: "system"|"friend_request"|"message", title, description, time, icon, read, refId } ]
    property var notifications: []
    
    // 当前选中的过滤器分类: "全部", "系统", "好友申请"
    property string currentFilter: "全部"
    
    property var filteredNotifications: {
        var result = []
        for (var i = 0; i < notifications.length; i++) {
            var notif = notifications[i]
            if (currentFilter === "全部") {
                result.push(notif)
            } else if (currentFilter === "系统" && notif.type === "system") {
                result.push(notif)
            } else if (currentFilter === "好友申请" && notif.type === "friend_request") {
                result.push(notif)
            }
        }
        return result
    }

    Component.onCompleted: {
        refreshData()
    }

    function refreshData() {
        ChatService.getPendingFriendRequests()
    }

    Connections {
        target: ChatService
        onPendingRequestsLoaded: {
            var newList = []
            
            // 将假System通知为了演示保留一条
            newList.push({
                id: "sys_1",
                type: "system",
                title: "系统通知",
                description: "欢迎使用 ChatApp!",
                time: "刚刚",
                icon: "🔔",
                read: true,
                refId: ""
            })

            // 这里把真实的好友请求转换成通知
            for (var i = 0; i < requests.length; i++) {
                var req = requests[i]
                if (req.status === "pending") {
                    newList.push({
                        id: req.requestId,
                        type: "friend_request",
                        title: "好友申请",
                        description: req.fromUsername + " 请求添加你为好友",
                        time: "",
                        icon: "👥",
                        read: false,
                        refId: req.requestId // 用于快速操作
                    })
                }
            }
            notifications = newList
        }
        
        onFriendRequestReceived: {
            refreshData()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        // 标题与过滤栏
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 12

            RowLayout {
                Layout.fillWidth: true
                spacing: 12

                Text {
                    text: "消息和通知"
                    font.pixelSize: 18
                    font.weight: Font.Bold
                    color: "#333"
                }

                Item { Layout.fillWidth: true }

                // 全部标记为已读按钮
                Button {
                    Layout.preferredWidth: 120
                    Layout.preferredHeight: 32
                    text: "全部标记为已读"
                    font.pixelSize: 12

                    background: Rectangle {
                        color: "#f5f5f5"
                        border.color: "#ddd"
                        radius: 4
                    }

                    contentItem: Text {
                        text: parent.text
                        color: "#666"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font: parent.font
                    }
                    
                    onClicked: {
                        var newList = []
                        for (var i = 0; i < notifications.length; i++) {
                            var notif = notifications[i]
                            notif.read = true
                            newList.push(notif)
                        }
                        notifications = newList
                    }
                }
            }

            // Segment Tabs (过滤器)
            RowLayout {
                spacing: 8
                Repeater {
                    model: ["全部", "好友申请", "系统"]
                    delegate: Rectangle {
                        width: 80
                        height: 30
                        radius: 15
                        color: currentFilter === modelData ? "#3B82F6" : "#f1f5f9"
                        
                        Text {
                            anchors.centerIn: parent
                            text: modelData
                            font.pixelSize: 13
                            color: currentFilter === modelData ? "white" : "#64748b"
                        }

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: currentFilter = modelData
                        }
                    }
                }
            }
        }

        // 通知列表
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            contentWidth: width

            ListView {
                id: notificationsList
                width: parent.width
                model: filteredNotifications.length
                spacing: 8

                delegate: Rectangle {
                    width: notificationsList.width
                    height: 80
                    color: notification.read ? "white" : "#f0f8ff"
                    radius: 4
                    border.color: "#eee"

                    property var notification: filteredNotifications[index]

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 12

                        // 图标
                        Rectangle {
                            Layout.preferredWidth: 44
                            Layout.preferredHeight: 44
                            radius: 22
                            color: notification.type === "message" ? "#e8f1ff" : "#fff3e0"

                            Text {
                                anchors.centerIn: parent
                                text: notification.icon
                                font.pixelSize: 20
                            }
                        }

                        // 通知内容
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 4

                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 8

                                Text {
                                    text: notification.title
                                    font.pixelSize: 14
                                    font.weight: notification.read ? Font.Normal : Font.Bold
                                    color: "#333"
                                    Layout.fillWidth: true
                                }

                                Text {
                                    text: notification.time
                                    font.pixelSize: 12
                                    color: "#999"
                                }
                            }

                            Text {
                                text: notification.description
                                font.pixelSize: 13
                                color: "#666"
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                            }
                        }

                        // 快速操作区域
                        RowLayout {
                            spacing: 8
                            visible: notification.type === "friend_request" && !notification.read
                            
                            Button {
                                text: "拒绝"
                                Layout.preferredHeight: 28
                                background: Rectangle { radius: 14; color: "#f1f5f9" }
                                contentItem: Text { text: parent.text; color: "#64748b"; font.pixelSize: 12; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                                onClicked: {
                                    ChatService.respondToFriendRequest(notification.refId, false)
                                    notification.read = true // 本地标记避免重复操作
                                    notificationsPage.refreshData()
                                }
                            }
                            
                            Button {
                                text: "同意"
                                Layout.preferredHeight: 28
                                background: Rectangle { radius: 14; color: "#10b981" }
                                contentItem: Text { text: parent.text; color: "white"; font.pixelSize: 12; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                                onClicked: {
                                    ChatService.respondToFriendRequest(notification.refId, true)
                                    notification.read = true
                                    notificationsPage.refreshData()
                                }
                            }
                        }

                        // 未读指示符
                        Rectangle {
                            visible: !notification.read
                            Layout.preferredWidth: 8
                            Layout.preferredHeight: 8
                            radius: 4
                            color: "#2b90ff"
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: parent.color = notification.read ? "#f9f9f9" : "#e8f4ff"
                        onExited: parent.color = notification.read ? "white" : "#f0f8ff"
                    }
                }
            }
        }
    }
}
