import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12
import ChatApp.Network 1.0

Popup {
    id: profilePanel
    width: Math.max(700, Math.min(900, (Overlay.overlay ? Overlay.overlay.width : 900) * 0.9))
    height: Math.max(520, Math.min(680, (Overlay.overlay ? Overlay.overlay.height : 680) * 0.9))
    modal: true
    focus: true
    closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape
    parent: Overlay.overlay
    anchors.centerIn: Overlay.overlay

    property var openChangePasswordDialog: null
    property var openLogoutDialog: null

    property string draftAvatar: ""
    property string draftName: ""
    property string draftGender: ""
    property string draftRegion: ""
    property string draftPhone: ""
    property string draftEmail: ""
    property string draftContact: ""
    property string draftBio: ""
    property int draftAge: 25
    property string saveHint: ""
    property bool saveSuccess: false
    property bool isEditing: false  // 新增：编辑状态标记
    property bool isUploadingAvatar: false
    property int avatarUploadProgress: 0
    property string avatarHint: ""
    property bool avatarUploadSuccess: false

    function syncDraftFromService() {
        draftAvatar = ChatService.currentUserAvatar || ""
        draftName = ChatService.currentUserName || ""
        draftGender = ChatService.currentUserGender || ""
        draftRegion = ChatService.currentUserRegion || ""
        draftPhone = ChatService.currentUserPhone || ""
        draftEmail = ChatService.currentUserEmail || ""
        draftContact = ChatService.currentUserContact || ""
        draftBio = ChatService.currentUserBio || ""
        draftAge = ChatService.currentUserAge || 25
    }

    function saveProfile() {
        if (isUploadingAvatar) {
            saveSuccess = false
            saveHint = "头像上传中，请稍候"
            return
        }

        if (draftAvatar.indexOf("file:///") === 0) {
            saveSuccess = false
            saveHint = "请等待头像上传成功后再保存"
            return
        }

        ChatService.saveUserProfile({
                                        avatar: draftAvatar,
                                        username: draftName,
                                        gender: draftGender,
                                        region: draftRegion,
                                        phone: draftPhone,
                                        email: draftEmail,
                                        contact: draftContact,
                                        bio: draftBio,
                                        age: draftAge
                                    })
    }

    function localFileUrl(filePath) {
        if (!filePath || filePath === "") {
            return ""
        }

        if (filePath.indexOf("file:///") === 0 || filePath.indexOf("qrc:") === 0 || filePath.indexOf("http://") === 0 || filePath.indexOf("https://") === 0) {
            return filePath
        }

        return "file:///" + filePath.replace(/\\/g, "/")
    }

    function selectLocalAvatar() {
        var avatarPath = ChatService.pickLocalFile(true)
        if (!avatarPath || avatarPath.length === 0) {
            return
        }

        draftAvatar = localFileUrl(avatarPath)
        isUploadingAvatar = true
        avatarUploadProgress = 0
        avatarUploadSuccess = false
        avatarHint = "头像上传中..."
        ChatService.uploadAvatar(avatarPath)
    }

    onOpened: {
        isEditing = false
        isUploadingAvatar = false
        avatarUploadProgress = 0
        avatarHint = ""
        syncDraftFromService()
        saveHint = ""
        // 每次打开时重新加载用户资料，确保显示最新数据
        ChatService.loadUserProfile()
        console.log("[ProfilePanel] Opened, loading user profile...")
    }

    Overlay.modal: Rectangle { color: "#4d000000" }
    background: Rectangle { color: "transparent" }

    Rectangle {
        anchors.fill: parent
        radius: 10
        color: "#f6f7f9"
        border.color: "#dfe3e8"
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 14
            spacing: 10

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 46
                radius: 8
                color: "#ffffff"
                border.color: "#e4e8ee"

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 12
                    anchors.rightMargin: 8
                    Text {
                        text: "个人信息"
                        color: "#111827"
                        font.pixelSize: 18
                        font.weight: Font.DemiBold
                    }
                    Item { Layout.fillWidth: true }
                    ToolButton {
                        onClicked: profilePanel.close()
                        contentItem: Text {
                            text: "×"
                            color: "#6b7280"
                            font.pixelSize: 18
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle { radius: 6; color: parent.hovered ? "#f3f4f6" : "transparent" }
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 10

                Rectangle {
                    Layout.preferredWidth: Math.max(220, Math.min(280, profilePanel.width * 0.30))
                    Layout.fillHeight: true
                    radius: 8
                    color: "#ffffff"
                    border.color: "#e4e8ee"

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 8

                        Rectangle {
                            Layout.alignment: Qt.AlignHCenter
                            width: 88
                            height: 88
                            radius: 44
                            color: "#ffffff"
                            border.color: isEditing ? "#60a5fa" : "#d1d5db"
                            border.width: isEditing ? 2 : 1

                            Avatar {
                                id: profileAvatar
                                anchors.centerIn: parent
                                width: 80
                                height: 80
                                isSelf: true
                                avatarSource: draftAvatar || "qrc:/new/prefix1/image/boy.png"
                            }

                            Rectangle {
                                anchors.fill: parent
                                radius: parent.radius
                                color: isUploadingAvatar ? "#66000000" : (avatarMouseArea.containsMouse && isEditing ? "#55000000" : "transparent")
                                visible: isUploadingAvatar || (avatarMouseArea.containsMouse && isEditing)

                                Column {
                                    anchors.centerIn: parent
                                    spacing: 4

                                    BusyIndicator {
                                        anchors.horizontalCenter: parent.horizontalCenter
                                        running: isUploadingAvatar
                                        visible: running
                                        width: 24
                                        height: 24
                                    }

                                    Text {
                                        text: isUploadingAvatar ? "上传中" : "点击更换"
                                        color: "white"
                                        font.pixelSize: 11
                                        anchors.horizontalCenter: parent.horizontalCenter
                                    }
                                }
                            }

                            MouseArea {
                                id: avatarMouseArea
                                anchors.fill: parent
                                enabled: isEditing && !isUploadingAvatar
                                hoverEnabled: true
                                cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                                onClicked: selectLocalAvatar()
                            }
                        }

                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            visible: isEditing
                            text: isUploadingAvatar ? "正在处理头像..." : "点击头像可直接更换"
                            color: "#6b7280"
                            font.pixelSize: 11
                        }

                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            text: draftName !== "" ? draftName : "未登录"
                            color: "#111827"
                            font.pixelSize: 18
                            font.weight: Font.Medium
                        }
                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            text: "ID: " + (ChatService.currentUserId || "N/A")
                            color: "#6b7280"
                            font.pixelSize: 12
                        }

                        Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: "#edf1f5" }

                        GridLayout {
                            Layout.fillWidth: true
                            columns: 2
                            rowSpacing: 6
                            columnSpacing: 6
                            Text { text: "性别"; color: "#6b7280"; font.pixelSize: 12 }
                            Text { text: draftGender === "" ? "未设置" : draftGender; color: "#111827"; font.pixelSize: 12; horizontalAlignment: Text.AlignRight; Layout.fillWidth: true }
                            Text { text: "地区"; color: "#6b7280"; font.pixelSize: 12 }
                            Text { text: draftRegion === "" ? "未设置" : draftRegion; color: "#111827"; font.pixelSize: 12; horizontalAlignment: Text.AlignRight; Layout.fillWidth: true }
                            Text { text: "联系"; color: "#6b7280"; font.pixelSize: 12 }
                            Text { text: draftContact === "" ? "未设置" : draftContact; color: "#111827"; font.pixelSize: 12; horizontalAlignment: Text.AlignRight; Layout.fillWidth: true }
                        }

                        Item { Layout.fillHeight: true }

                        Button {
                            text: "修改密码"
                            Layout.fillWidth: true
                            Layout.preferredHeight: 36
                            onClicked: {
                                profilePanel.close()
                                if (profilePanel.openChangePasswordDialog) profilePanel.openChangePasswordDialog()
                            }
                        }

                        Button {
                            text: "退出登录"
                            Layout.fillWidth: true
                            Layout.preferredHeight: 36
                            onClicked: {
                                profilePanel.close()
                                if (profilePanel.openLogoutDialog) profilePanel.openLogoutDialog()
                            }
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    radius: 8
                    color: "#ffffff"
                    border.color: "#e4e8ee"

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 8

                        Text {
                            text: "资料编辑"
                            color: "#111827"
                            font.pixelSize: 15
                            font.weight: Font.DemiBold
                        }

                        ScrollView {
                            id: editScroll
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            clip: true

                            ColumnLayout {
                                width: editScroll.availableWidth
                                spacing: 12

                                // 只读展示模式
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 12
                                    visible: !isEditing

                                    RowLayout {
                                        spacing: 8
                                        Text { text: "昵称:"; color: "#6b7280"; font.pixelSize: 14; Layout.preferredWidth: 60 }
                                        Text { text: draftName || "未设置"; color: "#111827"; font.pixelSize: 14; font.weight: Font.Medium; Layout.fillWidth: true }
                                    }

                                    RowLayout {
                                        spacing: 8
                                        Text { text: "性别:"; color: "#6b7280"; font.pixelSize: 14; Layout.preferredWidth: 60 }
                                        Text { text: draftGender || "未设置"; color: "#111827"; font.pixelSize: 14; font.weight: Font.Medium; Layout.fillWidth: true }
                                    }

                                    RowLayout {
                                        spacing: 8
                                        Text { text: "地区:"; color: "#6b7280"; font.pixelSize: 14; Layout.preferredWidth: 60 }
                                        Text { text: draftRegion || "未设置"; color: "#111827"; font.pixelSize: 14; font.weight: Font.Medium; Layout.fillWidth: true }
                                    }

                                    RowLayout {
                                        spacing: 8
                                        Text { text: "年龄:"; color: "#6b7280"; font.pixelSize: 14; Layout.preferredWidth: 60 }
                                        Text { text: draftAge > 0 ? (draftAge + " 岁") : "未设置"; color: "#111827"; font.pixelSize: 14; font.weight: Font.Medium; Layout.fillWidth: true }
                                    }

                                    RowLayout {
                                        spacing: 8
                                        Text { text: "手机号:"; color: "#6b7280"; font.pixelSize: 14; Layout.preferredWidth: 60 }
                                        Text { text: draftPhone || "未设置"; color: "#111827"; font.pixelSize: 14; font.weight: Font.Medium; Layout.fillWidth: true }
                                    }

                                    RowLayout {
                                        spacing: 8
                                        Text { text: "邮箱:"; color: "#6b7280"; font.pixelSize: 14; Layout.preferredWidth: 60 }
                                        Text { text: draftEmail || "未设置"; color: "#111827"; font.pixelSize: 14; font.weight: Font.Medium; Layout.fillWidth: true }
                                    }

                                    RowLayout {
                                        spacing: 8
                                        Text { text: "联系方式:"; color: "#6b7280"; font.pixelSize: 14; Layout.preferredWidth: 60 }
                                        Text { text: draftContact || "未设置"; color: "#111827"; font.pixelSize: 14; font.weight: Font.Medium; Layout.fillWidth: true }
                                    }

                                    ColumnLayout {
                                        spacing: 4
                                        Layout.fillWidth: true
                                        Text { text: "个人简介:"; color: "#6b7280"; font.pixelSize: 14 }
                                        Rectangle {
                                            Layout.fillWidth: true
                                            Layout.preferredHeight: Math.max(60, bioText.implicitHeight + 16)
                                            color: "#f9fafb"
                                            radius: 6
                                            border.color: "#f3f4f6"
                                            Text {
                                                id: bioText
                                                anchors.fill: parent
                                                anchors.margins: 8
                                                text: draftBio || "这个人很懒，什么都没留下~"
                                                color: "#4b5563"
                                                font.pixelSize: 13
                                                wrapMode: Text.Wrap
                                                lineHeight: 1.4
                                            }
                                        }
                                    }
                                }

                                // 编辑模式
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 8
                                    visible: isEditing

                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 8
                                        TextField {
                                            Layout.fillWidth: true
                                            placeholderText: "头像 URL"
                                            text: draftAvatar
                                            onTextChanged: draftAvatar = text
                                            selectByMouse: true
                                        }
                                        Button {
                                            text: "选择本地图片"
                                            enabled: !isUploadingAvatar
                                            onClicked: selectLocalAvatar()
                                        }
                                    }

                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 8
                                        visible: isUploadingAvatar || avatarHint !== ""

                                        ProgressBar {
                                            Layout.fillWidth: true
                                            from: 0
                                            to: 100
                                            value: avatarUploadProgress
                                            visible: isUploadingAvatar
                                        }

                                        Text {
                                            Layout.fillWidth: true
                                            text: avatarHint
                                            color: avatarUploadSuccess ? "#166534" : (isUploadingAvatar ? "#1d4ed8" : "#991b1b")
                                            font.pixelSize: 12
                                            elide: Text.ElideRight
                                        }
                                    }
                                    
                                    TextField {
                                        Layout.fillWidth: true
                                        placeholderText: "输入头像图片远程URL (如: https://...)"
                                        text: draftAvatar
                                        onTextChanged: draftAvatar = text
                                        selectByMouse: true
                                    }

                                    TextField { Layout.fillWidth: true; placeholderText: "用户名"; text: draftName; onTextChanged: draftName = text; selectByMouse: true }

                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 8
                                        ComboBox {
                                            Layout.preferredWidth: 160
                                            model: ["", "男", "女", "其他", "不方便透露"]
                                            currentIndex: {
                                                var idx = model.indexOf(draftGender)
                                                return idx >= 0 ? idx : 0
                                            }
                                            onActivated: draftGender = currentText
                                        }
                                        TextField { Layout.fillWidth: true; placeholderText: "地区"; text: draftRegion; onTextChanged: draftRegion = text; selectByMouse: true }
                                    }

                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 8
                                        SpinBox {
                                            Layout.preferredWidth: 160
                                            from: 1
                                            to: 150
                                            value: draftAge > 0 ? draftAge : 25
                                            onValueModified: draftAge = value
                                            textFromValue: function(value) { return value + " 岁"; }
                                        }
                                        TextField { Layout.fillWidth: true; placeholderText: "手机号"; text: draftPhone; onTextChanged: draftPhone = text; selectByMouse: true }
                                    }

                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 8
                                        TextField { Layout.fillWidth: true; placeholderText: "邮箱"; text: draftEmail; onTextChanged: draftEmail = text; selectByMouse: true }
                                        TextField { Layout.fillWidth: true; placeholderText: "联系方式"; text: draftContact; onTextChanged: draftContact = text; selectByMouse: true }
                                    }

                                    TextArea {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 86
                                        placeholderText: "个人简介"
                                        text: draftBio
                                        wrapMode: TextEdit.Wrap
                                        onTextChanged: draftBio = text
                                        selectByMouse: true
                                        background: Rectangle { radius: 8; border.color: "#d1d5db"; color: "#ffffff" }
                                    }

                                    Rectangle {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: saveHint === "" ? 0 : 32
                                        radius: 7
                                        color: saveSuccess ? "#ecfdf5" : "#fef2f2"
                                        border.color: saveSuccess ? "#86efac" : "#fca5a5"
                                        visible: saveHint !== ""
                                        Text {
                                            anchors.centerIn: parent
                                            text: saveHint
                                            color: saveSuccess ? "#166534" : "#991b1b"
                                            font.pixelSize: 12
                                        }
                                    }
                                }
                            }

                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8
                            Button {
                                text: isEditing ? "保存" : "编辑资料"
                                Layout.preferredWidth: 86
                                Layout.preferredHeight: 36
                                enabled: !isUploadingAvatar
                                onClicked: {
                                    if (isEditing) {
                                        saveProfile()
                                        isEditing = false
                                    } else {
                                        isEditing = true
                                    }
                                }
                            }
                            Button {
                                visible: isEditing
                                text: "取消"
                                Layout.preferredWidth: 86
                                Layout.preferredHeight: 36
                                onClicked: {
                                    syncDraftFromService()
                                    isEditing = false
                                    saveHint = ""
                                }
                            }
                            Item { Layout.fillWidth: true }
                        }
                    }
                }
            }
        }

        Connections {
            target: ChatService
            function onCurrentUserNameChanged() { draftName = ChatService.currentUserName || "" }
            function onCurrentUserAvatarChanged() { draftAvatar = ChatService.currentUserAvatar || "" }
            function onCurrentUserGenderChanged() { draftGender = ChatService.currentUserGender || "" }
            function onCurrentUserRegionChanged() { draftRegion = ChatService.currentUserRegion || "" }
            function onCurrentUserPhoneChanged() { draftPhone = ChatService.currentUserPhone || "" }
            function onCurrentUserEmailChanged() { draftEmail = ChatService.currentUserEmail || "" }
            function onCurrentUserContactChanged() { draftContact = ChatService.currentUserContact || "" }
            function onCurrentUserBioChanged() { draftBio = ChatService.currentUserBio || "" }
            function onCurrentUserAgeChanged() { draftAge = ChatService.currentUserAge || 25 }
            function onAvatarUploadProgressChanged(progress) {
                avatarUploadProgress = progress
                isUploadingAvatar = progress < 100
                avatarHint = progress < 100 ? ("头像上传中 " + progress + "%") : "头像上传完成"
            }
            function onAvatarUploadResult(success, avatarUrl, message) {
                avatarUploadSuccess = success
                isUploadingAvatar = false
                avatarUploadProgress = success ? 100 : 0
                avatarHint = message || (success ? "头像上传成功" : "头像上传失败")
                if (success && avatarUrl) {
                    draftAvatar = avatarUrl
                }
            }
            function onUserProfileSaveResult(success, message) {
                saveSuccess = success
                saveHint = message || (success ? "保存成功" : "保存失败")
            }
        }
    }
}
