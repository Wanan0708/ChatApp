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
        console.log("[ContactsPage] Component completed, refreshing data...")
        refreshRequests()
        refreshFriends()
    }

    function refreshRequests() {
        ChatService.getPendingFriendRequests()
    }

    function refreshFriends() {
        console.log("[ContactsPage] refreshFriends called")
        ChatService.getFriendList()
    }
    
    function handleFriendsLoaded(friends) {
        console.log("[ContactsPage] handleFriendsLoaded called, friends count:", friends.length)
        let newContacts = []
        for (let i = 0; i < friends.length; i++) {
            // 从后端获取头像数据，如果没有则使用默认头像
            let avatar = friends[i].avatar || "qrc:/new/prefix1/image/boy.png"
            let name = friends[i].username || "未知用户"
            let firstChar = name.charAt(0).toUpperCase()
            // 简单的首字母逻辑：如果首个字符是英文字母则使用，否则统一归入 "#"
            let initial = /^[A-Z]$/.test(firstChar) ? firstChar : "#"

            console.log("[ContactsPage] Friend", i, ":", {
                userId: friends[i].userId,
                username: friends[i].username,
                avatar: friends[i].avatar,
                isMale: friends[i].isMale,
                age: friends[i].age,
                region: friends[i].region,
                status: friends[i].status,
                signature: friends[i].signature
            })

            newContacts.push({
                                 id: friends[i].userId || "",
                                 name: name,
                                 initial: initial,
                                 avatar: avatar,
                                 isMale: friends[i].isMale !== undefined ? friends[i].isMale : true,
                                 age: friends[i].age || 25,
                                 region: friends[i].region || "",
                                 status: friends[i].status || "在线",
                                 signature: friends[i].signature || "",
                                 userId: friends[i].userId || ""
                             })
        }

        // 按照首字母排序 (A-Z, 然后是 #)
        newContacts.sort(function(a, b) {
            if (a.initial === b.initial) {
                return a.name < b.name ? -1 : (a.name > b.name ? 1 : 0)
            }
            if (a.initial === "#") return 1
            if (b.initial === "#") return -1
            return a.initial < b.initial ? -1 : 1
        })

        contacts = newContacts
        contactsVersion++  // 触发 filteredContacts 更新
        console.log("[ContactsPage] contacts updated, count:", contacts.length, "version:", contactsVersion)
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
            console.log("[ContactsPage] onPendingRequestsLoaded, count:", requests.length)
            pendingRequests = requests
        }
        onFriendListLoaded: {
            console.log("[ContactsPage] onFriendListLoaded signal received, count:", friends.length)
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

    // 联系人列表数据
    property var contacts: []
    // 用于触发 filteredContacts 更新的辅助属性
    property int contactsVersion: 0

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
        console.log("[ContactsPage] filteredContacts computed, count:", result.length, "searchText:", searchText)
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

                // 联系人列表（不用 ScrollView 包裹 ColumnLayout，否则 fillHeight 失效）
                // 好友申请条目（固定高度）
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

                // 联系人列表容器（填满剩余高度）
                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    ListView {
                        id: contactsList
                        anchors.fill: parent
                        anchors.rightMargin: 16
                        model: contactsPage.filteredContacts
                        spacing: 0
                        currentIndex: selectedContactIndex
                        clip: true

                        section.property: "initial"
                        section.criteria: ViewSection.FullString
                        section.delegate: Rectangle {
                            width: contactsList.width
                            height: 24
                            color: "#f8fafc"
                            border.color: "#e2e8f0"
                            border.width: 1

                            Text {
                                anchors.left: parent.left
                                anchors.leftMargin: 12
                                anchors.verticalCenter: parent.verticalCenter
                                text: section
                                font.pixelSize: 12
                                font.weight: Font.DemiBold
                                color: "#64748b"
                            }
                        }

                        delegate: Rectangle {
                            width: contactsList.width
                            height: 56
                            color: {
                                if (index === selectedContactIndex) return "#EFF6FF"
                                if (contactMouseArea.containsMouse) return "#f5f5f5"
                                return "white"
                            }

                            Behavior on color { ColorAnimation { duration: 120 } }

                            // 选中指示条
                            Rectangle {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                width: 3
                                height: index === selectedContactIndex ? 28 : 0
                                radius: 2
                                color: "#3B82F6"
                                opacity: index === selectedContactIndex ? 1 : 0

                                Behavior on height { NumberAnimation { duration: 200; easing.type: Easing.OutQuad } }
                                Behavior on opacity { NumberAnimation { duration: 150 } }
                            }

                            property var contact: modelData

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 12
                                anchors.rightMargin: 12
                                spacing: 10

                                // 头像
                                Rectangle {
                                    Layout.preferredWidth: 40; Layout.preferredHeight: 40; radius: 20; color: "#e8f1ff"; clip: true
                                    Image {
                                        anchors.centerIn: parent
                                        source: contact.avatar || "qrc:/new/prefix1/image/boy.png"
                                        width: 36; height: 36; fillMode: Image.PreserveAspectCrop
                                    }
                                }

                                // 用户信息
                                ColumnLayout {
                                    Layout.fillWidth: true; spacing: 4
                                    Text {
                                        text: contact.name; font.pixelSize: 14; font.weight: Font.Medium; color: "#333"
                                        font.family: "Microsoft YaHei, SimSun, sans-serif"; elide: Text.ElideRight; Layout.fillWidth: true
                                    }
                                    RowLayout {
                                        spacing: 4
                                        Rectangle {
                                            width: 7; height: 7; radius: 3.5
                                            color: contact.status === "在线" ? "#4caf50" : (contact.status === "忙碌" ? "#ff9800" : "#bbb")
                                        }
                                        Text { text: contact.status; font.pixelSize: 11; color: "#999"; font.family: "Microsoft YaHei, SimSun, sans-serif" }
                                    }
                                }
                            }

                            MouseArea {
                                id: contactMouseArea
                                anchors.fill: parent
                                hoverEnabled: true; cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    console.log("[ContactsPage] Contact clicked:", contact.name, "index:", index)
                                    selectedContactIndex = index
                                }
                            }

                            // 底部分隔线
                            Rectangle {
                                anchors.bottom: parent.bottom; anchors.left: parent.left; anchors.right: parent.right
                                anchors.leftMargin: 62; height: 1; color: "#f0f0f0"
                            }
                        }
                    }

                    // A-Z 侧边快速导航
                    Column {
                        anchors.right: parent.right
                        anchors.rightMargin: 2
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 2

                        Repeater {
                            model: ["A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","#"]
                            delegate: Text {
                                text: modelData
                                font.pixelSize: 10
                                font.weight: Font.Medium
                                color: letterMA.containsMouse ? "#3b82f6" : "#94a3b8"

                                MouseArea {
                                    id: letterMA
                                    anchors.fill: parent
                                    anchors.margins: -4
                                    hoverEnabled: true
                                    onClicked: {
                                        let targetInitial = modelData
                                        let targetIndex = -1
                                        let list = contactsPage.filteredContacts
                                        for (let i = 0; i < list.length; i++) {
                                            if (list[i].initial === targetInitial) {
                                                targetIndex = i
                                                break
                                            }
                                        }
                                        if (targetIndex >= 0) {
                                            contactsList.positionViewAtIndex(targetIndex, ListView.Beginning)
                                        }
                                    }
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
                            property bool isHidden: false

                            opacity: isHidden ? 0 : 1
                            Behavior on opacity { NumberAnimation { duration: 300 } }
                            Behavior on height { NumberAnimation { duration: 300; easing.type: Easing.InOutQuad } }
                            visible: height > 0

                            Timer {
                                id: hideTimer
                                interval: 800
                                onTriggered: requestDelegate.isHidden = true
                            }

                            onActionStatusChanged: {
                                if (actionStatus !== "pending") {
                                    hideTimer.start()
                                }
                            }

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
                id: detailItem
                anchors.fill: parent
                visible: selectedContactIndex >= 0 && selectedContactIndex < filteredContacts.length

                property var selectedContact: null

                // 使用 Connections 监听父级属性变化
                Connections {
                    target: contactsPage

                    function onSelectedContactIndexChanged() {
                        detailItem.updateSelectedContact()
                    }

                    function onContactsVersionChanged() {
                        detailItem.updateSelectedContact()
                    }
                }

                function updateSelectedContact() {
                    if (selectedContactIndex >= 0 && selectedContactIndex < filteredContacts.length) {
                        selectedContact = filteredContacts[selectedContactIndex]
                        console.log("[ContactsPage] selectedContact updated:", {
                            id: selectedContact.id,
                            name: selectedContact.name,
                            avatar: selectedContact.avatar,
                            isMale: selectedContact.isMale,
                            age: selectedContact.age,
                            region: selectedContact.region,
                            signature: selectedContact.signature,
                            status: selectedContact.status
                        })
                    } else {
                        selectedContact = null
                    }
                }

                // 初始化时更新一次
                Component.onCompleted: {
                    detailItem.updateSelectedContact()
                }

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
                                        source: detailItem.selectedContact ? (detailItem.selectedContact.avatar || "qrc:/new/prefix1/image/boy.png") : ""
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
                                            text: detailItem.selectedContact ? detailItem.selectedContact.name : ""
                                            font.pixelSize: 18
                                            font.weight: Font.Bold
                                            color: "#222"
                                            font.family: "Microsoft YaHei, SimSun, sans-serif"
                                        }

                                        // 性别标识
                                        Text {
                                            text: detailItem.selectedContact ? (detailItem.selectedContact.isMale ? "♂" : "♀") : ""
                                            font.pixelSize: 16
                                            font.weight: Font.Bold
                                            color: detailItem.selectedContact ? (detailItem.selectedContact.isMale ? "#3B82F6" : "#EC4899") : "#888"
                                        }
                                    }

                                    // ID
                                    Text {
                                        text: detailItem.selectedContact ? ("ID: " + detailItem.selectedContact.id) : ""
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
                                                text: (detailItem.selectedContact ? detailItem.selectedContact.age : 0) + "岁"
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
                                                text: detailItem.selectedContact ? detailItem.selectedContact.region : ""
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
                                    text: detailItem.selectedContact ? detailItem.selectedContact.signature : ""
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

                        // 丰富的详情展示区域
                        Rectangle {
                            width: parent.width
                            height: 180
                            color: "white"

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 30
                                spacing: 16

                                // 在线状态
                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 12
                                    Text { text: "状态"; color: "#999"; font.pixelSize: 13; Layout.preferredWidth: 60 }
                                    RowLayout {
                                        spacing: 6
                                        Rectangle {
                                            width: 8; height: 8; radius: 4
                                            color: detailItem.selectedContact && detailItem.selectedContact.status === "在线" ? "#4caf50" : (detailItem.selectedContact && detailItem.selectedContact.status === "忙碌" ? "#ff9800" : "#bbb")
                                        }
                                        Text { text: detailItem.selectedContact ? detailItem.selectedContact.status : ""; color: "#333"; font.pixelSize: 13; font.weight: Font.Medium }
                                    }
                                    Item { Layout.fillWidth: true }
                                }

                                Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: "#f5f5f5" }

                                // 年龄信息
                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 12
                                    Text { text: "年龄"; color: "#999"; font.pixelSize: 13; Layout.preferredWidth: 60 }
                                    Text { text: (detailItem.selectedContact && detailItem.selectedContact.age > 0) ? (detailItem.selectedContact.age + " 岁") : "未设置"; color: "#333"; font.pixelSize: 13 }
                                    Item { Layout.fillWidth: true }
                                }

                                Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: "#f5f5f5" }

                                // 地区信息
                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 12
                                    Text { text: "地区"; color: "#999"; font.pixelSize: 13; Layout.preferredWidth: 60 }
                                    Text { text: (detailItem.selectedContact && detailItem.selectedContact.region) ? detailItem.selectedContact.region : "未设置"; color: "#333"; font.pixelSize: 13 }
                                    Item { Layout.fillWidth: true }
                                }

                                Item { Layout.fillHeight: true }
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

                                // 发消息按钮
                                Rectangle {
                                    width: 140
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
                                            if (detailItem.selectedContact && detailItem.selectedContact.id !== "" && detailItem.selectedContact.name !== "") {
                                                contactsPage.contactSelected(detailItem.selectedContact.id, detailItem.selectedContact.name)
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
