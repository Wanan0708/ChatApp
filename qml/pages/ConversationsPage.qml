import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Shapes 1.12
import ChatApp.Models 1.0
import ChatApp.Network 1.0
import "../components"

ColumnLayout {
    id: root
    signal conversationSelected(string conversationId, string title, string avatar)

    // 搜索关键词
    property string searchText: ""

    Connections {
        target: ChatService

        // 新消息到达时，MessageModel 和 ConversationModel 已在 C++ 端自动更新
        // QML 通过绑定自动刷新视图
        function onMessageReceived(conversationId, message) {
            // 此处留空，UI 会通过数据绑定自动更新
        }
    }

    // 搜索框
    Rectangle {
        Layout.preferredHeight: 50
        Layout.fillWidth: true
        color: "transparent"

        RowLayout {
            anchors.fill: parent
            anchors.margins: 8
            spacing: 8

            TextField {
                id: searchField
                Layout.fillWidth: true
                selectByMouse: true
                font.pixelSize: 14
                font.family: "Microsoft YaHei, SimSun, sans-serif"
                color: "#333"
                placeholderText: "搜索联系人/群组..."
                placeholderTextColor: "#999"

                verticalAlignment: TextInput.AlignVCenter

                background: Rectangle {
                    radius: 6
                    color: "white"
                    border.color: searchField.activeFocus ? "#3B82F6" : "#ccc"
                    border.width: 1
                }

                onTextChanged: {
                    root.searchText = text.trim()
                }
            }

            // 清除按钮
            MouseArea {
                visible: searchField.text.length > 0
                Layout.preferredWidth: 24
                Layout.preferredHeight: 24
                cursorShape: Qt.PointingHandCursor

                Rectangle {
                    anchors.centerIn: parent
                    width: 20
                    height: 20
                    radius: 10
                    color: parent.containsMouse ? "#e0e0e0" : "transparent"

                    Text {
                        anchors.centerIn: parent
                        text: "×"
                        font.pixelSize: 16
                        color: "#666"
                    }
                }

                onClicked: {
                    searchField.text = ""
                    root.searchText = ""
                }

                property bool containsMouse: false
                onEntered: containsMouse = true
                onExited: containsMouse = false
            }
        }
    }

    Rectangle {
        id: networkStatus
        Layout.preferredHeight: statusVisible ? 36 : 0
        Layout.fillWidth: true
        color: statusColor
        border.color: statusBorderColor
        border.width: 1
        clip: true  // 防止子元素溢出

        // 平滑高度动画
        Behavior on Layout.preferredHeight {
            NumberAnimation { duration: 300; easing.type: Easing.OutCubic }
        }

        // 状态计算
        property bool statusVisible: !networkStatus.isConnected || networkStatus.showConnectedMessage
        property color statusColor: networkStatus.isConnected
                                    ? (networkStatus.showConnectedMessage ? "#e8f5e9" : "transparent")
                                    : (networkStatus.isReconnecting ? "#fff8e1" : "#ffebee")
        property color statusBorderColor: networkStatus.isConnected
                                          ? (networkStatus.showConnectedMessage ? "#4caf50" : "transparent")
                                          : (networkStatus.isReconnecting ? "#f59e0b" : "#f44336")
        property bool isConnected: ChatService.isConnected
        property bool isReconnecting: ChatService.isReconnecting
        property string connectionState: ChatService.connectionState
        property bool showConnectedMessage: false  // 连接成功后短暂显示
        property string disconnectedText: networkStatus.isReconnecting
                                          ? "网络已断开，正在自动重连..."
                                          : "网络连接不可用"

        Row {
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 8
            spacing: 8
            visible: !networkStatus.isConnected

            Shape {
                id: spinner
                width: 16; height: 16
                anchors.verticalCenter: parent.verticalCenter
                visible: networkStatus.isReconnecting

                property real spinnerAngle: 0

                ShapePath {
                    fillColor: "transparent"
                    strokeColor: "#1976d2"
                    strokeWidth: 2
                    startX: 8; startY: 2
                    PathArc {
                        x: 8; y: 14
                        radiusX: 6; radiusY: 6
                        useLargeArc: false
                        direction: PathArc.Clockwise
                    }
                }

                transform: Rotation {
                    origin.x: 8; origin.y: 8
                    angle: spinner.spinnerAngle
                }

                SequentialAnimation on spinnerAngle {
                    running: networkStatus.visible && networkStatus.isReconnecting
                    loops: Animation.Infinite
                    NumberAnimation { from: 0; to: 360; duration: 1000; easing.type: Easing.Linear }
                }
            }

            Rectangle {
                width: 16
                height: 16
                radius: 8
                color: "#fde68a"
                border.color: "#d97706"
                border.width: 1
                anchors.verticalCenter: parent.verticalCenter
                visible: !networkStatus.isReconnecting

                Text {
                    anchors.centerIn: parent
                    text: "!"
                    color: "#92400e"
                    font.pixelSize: 12
                    font.bold: true
                }
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: networkStatus.disconnectedText
                font.pixelSize: 13
                color: networkStatus.isReconnecting ? "#b45309" : "#d32f2f"
                font.family: "Microsoft YaHei, SimSun, sans-serif"
            }

            Item {
                width: 1
                height: 1
            }

            Button {
                anchors.verticalCenter: parent.verticalCenter
                text: networkStatus.isReconnecting ? "立即重试" : "重连"
                visible: !networkStatus.isConnected
                font.pixelSize: 12
                font.family: "Microsoft YaHei, SimSun, sans-serif"

                background: Rectangle {
                    radius: 5
                    color: parent.pressed ? "#d97706" : "#f59e0b"
                }

                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.pixelSize: parent.font.pixelSize
                    font.family: parent.font.family
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: ChatService.connectToDefaultChatServer()
            }
        }

        // 已连接消息（短暂显示后淡出）
        Row {
            anchors.centerIn: parent
            spacing: 8
            visible: networkStatus.showConnectedMessage

            Rectangle {
                width: 10; height: 10
                radius: 5
                color: "#4caf50"
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                text: "✓ 已连接"
                font.pixelSize: 13
                color: "#2e7d32"
                font.bold: true
                font.family: "Microsoft YaHei, SimSun, sans-serif"
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        // 自动淡出定时器
        Timer {
            id: fadeOutTimer
            interval: 3000  // 3秒后淡出
            onTriggered: networkStatus.showConnectedMessage = false
        }

        // 网络状态变化监听
        Connections {
            target: ChatService
            function onConnectedChanged(connected) {
                if (connected) {
                    // 连接成功：显示"已连接"3秒后淡出
                    networkStatus.showConnectedMessage = true
                    fadeOutTimer.restart()
                } else {
                    // 断开连接：立即显示重连状态
                    networkStatus.showConnectedMessage = false
                }
            }
        }
    }

    // 过滤后的会话列表
    property var filteredConversations: []
    
    function updateFilteredConversations() {
        var result = []
        if (!ChatService.conversationModel) return
        var searchLower = searchText.toLowerCase()
        var count = ChatService.conversationModel.count
        for (var i = 0; i < count; i++) {
            var conv = ChatService.conversationModel.get(i)
            if (conv) {
                var title = conv.title || ""
                var lastMessage = conv.lastMessage || ""
                var time = conv.time || ""
                console.log("[ConversationsPage] Conversation", i, ":", title, "lastMessage:", lastMessage, "time:", time)
                // 如果搜索关键词为空，或标题/最后消息包含搜索词，则显示
                if (searchText === "" ||
                    title.toLowerCase().indexOf(searchLower) >= 0 ||
                    lastMessage.toLowerCase().indexOf(searchLower) >= 0) {
                    result.push(conv)
                }
            }
        }
        filteredConversations = result
    }

    // 监听模型数据变化
    Connections {
        target: ChatService.conversationModel
        function onCountChanged() {
            updateFilteredConversations()
        }
        function onModelDataChanged() {
            updateFilteredConversations()
        }
    }

    // 初始加载
    onSearchTextChanged: updateFilteredConversations()
    Component.onCompleted: updateFilteredConversations()

    // 会话列表
    ScrollView {
        Layout.fillWidth: true
        Layout.fillHeight: true

        Column {
            width: root.width
            spacing: 1

            Repeater {
                model: root.filteredConversations

                delegate: Rectangle {
                    width: parent.width
                    height: 70

                    property var convData: modelData
                    property string convId: convData && convData.id ? convData.id : ""
                    property string convTitle: convData && convData.title ? convData.title : ""
                    property string convLastMessage: convData && convData.lastMessage ? convData.lastMessage : ""
                    property string convTime: convData && convData.time ? convData.time : ""
                    property string convAvatar: convData && convData.avatar ? convData.avatar : ""
                    property int convUnreadCount: convData && convData.unreadCount !== undefined ? convData.unreadCount : 0
                    property string convType: convData && convData.type ? convData.type : "user"
                    property bool convIsCurrent: convData && convData.isCurrent ? convData.isCurrent : false

                    property bool isCurrent: convId === ChatService.currentConversationId

                    color: isCurrent
                           ? "#fffff5"  // 蓝色高亮背景
                           : (mouseArea.pressed ? "#fffff5" : "#FFF0F0")
                    border.color: isCurrent ? "#fffff5" : "#eee"
                    border.width: isCurrent ? 2 : 0

                    Row {
                        anchors.fill: parent
                        spacing: 12
                        padding: 8

                        Avatar {
                            id: avatar
                            isSelf: false
                            avatarSource: convAvatar || "qrc:/new/prefix1/image/boy.png"
                        }

                        Item {
                            width: parent.width - timeItem.width - avatar.width - 45
                            height: parent.height
                            anchors.verticalCenter: parent.verticalCenter

                            Column {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                spacing: 4
                                width: parent.width

                                Text {
                                    text: convTitle
                                    font.pixelSize: 16
                                    font.bold: true
                                    color: "#212121"
                                    elide: Text.ElideRight
                                    width: parent.width
                                    font.family: "Microsoft YaHei, SimSun, sans-serif"
                                }

                                Text {
                                    text: convLastMessage
                                    font.pixelSize: 13
                                    color: "#757575"
                                    elide: Text.ElideRight
                                    width: parent.width
                                    font.family: "Microsoft YaHei, SimSun, sans-serif"
                                }
                            }
                        }

                        Item {
                            id: timeItem
                            width: 40
                            height: parent.height
                            anchors.verticalCenter: parent.verticalCenter

                            Column {
                                anchors.right: parent.right
                                spacing: 10
                                anchors.verticalCenter: parent.verticalCenter

                                // ✅ 时间显示（仅当无未读消息时）
                                Text {
                                    text: convTime
                                    font.pixelSize: 11
                                    color: "#9e9e9e"
                                    font.family: "Microsoft YaHei, SimSun, sans-serif"
                                    horizontalAlignment: Text.AlignRight
                                }

                                // ✅ 未读消息计数（优先显示）
                                Rectangle {
                                    width: convUnreadCount > 99 ? 24 : 20
                                    height: 20
                                    radius: 10
                                    color: convUnreadCount > 0 ? "#F44336" : "transparent"
                                    anchors.horizontalCenter: parent.horizontalCenter

                                    Text {
                                        anchors.centerIn: parent
                                        text: convUnreadCount <= 0 ? "" : (convUnreadCount > 99 ? "99+" : convUnreadCount)
                                        color: "white"
                                        font.pixelSize: 10
                                        font.bold: true
                                    }
                                }
                            }
                        }
                    }

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        onClicked: {
                            root.conversationSelected(convId, convTitle, convAvatar)
                            Qt.callLater(function() {
                                ChatService.setCurrentConversation(convId)
                            })
                        }
                    }
                }
            }
        }
    }


}
