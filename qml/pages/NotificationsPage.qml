import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: notificationsPage
    color: "white"

    property var notifications: [
        {
            id: 1,
            type: "message",
            title: "新消息来自 Alice",
            description: "Hi, how are you?",
            time: "2 分钟前",
            icon: "💬",
            read: false
        },
        {
            id: 2,
            type: "system",
            title: "系统通知",
            description: "数据库连接已恢复",
            time: "5 分钟前",
            icon: "🔔",
            read: true
        },
        {
            id: 3,
            type: "message",
            title: "新消息来自 Bob",
            description: "See you later!",
            time: "15 分钟前",
            icon: "💬",
            read: true
        },
        {
            id: 4,
            type: "system",
            title: "好友请求",
            description: "Charlie 要求添加你为好友",
            time: "1 小时前",
            icon: "👥",
            read: true
        }
    ]

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        // 标题栏
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
                model: notifications.length
                spacing: 8

                delegate: Rectangle {
                    width: notificationsList.width
                    height: 80
                    color: notification.read ? "white" : "#f0f8ff"
                    radius: 4
                    border.color: "#eee"

                    property var notification: notifications[index]

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
                                font.pixelSize: 12
                                color: "#666"
                                Layout.fillWidth: true
                                elide: Text.ElideRight
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
