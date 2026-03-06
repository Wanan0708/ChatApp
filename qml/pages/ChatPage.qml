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

    // 当会话ID改变时，刷新消息模型
    onCurrentConversationIdChanged: {
        messageModel = ChatService.getMessageModel(currentConversationId)
        console.log("[ChatPage] Switched to conversation:", currentConversationId)
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
                    Qt.callLater(function() {
                        if (chatView.contentHeight > chatView.height) {
                            chatView.contentY = chatView.contentHeight - chatView.height
                        }
                    }, 16)
                }, 16)
            }
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

        // 消息列表（关键：用纯Flickable+Column，无Layout）
        Flickable {
            id: chatView
            Layout.fillWidth: true
            Layout.fillHeight: true
            contentHeight: messageColumn.height
            contentWidth: width
            clip: true
            boundsBehavior: Flickable.StopAtBounds

            property bool userIsAtBottom: true
            property real lastContentHeight: 0

            onContentYChanged: {
                userIsAtBottom = (contentY >= contentHeight - height - 10)
            }

            onContentHeightChanged: {
                // 仅当用户在底部时自动滚动
                if (userIsAtBottom && contentHeight > lastContentHeight) {
                    // 双重延迟确保布局完全稳定（Qt 5.14.2必需）
                    Qt.callLater(function() {
                        Qt.callLater(function() {
                            if (contentHeight > height) {
                                contentY = contentHeight - height
                                // console.log("Auto-scrolled to bottom:", contentY)
                            }
                        }, 16) // 1帧
                    }, 16) // 1帧
                }
                lastContentHeight = contentHeight
            }

            Column {
                id: messageColumn
                width: parent.width
                spacing: 1
                padding: 12

                Repeater {
                    model: root.messageModel
                    delegate: Item {
                        width: parent.width
                        height: Math.max(bubble.height + 15, 40)  // 确保最小高度

                        // 头像（左侧）
                        Avatar {
                            id: leftAvatar
                            isSelf: false
                            x: 0
                            y: (parent.height - height) / 2
                            visible: !isSelfMessage
                            avatarSource: "qrc:/new/prefix1/image/boy.png"
                        }

                        // 头像（右侧）
                        Avatar {
                            id: rightAvatar
                            isSelf: true
                            x: parent.width - width - 24
                            y: (parent.height - height) / 2
                            visible: isSelfMessage
                            avatarSource: ChatService.currentUserAvatar || "qrc:/new/prefix1/image/boy.png"
                        }

                        // 消息气泡
                        MessageBubble {
                            id: bubble
                            x: isSelfMessage ? (rightAvatar.x - width - 8) : (leftAvatar.x + leftAvatar.width + 8)
                            y: (parent.height - height) / 2 + 8
                            width: parent.width - 64
                            content: model.content
                            isSelf: model.senderId === ChatService.currentUserId
                            timestamp: model.timestamp
                            timeAlignment: isSelfMessage ? "right" : "left"
                        }

                        // 计算是否自己消息
                        readonly property bool isSelfMessage: model.senderId === ChatService.currentUserId
                    }
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
