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
    property var messageModel: null

    // 当会话 ID 改变时，刷新消息模型
    onCurrentConversationIdChanged: {
        messageModel = ChatService.getMessageModel(currentConversationId)
        console.log("[ChatPage] Switched to conversation:", currentConversationId)

        if (currentConversationId !== "") {
            ChatService.refreshMessages(currentConversationId)
            Qt.callLater(function() {
                scrollToBottom()
            })
        }
    }

    Component.onCompleted: {
        messageModel = ChatService.getMessageModel(currentConversationId)
    }

    Connections {
        target: ChatService
        function onMessageReceived(conversationId, message) {
            if (conversationId === root.currentConversationId) {
                Qt.callLater(function() {
                    chatView.positionViewAtEnd()
                })
            }
        }
    }

    Connections {
        target: ChatService
        function onMessagesRefreshed(conversationId) {
            if (conversationId === root.currentConversationId) {
                Qt.callLater(function() {
                    chatView.positionViewAtEnd()
                })
            }
        }
    }

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

    // 上传并发送文件
    function uploadAndSendFile(fileUrl) {
        console.log("上传文件:", fileUrl)
        
        // 调用 ChatService 发送文件消息
        ChatService.sendFileMessage(currentConversationId, fileUrl.toString().replace("file:///", ""))
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

    // 完整的聊天界面内容
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

        // 消息列表
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
                height: Math.max(bubble.height + 16, 44)

                readonly property bool isSelfMessage: model.senderId === ChatService.currentUserId

                Avatar {
                    id: leftAvatar
                    isSelf: false
                    x: 12
                    y: 8
                    visible: !isSelfMessage
                    avatarSource: "qrc:/new/prefix1/image/boy.png"
                }

                Avatar {
                    id: rightAvatar
                    isSelf: true
                    x: parent.width - width - 12
                    y: 8
                    visible: isSelfMessage
                    avatarSource: ChatService.currentUserAvatar || "qrc:/new/prefix1/image/boy.png"
                }

                MessageBubble {
                    id: bubble
                    x: isSelfMessage ? (rightAvatar.x - width - 8) : (leftAvatar.x + leftAvatar.width + 8)
                    y: 8
                    width: parent.width - 80
                    content: model.content
                    isSelf: model.senderId === ChatService.currentUserId
                    timestamp: model.timestamp
                    timeAlignment: isSelfMessage ? "right" : "left"
                    // 绑定消息类型和状态
                    messageType: model.type || 0
                    fileName: model.fileName || ""
                    fileSize: model.fileSize || ""
                    fileUrl: model.fileUrl || ""
                    thumbnailUrl: model.thumbnailUrl || ""
                    messageId: model.messageId || ""
                    // 判断是否可撤回（2 分钟内）
                    isRecalled: model.recalled || false
                    canRecall: {
                        var now = Date.now()
                        var msgTime = model.timestamp
                        if (msgTime < 10000000000) msgTime *= 1000  // 秒转毫秒
                        var diff = now - msgTime
                        return isSelfMessage && (diff < 2 * 60 * 1000)  // 2 分钟
                    }
                    onMessageRecalled: {
                        if (messageId) {
                            ChatService.recallMessage(currentConversationId, messageId)
                        }
                    }
                }
            }
        }

        // 输入区域
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

                // 附件按钮
                Text {
                    width: 40; height: 40
                    text: "📎"; font.pixelSize: 22; color: "#757575"
                    verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                    ToolTip.visible: mouseArea.containsMouse
                    ToolTip.text: "发送文件"
                    
                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked: {
                            var path = ChatService.pickLocalFile(false)
                            if (path && path.length > 0) {
                                uploadAndSendFile(path)
                            }
                        }
                    }
                }

                // 图片按钮
                Text {
                    width: 40; height: 40
                    text: "🖼️"; font.pixelSize: 22; color: "#757575"
                    verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                    ToolTip.visible: mouseArea2.containsMouse
                    ToolTip.text: "发送图片"
                    
                    MouseArea {
                        id: mouseArea2
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked: {
                            var path = ChatService.pickLocalFile(true)
                            if (path && path.length > 0) {
                                uploadAndSendFile(path)
                            }
                        }
                    }
                }

                // 输入框
                Item {
                    width: parent.width - 200
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
        ChatService.sendMessage(currentConversationId, content)
        console.log("[ChatPage] Sent message:", content)
        inputText.text = ""
    }
}
