import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12
import ChatApp.Models 1.0
import ChatApp.Network 1.0
import "../components"

Rectangle {
    id: contactsPage
    color: "white"

    signal contactSelected(string contactId, string contactName)
    signal addContactRequested()

    property var pendingRequests: []
    property bool hasPendingRequests: pendingRequests.length > 0
    
    // 搜索关键词
    property string searchText: ""

    Component.onCompleted: {
        refreshRequests()
        refreshFriends()
    }

    function refreshRequests() {
        ChatService.getPendingFriendRequests()
    }

    function refreshFriends() {
        ChatService.getFriendList()
    }
    
    function handleFriendsLoaded(friends) {
        let newContacts = []
        for (let i = 0; i < friends.length; i++) {
            // 从后端获取头像数据，如果没有则使用默认头像
            let avatar = friends[i].avatar || "qrc:/new/prefix1/image/boy.png"

            newContacts.push({
                                 id: friends[i].userId || "",
                                 name: friends[i].username || "未知用户",
                                 avatar: avatar,
                                 isMale: friends[i].isMale !== undefined ? friends[i].isMale : true,
                                 age: friends[i].age || 25,
                                 region: friends[i].region || "",
                                 status: friends[i].status || "在线",
                                 signature: friends[i].signature || "",
                                 userId: friends[i].userId || ""
                             })
        }
        contacts = newContacts
    }

    function addContact(user) {
        console.log("ContactsPage: Contact request sent to", user.username)
        // Optionally refresh requests immediately
        refreshRequests()
    }

    Connections {
        target: ChatService
        onFriendRequestReceived: {
            refreshRequests()
        }
        onFriendshipEstablished: {
            refreshFriends()
            refreshConversations()  // 刷新会话列表
        }
        onPendingRequestsLoaded: {
            pendingRequests = requests
        }
        onFriendListLoaded: {
            handleFriendsLoaded(friends)
        }
    }

    function refreshConversations() {
        console.log("[ContactsPage] Refreshing conversations...")
        // 重新加载会话列表
        ChatService.refreshConversations()
    }

    // 当前选中的联系人索引，-1 表示未选中
    property int selectedContactIndex: -1

    // 示例联系人列表数据
    property var contacts: []

    // 过滤后的联系人列表
    property var filteredContacts: {
        var result = []
        var searchLower = searchText.toLowerCase()
        for (var i = 0; i < contacts.length; i++) {
            var contact = contacts[i]
            if (searchText === "" ||
                contact.name.toLowerCase().indexOf(searchLower) >= 0) {
                result.push(contact)
            }
        }
        return result
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // ========== 左侧面板：联系人列表 ==========
        Rectangle {
            Layout.preferredWidth: parent.width * 0.30
            Layout.maximumWidth: parent.width * 0.30
            Layout.fillHeight: true
            color: "white"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 0
                spacing: 0

                // 搜索框区域
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 52
                    color: "white"

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 8

                        TextField {
                            id: searchField
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            placeholderText: "搜索联系人..."
                            font.pixelSize: 13
                            font.family: "Microsoft YaHei, SimSun, sans-serif"

                            background: Rectangle {
                                color: "#f5f5f5"
                                border.color: searchField.activeFocus ? "#3B82F6" : "#e0e0e0"
                                border.width: searchField.activeFocus ? 2 : 1
                                radius: 6

                                Behavior on border.color {
                                    ColorAnimation { duration: 150 }
                                }
                            }

                            onTextChanged: {
                                contactsPage.searchText = text.trim()
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
                                contactsPage.searchText = ""
                            }

                            property bool containsMouse: false
                            onEntered: containsMouse = true
                            onExited: containsMouse = false
                        }

                        // 添加好友按钮
                        Rectangle {
                            Layout.preferredWidth: 32
                            Layout.preferredHeight: 32
                            radius: 6
                            color: addMouseArea.containsMouse ? "#eff6ff" : "transparent"
                            border.color: addMouseArea.containsMouse ? "#3b82f6" : "transparent"

                            Text {
                                anchors.centerIn: parent
                                text: "+"
                                font.pixelSize: 22
                                font.weight: Font.Light
                                color: addMouseArea.containsMouse ? "#3b82f6" : "#666"
                            }

                            MouseArea {
                                id: addMouseArea
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: contactsPage.addContactRequested()
                            }
                        }
                    }
                }

                // 分隔线
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: "#e8e8e8"
                }

                // 联系人列表
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    contentWidth: width
                    clip: true

                    ColumnLayout {
                        width: parent.width
                        spacing: 0

                        // 好友申请条目
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 56
                            visible: hasPendingRequests
                            color: selectedContactIndex === -2 ? "#EFF6FF" : "white"
                            
                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 12
                                anchors.rightMargin: 12
                                spacing: 10
                                
                                Rectangle {
                                    Layout.preferredWidth: 40; Layout.preferredHeight: 40; radius: 20
                                    color: "#FEF3C7"
                                    Text { anchors.centerIn: parent; text: "🔔"; font.pixelSize: 20 }
                                }
                                
                                Text {
                                    text: "新好友申请 (" + pendingRequests.length + ")"
                                    font.pixelSize: 14; font.weight: Font.Medium; color: "#D97706"
                                    Layout.fillWidth: true
                                }
                            }
                            
                            MouseArea {
                                anchors.fill: parent
                                onClicked: selectedContactIndex = -2
                            }
                        }

                        ListView {
                            id: contactsList
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            model: contactsPage.filteredContacts.length
                            spacing: 0
                            currentIndex: selectedContactIndex

                            delegate: Rectangle {
                                width: contactsList.width
                                height: 56
                                color: {
                                    if (index === selectedContactIndex) return "#EFF6FF"
                                    if (contactMouseArea.containsMouse) return "#f5f5f5"
                                    return "white"
                                }

                                Behavior on color {
                                    ColorAnimation { duration: 120 }
                                }

                                // 选中指示条
                                Rectangle {
                                    anchors.left: parent.left
                                    anchors.verticalCenter: parent.verticalCenter
                                    width: 3
                                    height: index === selectedContactIndex ? 28 : 0
                                    radius: 2
                                    color: "#3B82F6"
                                    opacity: index === selectedContactIndex ? 1 : 0

                                    Behavior on height {
                                        NumberAnimation { duration: 200; easing.type: Easing.OutQuad }
                                    }
                                    Behavior on opacity {
                                        NumberAnimation { duration: 150 }
                                    }
                                }

                                property var contact: filteredContacts[index]

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.leftMargin: 12
                                    anchors.rightMargin: 12
                                    spacing: 10

                                    // 头像
                                    Rectangle {
                                        Layout.preferredWidth: 40
                                        Layout.preferredHeight: 40
                                        radius: 20
                                        color: "#e8f1ff"
                                        clip: true

                                        Image {
                                            anchors.centerIn: parent
                                            source: contact.avatar || "qrc:/new/prefix1/image/boy.png"
                                            width: 36
                                            height: 36
                                            fillMode: Image.PreserveAspectCrop
                                        }
                                    }

                                    // 用户信息
                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 4

                                        Text {
                                            text: contact.name
                                            font.pixelSize: 14
                                            font.weight: Font.Medium
                                            color: "#333"
                                            font.family: "Microsoft YaHei, SimSun, sans-serif"
                                            elide: Text.ElideRight
                                            Layout.fillWidth: true
                                        }

                                        RowLayout {
                                            spacing: 4

                                            Rectangle {
                                                width: 7
                                                height: 7
                                                radius: 3.5
                                                color: contact.status === "在线" ? "#4caf50" :
                                                                                 contact.status === "忙碌" ? "#ff9800" : "#bbb"
                                            }

                                            Text {
                                                text: contact.status
                                                font.pixelSize: 11
                                                color: "#999"
                                                font.family: "Microsoft YaHei, SimSun, sans-serif"
                                            }
                                        }
                                    }
                                }

                                MouseArea {
                                    id: contactMouseArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: {
                                        selectedContactIndex = index
                                    }
                                }

                                // 底部分隔线
                                Rectangle {
                                    anchors.bottom: parent.bottom
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.leftMargin: 62
                                    height: 1
                                    color: "#f0f0f0"
                                }
                            }
                        }
                    }
                }
            }

        }

        // ========== 中间分割线 ==========
        Rectangle {
            Layout.preferredWidth: 1
            Layout.fillHeight: true
            color: "#e0e0e0"
        }

        // ========== 右侧面板：联系人详情 ==========
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#FAFAFA"

            // 未选中时的占位提示
            Item {
                anchors.fill: parent
                visible: selectedContactIndex === -1

                Column {
                    anchors.centerIn: parent
                    spacing: 16

                    Text {
                        text: "👥"
                        font.pixelSize: 64
                        color: "#e0e0e0"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    Text {
                        text: "请选择一个联系人查看详情"
                        font.pixelSize: 16
                        color: "#999999"
                        font.family: "Microsoft YaHei, SimSun, sans-serif"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }

            // 好友申请管理面板
            Item {
                anchors.fill: parent
                visible: selectedContactIndex === -2

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 40
                    spacing: 24

                    Text {
                        text: "好友申请列表"
                        font.pixelSize: 24; font.weight: Font.Bold; color: "#1e293b"
                    }

                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: pendingRequests
                        spacing: 12
                        clip: true

                        delegate: Rectangle {
                            id: requestDelegate
                            width: parent.width
                            height: 80
                            color: "white"
                            radius: 12
                            border.color: "#f1f5f9"

                            property var requestData: modelData
                            property string actionStatus: requestData.status || "pending" // "accepted", "rejected", or "pending"

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 16
                                spacing: 16

                                Rectangle {
                                    width: 48; height: 48; radius: 24; color: "#eff6ff"
                                    Text { anchors.centerIn: parent; text: "👤"; font.pixelSize: 24 }
                                }

                                ColumnLayout {
                                    Layout.fillWidth: true; spacing: 2
                                    Text { text: modelData.fromUsername; font.pixelSize: 16; font.weight: Font.Bold }
                                    Text {
                                        text: requestDelegate.actionStatus === "accepted" ? "已同意" :
                                                                                            (requestDelegate.actionStatus === "rejected" ? "已忽略" : "申请加为好友")
                                        font.pixelSize: 13
                                        color: requestDelegate.actionStatus === "accepted" ? "#10b981" :
                                                                                             (requestDelegate.actionStatus === "rejected" ? "#64748b" : "#64748b")
                                    }
                                }

                                RowLayout {
                                    spacing: 10
                                    visible: requestDelegate.actionStatus === "pending"

                                    // 忽略按钮
                                    Rectangle {
                                        id: ignoreBtn
                                        implicitWidth: 70
                                        implicitHeight: 36
                                        radius: 10
                                        color: ignoreBtnMA.containsMouse ? "#f1f5f9" : "#f8fafc"
                                        border.color: "#e2e8f0"
                                        border.width: 1

                                        Text {
                                            anchors.centerIn: parent
                                            text: "忽略"
                                            font.pixelSize: 13
                                            font.weight: Font.Medium
                                            color: ignoreBtnMA.containsMouse ? "#64748b" : "#94a3b8"
                                        }

                                        MouseArea {
                                            id: ignoreBtnMA
                                            anchors.fill: parent
                                            hoverEnabled: true
                                            cursorShape: Qt.PointingHandCursor
                                            onClicked: {
                                                ChatService.respondToFriendRequest(modelData.requestId, false)
                                                requestDelegate.actionStatus = "rejected"
                                            }
                                        }

                                        Behavior on color { ColorAnimation { duration: 200 } }
                                        scale: ignoreBtnMA.pressed ? 0.95 : 1.0
                                        Behavior on scale { NumberAnimation { duration: 150 } }
                                    }

                                    // 通过按钮（渐变样式）
                                    Rectangle {
                                        id: acceptBtn
                                        implicitWidth: 70
                                        implicitHeight: 36
                                        radius: 10
                                        gradient: Gradient {
                                            orientation: Gradient.Horizontal
                                            GradientStop { position: 0.0; color: acceptBtnMA.pressed ? "#10b981" : "#34d399" }
                                            GradientStop { position: 1.0; color: acceptBtnMA.pressed ? "#059669" : "#6ee7b7" }
                                        }

                                        Text {
                                            anchors.centerIn: parent
                                            text: "通过"
                                            font.pixelSize: 13
                                            font.weight: Font.Bold
                                            color: "white"
                                        }

                                        MouseArea {
                                            id: acceptBtnMA
                                            anchors.fill: parent
                                            hoverEnabled: true
                                            cursorShape: Qt.PointingHandCursor
                                            onClicked: {
                                                ChatService.respondToFriendRequest(modelData.requestId, true)
                                                requestDelegate.actionStatus = "accepted"
                                            }
                                        }

                                        scale: acceptBtnMA.pressed ? 0.95 : (acceptBtnMA.containsMouse ? 1.05 : 1.0)
                                        Behavior on scale { NumberAnimation { duration: 200; easing.type: Easing.OutBack } }

                                        layer.enabled: true
                                        layer.effect: DropShadow {
                                            transparentBorder: true
                                            color: "#4034d399"
                                            radius: 12
                                            samples: 17
                                            verticalOffset: 3
                                        }
                                    }
                                }

                                // 已处理状态图标
                                Text {
                                    visible: requestDelegate.actionStatus !== "pending"
                                    text: requestDelegate.actionStatus === "accepted" ? "✓" : "×"
                                    font.pixelSize: 24
                                    color: requestDelegate.actionStatus === "accepted" ? "#10b981" : "#64748b"
                                }
                            }
                        }
                    }
                }
            }

            // 选中联系人时显示详情
            Item {
                anchors.fill: parent
                visible: selectedContactIndex >= 0 && selectedContactIndex < filteredContacts.length

                property var selectedContact: (selectedContactIndex >= 0 && selectedContactIndex < filteredContacts.length) ? filteredContacts[selectedContactIndex] : null

                Flickable {
                    anchors.fill: parent
                    contentHeight: detailColumn.height + 40
                    clip: true
                    boundsBehavior: Flickable.StopAtBounds

                    Column {
                        id: detailColumn
                        width: parent.width
                        spacing: 0

                        // 顶部个人信息卡片
                        Rectangle {
                            width: parent.width
                            height: 140
                            color: "white"

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 20
                                spacing: 12

                                // 大头像
                                Rectangle {
                                    Layout.preferredWidth: 80
                                    Layout.preferredHeight: 80
                                    radius: 40
                                    color: "#e8f1ff"
                                    border.color: "#d0e0ff"
                                    border.width: 2
                                    clip: true

                                    Image {
                                        anchors.centerIn: parent
                                        source: selectedContactIndex >= 0 && selectedContactIndex < filteredContacts.length ? (filteredContacts[selectedContactIndex].avatar || "qrc:/new/prefix1/image/boy.png") : ""
                                        width: 76
                                        height: 76
                                        fillMode: Image.PreserveAspectCrop
                                    }
                                }

                                // 用户信息
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 4

                                    // 名称和性别
                                    RowLayout {
                                        spacing: 6

                                        Text {
                                            text: selectedContactIndex >= 0 && selectedContactIndex < filteredContacts.length ? filteredContacts[selectedContactIndex].name : ""
                                            font.pixelSize: 18
                                            font.weight: Font.Bold
                                            color: "#222"
                                            font.family: "Microsoft YaHei, SimSun, sans-serif"
                                        }

                                        // 性别标识
                                        Text {
                                            text: (selectedContactIndex >= 0 && selectedContactIndex < filteredContacts.length && filteredContacts[selectedContactIndex].isMale) ? "♂" : "♀"
                                            font.pixelSize: 16
                                            font.weight: Font.Bold
                                            color: (selectedContactIndex >= 0 && selectedContactIndex < filteredContacts.length && filteredContacts[selectedContactIndex].isMale) ? "#3B82F6" : "#EC4899"
                                        }
                                    }

                                    // ID
                                    Text {
                                        text: selectedContactIndex >= 0 && selectedContactIndex < filteredContacts.length ? "ID: " + filteredContacts[selectedContactIndex].userId : ""
                                        font.pixelSize: 11
                                        color: "#888"
                                        font.family: "Microsoft YaHei, SimSun, sans-serif"
                                    }

                                    // 年龄和地区
                                    RowLayout {
                                        spacing: 12

                                        RowLayout {
                                            spacing: 3
                                            Text {
                                                text: "🎂"
                                                font.pixelSize: 13
                                            }
                                            Text {
                                                text: (selectedContactIndex >= 0 && selectedContactIndex < filteredContacts.length ? filteredContacts[selectedContactIndex].age : "") + "岁"
                                                font.pixelSize: 12
                                                color: "#666"
                                                font.family: "Microsoft YaHei, SimSun, sans-serif"
                                            }
                                        }

                                        RowLayout {
                                            spacing: 3
                                            Text {
                                                text: "📍"
                                                font.pixelSize: 13
                                            }
                                            Text {
                                                text: selectedContactIndex >= 0 && selectedContactIndex < filteredContacts.length ? filteredContacts[selectedContactIndex].region : ""
                                                font.pixelSize: 12
                                                color: "#666"
                                                font.family: "Microsoft YaHei, SimSun, sans-serif"
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        // 分隔线
                        Rectangle {
                            width: parent.width
                            height: 1
                            color: "#eee"
                        }

                        // 签名信息区域
                        Rectangle {
                            width: parent.width
                            height: 60
                            color: "white"

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 30
                                anchors.rightMargin: 30
                                spacing: 12

                                Text {
                                    text: "✍"
                                    font.pixelSize: 18
                                    color: "#999"
                                }

                                Text {
                                    text: "签名"
                                    font.pixelSize: 13
                                    color: "#999"
                                    font.family: "Microsoft YaHei, SimSun, sans-serif"
                                    Layout.preferredWidth: 50
                                }

                                Text {
                                    text: selectedContactIndex >= 0 && selectedContactIndex < filteredContacts.length ? filteredContacts[selectedContactIndex].signature : ""
                                    font.pixelSize: 13
                                    color: "#555"
                                    font.family: "Microsoft YaHei, SimSun, sans-serif"
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                }
                            }
                        }

                        // 分隔线
                        Rectangle {
                            width: parent.width
                            height: 1
                            color: "#eee"
                        }

                        // 操作按钮区域
                        Rectangle {
                            width: parent.width
                            height: 80
                            color: "white"

                            Row {
                                anchors.centerIn: parent
                                spacing: 20

                                // 编辑资料按钮
                                Rectangle {
                                    width: 120
                                    height: 36
                                    radius: 6
                                    color: editArea.containsMouse ? "#e8e8e8" : "#f0f0f0"
                                    border.color: "#d0d0d0"
                                    border.width: 1

                                    Behavior on color {
                                        ColorAnimation { duration: 120 }
                                    }

                                    Text {
                                        anchors.centerIn: parent
                                        text: "编辑资料"
                                        font.pixelSize: 13
                                        color: "#555"
                                        font.family: "Microsoft YaHei, SimSun, sans-serif"
                                    }

                                    MouseArea {
                                        id: editArea
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: {
                                            console.log("编辑联系人资料:", selectedContactIndex >= 0 ? contacts[selectedContactIndex].name : "")
                                        }
                                    }
                                }

                                // 发消息按钮
                                Rectangle {
                                    width: 120
                                    height: 36
                                    radius: 6
                                    color: sendMsgArea.containsMouse ? "#2680EB" : "#3B82F6"

                                    Behavior on color {
                                        ColorAnimation { duration: 120 }
                                    }

                                    Text {
                                        anchors.centerIn: parent
                                        text: "发消息"
                                        font.pixelSize: 13
                                        color: "white"
                                        font.weight: Font.Medium
                                        font.family: "Microsoft YaHei, SimSun, sans-serif"
                                    }

                                    MouseArea {
                                        id: sendMsgArea
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: {
                                            if (selectedContactIndex >= 0 && selectedContactIndex < contacts.length) {
                                                var c = contacts[selectedContactIndex]
                                                contactsPage.contactSelected(c.id, c.name)
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
