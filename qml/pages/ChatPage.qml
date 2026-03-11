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
    property string currentAvatar: ""
    property string currentConversationType: "single"
    property var messageModel: null
    property bool pendingOlderMessagesLoad: false
    property real historyContentHeightBeforeLoad: 0
    property bool stickToBottom: true

    function insertEmojiAtCursor(emoji) {
        if (!emoji || emoji.length === 0) {
            return
        }

        var cursor = inputText.cursorPosition
        inputText.insert(cursor, emoji)
        inputText.cursorPosition = cursor + emoji.length
        inputText.forceActiveFocus()
    }

    function syncConversationMeta() {
        if (currentConversationId === "") {
            currentTitle = ""
            currentAvatar = ""
            currentConversationType = "single"
            return
        }

        var conversation = ChatService.conversationModel.getById(currentConversationId)
        if (conversation && conversation.title) {
            currentTitle = conversation.title
        }
        if (conversation && conversation.avatar !== undefined) {
            currentAvatar = conversation.avatar || ""
        }
        if (conversation && conversation.type !== undefined) {
            currentConversationType = conversation.type || "single"
        }
    }

    // 当会话 ID 改变时，刷新消息模型
    onCurrentConversationIdChanged: {
        messageModel = currentConversationId !== "" ? ChatService.getMessageModel(currentConversationId) : null
        pendingOlderMessagesLoad = false
        historyContentHeightBeforeLoad = 0
        stickToBottom = true
        syncConversationMeta()
        console.log("[ChatPage] Switched to conversation:", currentConversationId)

        if (currentConversationId !== "") {
            ChatService.refreshMessages(currentConversationId)
            Qt.callLater(function() {
                scrollToBottom()
            })
        }
    }

    Component.onCompleted: {
        messageModel = currentConversationId !== "" ? ChatService.getMessageModel(currentConversationId) : null
        syncConversationMeta()
    }

    Connections {
        target: ChatService
        function onConversationIdResolved(fromConversationId, toConversationId) {
            if (root.currentConversationId === fromConversationId) {
                root.currentConversationId = toConversationId
            }
        }
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
                pendingOlderMessagesLoad = false
                Qt.callLater(function() {
                    chatView.positionViewAtEnd()
                })
            }
        }
    }

    Connections {
        target: ChatService
        function onConversationUpdated(conversationId) {
            if (conversationId === root.currentConversationId) {
                syncConversationMeta()
            }
        }
    }

    Connections {
        target: ChatService.conversationModel
        function onModelDataChanged() {
            root.syncConversationMeta()
        }
    }

    Connections {
        target: ChatService
        function onOlderMessagesLoaded(conversationId, addedCount, hasMore) {
            if (conversationId !== root.currentConversationId) {
                return
            }

            pendingOlderMessagesLoad = false

            Qt.callLater(function() {
                var delta = chatView.contentHeight - historyContentHeightBeforeLoad
                if (delta > 0) {
                    chatView.contentY = delta
                }
            })
        }
    }

    onMessageModelChanged: {
        if (messageModel && currentConversationId !== "") {
            stickToBottom = true
            Qt.callLater(function() {
                scrollToBottom()
            })
        }
    }

    function isNearBottom() {
        if (!chatView) {
            return true
        }

        return (chatView.contentY + chatView.height) >= (chatView.contentHeight - 48)
    }

    function scrollToBottom() {
        if (chatView && chatView.count > 0) {
            chatView.positionViewAtEnd()
        }
    }

    function loadOlderMessagesIfNeeded() {
        if (pendingOlderMessagesLoad || currentConversationId === "") {
            return
        }

        if (!ChatService.hasMoreMessages(currentConversationId) || ChatService.isLoadingMessages(currentConversationId)) {
            return
        }

        historyContentHeightBeforeLoad = chatView.contentHeight
        pendingOlderMessagesLoad = ChatService.loadOlderMessages(currentConversationId)
    }

    function normalizeLocalPath(fileUrl) {
        var path = fileUrl ? fileUrl.toString() : ""
        if (path.indexOf("file:///") === 0) {
            path = decodeURIComponent(path.substring("file:///".length))
        }
        return path
    }

    // 上传并发送文件
    function uploadAndSendFile(fileUrl) {
        var localPath = normalizeLocalPath(fileUrl)
        console.log("上传文件:", localPath)

        ChatService.sendFileMessage(currentConversationId, localPath)
    }

    // 上传并发送图片
    function uploadAndSendImage(fileUrl) {
        var localPath = normalizeLocalPath(fileUrl)
        console.log("上传图片:", localPath)

        ChatService.sendImageMessage(currentConversationId, localPath)
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
                    avatarSource: root.currentAvatar || "qrc:/new/prefix1/image/boy.png"
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
            verticalLayoutDirection: ListView.TopToBottom
            highlightRangeMode: ListView.StrictlyEnforceRange
            onContentYChanged: {
                if (contentY <= 0 && movingVertically) {
                    root.loadOlderMessagesIfNeeded()
                }

                if (movingVertically && !root.pendingOlderMessagesLoad) {
                    root.stickToBottom = root.isNearBottom()
                }
            }

            onContentHeightChanged: {
                if (root.currentConversationId !== "" && root.stickToBottom && !root.pendingOlderMessagesLoad) {
                    Qt.callLater(function() {
                        if (root.stickToBottom) {
                            chatView.positionViewAtEnd()
                        }
                    })
                }
            }

            header: Item {
                width: chatView.width
                height: root.pendingOlderMessagesLoad || ChatService.hasMoreMessages(root.currentConversationId) ? 36 : 18

                BusyIndicator {
                    anchors.centerIn: parent
                    running: root.pendingOlderMessagesLoad
                    visible: running
                    width: 24
                    height: 24
                }

                Text {
                    anchors.centerIn: parent
                    visible: !root.pendingOlderMessagesLoad && root.currentConversationId !== "" && !ChatService.hasMoreMessages(root.currentConversationId)
                    text: "没有更早的消息了"
                    font.pixelSize: 12
                    color: "#999"
                    font.family: "Microsoft YaHei, SimSun, sans-serif"
                }
            }

            delegate: Item {
                width: ListView.view.width
                readonly property bool isGroupConversation: root.currentConversationType !== "single" && root.currentConversationType !== "user" && root.currentConversationType !== ""
                readonly property var previousMessage: (root.messageModel && index > 0) ? root.messageModel.get(index - 1) : null
                readonly property bool previousIsSystem: previousMessage && Number(previousMessage.type || 0) === 3
                readonly property bool currentIsSystem: Number(model.type || 0) === 3
                readonly property bool sameSenderAsPrevious: previousMessage && previousMessage.senderId === model.senderId
                readonly property bool isGroupedMessage: isGroupConversation && sameSenderAsPrevious && !previousIsSystem && !currentIsSystem
                readonly property real bubbleTopMargin: isGroupedMessage ? 2 : 10
                height: Math.max(bubble.y + bubble.height + 8, (leftAvatar.visible ? leftAvatar.y + leftAvatar.height + 8 : bubble.y + bubble.height + 8), 44)

                readonly property bool isSelfMessage: model.senderId === ChatService.currentUserId

                Avatar {
                    id: leftAvatar
                    isSelf: false
                    x: 12
                    y: bubble.showSenderName ? (bubble.y + 18) : bubble.y
                    visible: !isSelfMessage && !isGroupedMessage
                    avatarSource: root.currentAvatar || "qrc:/new/prefix1/image/boy.png"
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
                    y: bubbleTopMargin
                    width: parent.width - 80
                    content: model.content
                    isSelf: model.senderId === ChatService.currentUserId
                    senderId: model.senderId || ""
                    senderName: model.senderName || ""
                    conversationType: root.currentConversationType
                    compactGroup: isGroupedMessage
                    timestamp: model.timestamp
                    timeAlignment: isSelfMessage ? "right" : "left"
                    // 绑定消息类型和状态
                    messageType: {
                        if (typeof model.type === "number") return model.type
                        if (model.type === "image") return 1
                        if (model.type === "file") return 2
                        if (model.type === "system") return 3
                        var parsedType = parseInt(model.type, 10)
                        return isNaN(parsedType) ? 0 : parsedType
                    }
                    messageStatus: {
                        if (typeof model.status === "number") return model.status
                        var parsedStatus = parseInt(model.status, 10)
                        return isNaN(parsedStatus) ? 1 : parsedStatus
                    }
                    fileName: model.fileName || ""
                    fileSize: model.fileSize || ""
                    fileUrl: model.fileUrl || ""
                    thumbnailUrl: model.thumbnailUrl || ""
                    messageId: model.messageId || ""
                    internalMessageId: model.internalMessageId || model.messageId || ""
                    isOffline: !!model.isOffline
                    errorText: model.errorText || ""
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
                    onMessageDeleteRequested: {
                        var targetMessageId = internalMessageId || messageId
                        if (targetMessageId) {
                            ChatService.deleteLocalMessage(currentConversationId, targetMessageId)
                        }
                    }
                    onRetryRequested: {
                        if (internalMessageId) {
                            ChatService.retryMessage(currentConversationId, internalMessageId)
                        }
                    }
                }
            }
        }

        // 输入区域
        Rectangle {
            id: composerBar
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
                                uploadAndSendImage(path)
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
                    id: emojiButton
                    width: 40; height: 40
                    text: "😊"; font.pixelSize: 22; color: "#757575"
                    verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            emojiPopup.open()
                        }
                    }
                }

                // 发送
                Button {
                    id: sendButton
                    width: 60; height: 40
                    text: "发送"
                    hoverEnabled: true
                    font.pixelSize: 14
                    font.family: "Microsoft YaHei, SimSun, sans-serif"
                    background: Rectangle {
                        radius: 10
                        color: sendButton.down ? "#1d4ed8" : (sendButton.hovered ? "#3b82f6" : "#2563eb")
                        border.color: sendButton.hovered ? "#1e40af" : "#1d4ed8"
                        border.width: 1
                        scale: sendButton.down ? 0.96 : 1.0
                        Behavior on color {
                            ColorAnimation { duration: 140 }
                        }
                        Behavior on border.color {
                            ColorAnimation { duration: 140 }
                        }
                        Behavior on scale {
                            NumberAnimation { duration: 100; easing.type: Easing.OutCubic }
                        }
                    }
                    contentItem: Text {
                        text: sendButton.text
                        color: "white"
                        font.pixelSize: sendButton.font.pixelSize
                        font.family: sendButton.font.family
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: sendMessage()
                }
            }

            Popup {
                id: emojiPopup
                parent: root
                width: 360
                height: 420
                modal: false
                focus: true
                padding: 0
                closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
                x: Math.max(8, root.width - width - 24)
                y: Math.max(8, composerBar.y - height - 8)
                background: Rectangle {
                    radius: 10
                    color: "white"
                    border.color: "#d9d9d9"
                    border.width: 1
                }

                EmojiPicker {
                    anchors.fill: parent
                    onEmojiSelected: {
                        root.insertEmojiAtCursor(emoji)
                        emojiPopup.close()
                    }
                    onCloseRequested: emojiPopup.close()
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
