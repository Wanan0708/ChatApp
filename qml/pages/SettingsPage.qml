import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12
import ChatApp.Network 1.0
import "../components"

Rectangle {
    id: settingsPage
    color: "#f8fafc" // 浅灰背景提升现代感

    // 对外暴露的方法，供 ProfilePanel 调用
    function openChangePasswordDialog() {
        changePasswordDialog.open()
    }

    function openLogoutDialog() {
        logoutConfirmDialog.open()
    }

    // 修改密码对话框 (现代化重构)
    Dialog {
        id: changePasswordDialog
        width: 400
        height: 380
        anchors.centerIn: parent
        modal: true
        dim: true
        
        enter: Transition {
            NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 250; easing.type: Easing.OutCubic }
            NumberAnimation { property: "scale"; from: 0.9; to: 1.0; duration: 250; easing.type: Easing.OutBack }
        }
        
        exit: Transition {
            NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 150 }
            NumberAnimation { property: "scale"; from: 1.0; to: 0.9; duration: 150 }
        }

        background: Rectangle {
            color: "#ffffff"
            radius: 16
            layer.enabled: true
            layer.effect: DropShadow {
                transparentBorder: true
                color: "#40000000"
                radius: 20
                samples: 25
                verticalOffset: 8
            }
        }

        header: Item {
            height: 64
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 24
                anchors.rightMargin: 16
                Text {
                    text: "修改密码"
                    font.pixelSize: 18
                    font.weight: Font.DemiBold
                    color: "#1e293b"
                    Layout.fillWidth: true
                }
                ToolButton {
                    text: "×"
                    font.pixelSize: 24
                    onClicked: changePasswordDialog.close()
                }
            }
            Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: "#f1f5f9" }
        }

        contentItem: ColumnLayout {
            spacing: 20
            anchors.margins: 24

            ColumnLayout {
                spacing: 8
                Layout.fillWidth: true
                Text { text: "新密码"; font.pixelSize: 13; color: "#64748b"; renderType: Text.NativeRendering }
                TextField {
                    id: newPwdField
                    Layout.fillWidth: true
                    echoMode: TextField.Password
                    placeholderText: "输入新密码"
                    selectByMouse: true
                    background: Rectangle {
                        implicitHeight: 40; radius: 8; border.color: parent.activeFocus ? "#3b82f6" : "#e2e8f0"; border.width: parent.activeFocus ? 2 : 1
                    }
                }
            }

            ColumnLayout {
                spacing: 8
                Layout.fillWidth: true
                Text { text: "确认新密码"; font.pixelSize: 13; color: "#64748b"; renderType: Text.NativeRendering }
                TextField {
                    id: confirmPwdField
                    Layout.fillWidth: true
                    echoMode: TextField.Password
                    placeholderText: "再次输入确认"
                    selectByMouse: true
                    background: Rectangle {
                        implicitHeight: 40; radius: 8; border.color: parent.activeFocus ? "#3b82f6" : "#e2e8f0"; border.width: parent.activeFocus ? 2 : 1
                    }
                }
            }

            Text {
                id: pwdErrorText
                Layout.fillWidth: true
                color: "#ef4444"
                font.pixelSize: 12
                visible: false
                renderType: Text.NativeRendering
                horizontalAlignment: Text.AlignHCenter
            }

            Item { Layout.fillHeight: true }

            RowLayout {
                spacing: 12
                Layout.alignment: Qt.AlignRight
                Button {
                    text: "取消"
                    onClicked: changePasswordDialog.close()
                    background: Rectangle { implicitWidth: 80; implicitHeight: 36; radius: 8; color: "transparent"; border.color: "#e2e8f0" }
                }
                Button {
                    text: "确认修改"
                    highlighted: true
                    onClicked: {
                        if (newPwdField.text !== "" && newPwdField.text === confirmPwdField.text) {
                            // 异步调用，结果在 onPasswordUpdateResult 中处理
                            ChatService.changePassword(newPwdField.text)
                            changePasswordDialog.close()
                        } else {
                            pwdErrorText.text = "密码不能为空或输入不匹配"
                            pwdErrorText.visible = true
                        }
                    }
                    background: Rectangle { implicitWidth: 100; implicitHeight: 36; radius: 8; color: parent.down ? "#2563eb" : "#3b82f6" }
                    contentItem: Text { text: parent.text; color: "white"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; font.weight: Font.Medium }
                }
            }
        }
    }

    // 成功提示对话框
    Dialog {
        id: successDialog
        width: 300
        height: 180
        anchors.centerIn: parent
        modal: true
        background: Rectangle { radius: 16; color: "white" }
        contentItem: ColumnLayout {
            anchors.centerIn: parent
            spacing: 16
            Text { text: "✅"; font.pixelSize: 40; Layout.alignment: Qt.AlignHCenter }
            Text { text: "密码修改成功！"; font.pixelSize: 16; color: "#1e293b"; Layout.alignment: Qt.AlignHCenter; renderType: Text.NativeRendering }
            Button {
                text: "确定"
                Layout.alignment: Qt.AlignHCenter
                onClicked: successDialog.close()
                background: Rectangle { implicitWidth: 80; implicitHeight: 32; radius: 16; color: "#3b82f6" }
                contentItem: Text { text: parent.text; color: "white"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
            }
        }
    }

    // 退出确认对话框 (现代化重构)
    Dialog {
        id: logoutConfirmDialog
        width: 360
        height: 240
        anchors.centerIn: parent
        modal: true
        dim: true
        
        enter: Transition {
            NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 250; easing.type: Easing.OutCubic }
            NumberAnimation { property: "scale"; from: 0.9; to: 1.0; duration: 250; easing.type: Easing.OutBack }
        }
        
        exit: Transition {
            NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 150 }
            NumberAnimation { property: "scale"; from: 1.0; to: 0.9; duration: 150 }
        }

        background: Rectangle {
            color: "#ffffff"
            radius: 16
            layer.enabled: true
            layer.effect: DropShadow {
                transparentBorder: true
                color: "#40000000"
                radius: 20
                samples: 25
                verticalOffset: 8
            }
        }

        contentItem: ColumnLayout {
            spacing: 24
            anchors.margins: 24

            Text {
                text: "退出登录"
                font.pixelSize: 20
                font.weight: Font.Bold
                color: "#1e293b"
                Layout.alignment: Qt.AlignHCenter
                renderType: Text.NativeRendering
            }

            Text {
                text: "您确定要退出当前账号并返回登录页面吗？请确保消息已发送完毕。"
                font.pixelSize: 14
                color: "#64748b"
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                renderType: Text.NativeRendering
            }

            RowLayout {
                spacing: 12
                Layout.fillWidth: true
                
                Button {
                    text: "取消"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 44
                    onClicked: logoutConfirmDialog.close()
                    background: Rectangle { radius: 12; color: parent.hovered ? "#f1f5f9" : "white"; border.color: "#e2e8f0" }
                    contentItem: Text { text: parent.text; color: "#64748b"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; font.weight: Font.Medium }
                }

                Button {
                    text: "确定退出"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 44
                    onClicked: {
                        logoutConfirmDialog.close()
                        ChatService.logout()
                    }
                    background: Rectangle { radius: 12; color: parent.down ? "#ea580c" : "#f97316" }
                    contentItem: Text { text: parent.text; color: "white"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; font.weight: Font.Medium }
                }
            }
        }
    }

    ScrollView {
        id: scroll
        anchors.fill: parent
        clip: true
        contentWidth: parent.width
        contentHeight: Math.round(container.implicitHeight + 80)

        ColumnLayout {
            id: container
            width: Math.round(Math.min(parent.width - 48, 600))
            x: Math.round((parent.width - width) / 2)
            y: 30
            spacing: 24

            // 标题
            Text {
                text: "设置"
                font.pixelSize: 24
                font.weight: Font.Bold
                font.family: "\"Microsoft YaHei\", \"Segoe UI\", sans-serif"
                font.letterSpacing: 0.5
                color: "#1e293b"
                renderType: Text.NativeRendering
            }

            // 账户信息区
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: accountSettingsCol.height + 40
                color: "#ffffff"
                radius: 12
                border.color: "#e2e8f0"
                border.width: 1

                layer.enabled: true
                layer.effect: DropShadow {
                    transparentBorder: true
                    color: "#0A000000"
                    radius: 8
                    samples: 17
                    verticalOffset: 2
                }

                ColumnLayout {
                    id: accountSettingsCol
                    width: parent.width
                    spacing: 20
                    anchors.margins: 20
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right

                    Text {
                        text: "账户设置"
                        font.pixelSize: 16
                        font.weight: Font.Bold
                        font.family: "\"Microsoft YaHei\", \"Segoe UI\", sans-serif"
                        font.letterSpacing: 0.2
                        color: "#1e293b"
                        renderType: Text.NativeRendering
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 16

                        Avatar {
                            width: 52
                            height: 52
                            isSelf: true
                            avatarSource: ChatService.currentUserAvatar || "qrc:/new/prefix1/image/boy.png"
                        }

                        ColumnLayout {
                            Layout.fillWidth: true; spacing: 4
                            Text {
                                text: ChatService.currentUserName || "未登录";
                                font.pixelSize: 15;
                                font.weight: Font.DemiBold;
                                font.family: "\"Microsoft YaHei\", \"Segoe UI\", sans-serif";
                                color: "#1e293b"
                                renderType: Text.NativeRendering
                            }
                            Text {
                                text: "用户 ID: " + (ChatService.currentUserId || "N/A");
                                font.pixelSize: 13;
                                font.family: "\"Microsoft YaHei\", sans-serif";
                                color: "#64748b"
                                renderType: Text.NativeRendering
                            }
                        }
                    }

                    Rectangle { Layout.fillWidth: true; height: 1; color: "#f1f5f9" }

                    RowLayout {
                        Layout.fillWidth: true; spacing: 12
                        ColumnLayout {
                            Layout.fillWidth: true; spacing: 4
                            Text { 
                                text: "修改密码"; 
                                font.pixelSize: 14; 
                                font.weight: Font.Medium; 
                                font.family: "\"Microsoft YaHei\", \"Segoe UI\", sans-serif";
                                color: "#1e293b"; 
                                renderType: Text.NativeRendering 
                            }
                            Text { 
                                text: "定期更新密码以保证账户安全"; 
                                font.pixelSize: 12; 
                                font.family: "\"Microsoft YaHei\", sans-serif";
                                color: "#64748b"; 
                                renderType: Text.NativeRendering 
                            }
                        }
                        Button {
                            id: changePwdBtn
                            text: "前往修改"
                            contentItem: Text {
                                text: parent.text
                                font.pixelSize: 12
                                font.family: "\"Microsoft YaHei\", sans-serif"
                                font.weight: Font.Medium
                                color: "#3b82f6"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                renderType: Text.NativeRendering
                            }
                            background: Rectangle { 
                                implicitWidth: 80; implicitHeight: 32; 
                                color: changePwdBtn.hovered ? "#eff6ff" : "transparent"; 
                                border.color: "#3b82f6"; border.width: 1; radius: 16 
                            }
                            onClicked: changePasswordDialog.open()
                        }
                    }
                }
            }

            // 应用设置区
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: appSettingsCol.height + 40
                color: "#ffffff"
                radius: 12
                border.color: "#e2e8f0"
                border.width: 1

                layer.enabled: true
                layer.effect: DropShadow {
                    transparentBorder: true
                    color: "#0A000000"
                    radius: 8
                    samples: 17
                    verticalOffset: 2
                }

                ColumnLayout {
                    id: appSettingsCol
                    width: parent.width; spacing: 16; anchors.margins: 20; anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
                    Text { 
                        text: "通用设置" ; 
                        font.pixelSize: 16; 
                        font.weight: Font.Bold; 
                        font.family: "\"Microsoft YaHei\", \"Segoe UI\", sans-serif"
                        font.letterSpacing: 0.2
                        color: "#1e293b"; 
                        renderType: Text.NativeRendering 
                    }
                    
                    RowLayout {
                        Layout.fillWidth: true; spacing: 12
                        ColumnLayout { Layout.fillWidth: true; spacing: 4
                            Text { text: "消息通知"; font.pixelSize: 14; font.family: "\"Microsoft YaHei\", sans-serif"; color: "#1e293b"; renderType: Text.NativeRendering }
                            Text { text: "接收新消息时显示系统通知"; font.pixelSize: 12; font.family: "\"Microsoft YaHei\", sans-serif"; color: "#64748b"; renderType: Text.NativeRendering }
                        }
                        Switch { 
                            checked: ChatService.notificationsEnabled
                            onCheckedChanged: ChatService.notificationsEnabled = checked 
                        }
                    }
                    
                    Rectangle { Layout.fillWidth: true; height: 1; color: "#f1f5f9" }
                    
                    RowLayout {
                        Layout.fillWidth: true; spacing: 12
                        ColumnLayout { Layout.fillWidth: true; spacing: 4
                            Text { text: "声音提醒"; font.pixelSize: 14; font.family: "\"Microsoft YaHei\", sans-serif"; color: "#1e293b"; renderType: Text.NativeRendering }
                            Text { text: "收到新消息时播放声音"; font.pixelSize: 12; font.family: "\"Microsoft YaHei\", sans-serif"; color: "#64748b"; renderType: Text.NativeRendering }
                        }
                        Switch { 
                            checked: ChatService.soundEnabled
                            onCheckedChanged: ChatService.soundEnabled = checked 
                        }
                    }
                    
                    Rectangle { Layout.fillWidth: true; height: 1; color: "#f1f5f9" }
                    
                    RowLayout {
                        Layout.fillWidth: true; spacing: 12
                        ColumnLayout { Layout.fillWidth: true; spacing: 4
                            Text { text: "自动登录"; font.pixelSize: 14; font.family: "\"Microsoft YaHei\", sans-serif"; color: "#1e293b"; renderType: Text.NativeRendering }
                            Text { text: "启动软件时自动登录当前账号"; font.pixelSize: 12; font.family: "\"Microsoft YaHei\", sans-serif"; color: "#64748b"; renderType: Text.NativeRendering }
                        }
                        Switch { 
                            checked: ChatService.autoLoginEnabled
                            onCheckedChanged: ChatService.autoLoginEnabled = checked 
                        }
                    }
                }
            }

            // 关于区 (新增)
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: aboutCol.height + 40
                color: "#ffffff"
                radius: 12
                border.color: "#e2e8f0"
                border.width: 1

                layer.enabled: true
                layer.effect: DropShadow {
                    transparentBorder: true
                    color: "#0A000000"
                    radius: 8
                    samples: 17
                    verticalOffset: 2
                }

                ColumnLayout {
                    id: aboutCol
                    width: parent.width; spacing: 12; anchors.margins: 20; anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
                    Text { 
                        text: "关于"; 
                        font.pixelSize: 16; 
                        font.weight: Font.Bold; 
                        font.family: "\"Microsoft YaHei\", \"Segoe UI\", sans-serif"
                        font.letterSpacing: 0.2
                        color: "#1e293b"; 
                        renderType: Text.NativeRendering 
                    }
                    
                    RowLayout {
                        Layout.fillWidth: true
                        Text { text: "应用版本"; font.pixelSize: 14; color: "#1e293b"; renderType: Text.NativeRendering }
                        Item { Layout.fillWidth: true }
                        Text { text: "v1.0.2"; font.pixelSize: 14; color: "#64748b"; renderType: Text.NativeRendering }
                    }
                    
                    RowLayout {
                        Layout.fillWidth: true
                        Text { text: "后端服务连接"; font.pixelSize: 14; color: "#1e293b"; renderType: Text.NativeRendering }
                        Item { Layout.fillWidth: true }
                        Text { 
                            text: ChatService.isDatabaseConnected() ? "已连接 ✓" : "未连接 ✗"; 
                            font.pixelSize: 14; color: ChatService.isDatabaseConnected() ? "#10b981" : "#ef4444"; 
                            renderType: Text.NativeRendering
                        }
                    }
                }
            }

            Item { Layout.preferredHeight: 12 }

            // 退出登录按钮
            Button {
                id: logoutBtn
                Layout.fillWidth: true
                Layout.preferredHeight: 50
                text: "退出登录"
                
                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 15
                    font.weight: Font.DemiBold
                    font.family: "\"Microsoft YaHei\", \"Segoe UI\", sans-serif"
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    renderType: Text.NativeRendering
                }

                background: Rectangle {
                    color: logoutBtn.pressed ? "#c2410c" : (logoutBtn.hovered ? "#ea580c" : "#f97316")
                    radius: 12
                    Behavior on color { ColorAnimation { duration: 150 } }
                }
                
                onClicked: logoutConfirmDialog.open()
            }

            Item { Layout.preferredHeight: 40 }
        }
    }

    // 处理 ChatService 的异步信号
    Connections {
        target: ChatService
        onPasswordUpdateResult: {
            if (success) {
                successDialog.open()
            } else {
                // 显示错误提示
                pwdErrorText.text = error || "修改失败，请检查网络"
                pwdErrorText.visible = true
            }
        }
    }
}