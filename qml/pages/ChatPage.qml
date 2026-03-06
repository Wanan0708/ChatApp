import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import ChatApp.Models 1.0
import ChatApp.Network 1.0
import "../components"

ColumnLayout {
    id: root
    property string currentConversationId: ""
    property string currentTitle: ""
    property var messageModel: null  // 初始为空

    // 当会话 ID 改变时，刷新消息模型
    onCurrentConversationIdChanged: {
        messageModel = ChatService.getMessageModel(currentConversationId)
        console.log("[ChatPage] Switched to conversation:", currentConversationId)

        // 主动加载消息
        if (currentConversationId !== "") {
            ChatService.refreshMessages(currentConversationId)
            // 延迟滚动到底部，确保消息已加载
            Qt.callLater(function() {
                scrollToBottom()
            })
        }
    }

    Component.onCompleted: {
        // 初始化时也获取一次
        messageModel = ChatService.getMessageModel(currentConversationId)
    }

    Connections {
        target: ChatService
        function onMessageReceived(conversationId, message) {
            // 如果是当前会话，自动滚动到底部
            if (conversationId === root.currentConversationId) {
                Qt.callLater(function() {
                    chatView.positionViewAtEnd()
                })
            }
        }
    }

    // 监听消息刷新完成信号，滚动到底部
    Connections {
        target: ChatService
        function onMessagesRefreshed(conversationId) {
            // 如果是当前会话，滚动到底部显示最新消息
            if (conversationId === root.currentConversationId) {
                Qt.callLater(function() {
                    chatView.positionViewAtEnd()
                })
            }
        }
    }

    // 监听消息模型变化，确保首次加载时滚动到底部
    onMessageModelChanged: {
        if (messageModel && currentConversationId !== "") {
            Qt.callLater(function() {
                scrollToBottom()
            })
        }
    }

    function scrollToBottom() {
        if (chatView && chatView.count > 0) {
            chatView.positionViewAtEnd()
        }
    }

    // 如果未选择会话，显示占位提示
    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
        visible: root.currentConversationId === ""

        Column {
            anchors.centerIn: parent
            spacing: 16

            Text {
                text: "💬"
                font.pixelSize: 64
                color: "#e0e0e0"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                text: "请选择一个会话开始聊天"
                font.pixelSize: 16
                color: "#999999"
                font.family: "Microsoft YaHei, SimSun, sans-serif"
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

    // 完整的聊天界面内容，只有选中会话时才显示
    ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        visible: root.currentConversationId !== ""

        // 顶部标题栏
        Rectangle {
            Layout.preferredHeight: 60
            Layout.fillWidth: true
            color: "#FFFFFE"
            border.color: "#E2E8F0"
            border.width: 1

            RowLayout {
                anchors.fill: parent
                spacing: 12
                anchors.leftMargin: 16
                anchors.rightMargin: 16

                Avatar {
                    isSelf: false
                    width: 36
                    height: 36
                    avatarSource: "qrc:/new/prefix1/image/boy.png"
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 2

                    Text {
                        Layout.fillWidth: true
                        text: root.currentTitle || "请选择会话"
                        color: "#2C3E50"
                        font.pixelSize: 16
                        font.bold: true
                        elide: Text.ElideRight
                        font.family: "Microsoft YaHei, SimSun, sans-serif"
                    }

                    Text {
                        Layout.fillWidth: true
                        text: root.currentTitle ? "聊天中" : ""
                        color: "#757575"
                        font.pixelSize: 12
                        elide: Text.ElideRight
                        font.family: "Microsoft YaHei, SimSun, sans-serif"
                    }
                }

                Text {
                    text: "⋮"
                    color: "#2C3E50"
                    font.pixelSize: 24
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    Layout.preferredWidth: 32
                    Layout.preferredHeight: 32
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: console.log("群设置")
                    }
                }
            }
        }

        // 消息列表（优化：使用 ListView 获取更好的性能）
        ListView {
            id: chatView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: root.messageModel
            spacing: 1
            highlightRangeMode: ListView.StrictlyEnforceRange

            delegate: Item {
                width: ListView.view.width
                height: Math.max(bubble.height + 16, 44)  // 确保最小高度

                // 计算是否自己消息
                readonly property bool isSelfMessage: model.senderId === ChatService.currentUserId

                // 头像（左侧）- 对方的消息
                Avatar {
                    id: leftAvatar
                    isSelf: false
                    x: 12  // 左边距
                    y: 8
                    visible: !isSelfMessage
                    avatarSource: "qrc:/new/prefix1/image/boy.png"
                }

                // 头像（右侧）- 自己的消息
                Avatar {
                    id: rightAvatar
                    isSelf: true
                    x: parent.width - width - 12
                    y: 8
                    visible: isSelfMessage
                    avatarSource: ChatService.currentUserAvatar || "qrc:/new/prefix1/image/boy.png"
                }

                // 消息气泡
                MessageBubble {
                    id: bubble
                    x: isSelfMessage ? (rightAvatar.x - width - 8) : (leftAvatar.x + leftAvatar.width + 8)
                    y: 8
                    width: parent.width - 80
                    content: model.content
                    isSelf: model.senderId === ChatService.currentUserId
                    timestamp: model.timestamp
                    timeAlignment: isSelfMessage ? "right" : "left"
                }
            }
        }

        // 输入区域（简化布局）
        Rectangle {
            Layout.preferredHeight: 60
            Layout.fillWidth: true
            color: "#f0f0f0"
            border.color: "#ddd"
            border.width: 1

            Row {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 4

                // 附件
                Text {
                    width: 40; height: 40
                    text: "📎"; font.pixelSize: 22; color: "#757575"
                    verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                    MouseArea { anchors.fill: parent; onClicked: console.log("附件") }
                }

                // 输入框
                Item {
                    width: parent.width - 160;
                    height: Math.max(40, Math.min(120, inputText.implicitHeight))

                    TextArea {
                        id: inputText
                        anchors.fill: parent
                        verticalAlignment: TextArea.AlignVCenter
                        font.pixelSize: 14
                        wrapMode: TextEdit.Wrap
                        selectByMouse: true
                        color: inputText.text ? "#333" : "#999"
                        font.family: "Microsoft YaHei, SimSun, sans-serif"

                        placeholderText: "输入消息..."
                        placeholderTextColor: "#999"

                        background: Rectangle {
                            radius: 10
                            color: "white"
                            border.color: inputText.activeFocus ? "#1e90ff" : "#ccc"
                            border.width: inputText.activeFocus ? 2 : 1

                            Behavior on border.color {
                                ColorAnimation { duration: 200 }
                            }
                            Behavior on border.width {
                                NumberAnimation { duration: 200 }
                            }
                        }

                        Keys.onPressed: {
                            if (event.key === Qt.Key_Enter && (event.modifiers & Qt.ControlModifier)) {
                                inputText.insert(inputText.cursorPosition, "\n")
                                event.accepted = true
                            } else if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
                                sendMessage()
                                event.accepted = true
                            }
                        }
                    }
                }

                // 表情
                Text {
                    width: 40; height: 40
                    text: "😊"; font.pixelSize: 22; color: "#757575"
                    verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                    MouseArea { anchors.fill: parent; onClicked: console.log("表情") }
                }

                // 发送
                Button {
                    width: 60; height: 40
                    text: "发送"
                    font.pixelSize: 14
                    font.family: "Microsoft YaHei, SimSun, sans-serif"
                    onClicked: sendMessage()
                }
            }
        }
    }

    function sendMessage() {
        if (inputText.text.trim() === "") return

        var content = inputText.text.trim()

        // 如果 currentConversationId 是用户 ID（以 user_开头），直接发送
        // 后端会自动创建会话
        ChatService.sendMessage(currentConversationId, content)

        console.log("[ChatPage] Sent message:", content)

        inputText.text = ""
    }
}
