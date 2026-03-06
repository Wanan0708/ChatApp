import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12
import ChatApp.Network 1.0
import "../components"

Item {
    id: loginPage
    signal loginRequested(string username, string password)
    signal registerRequested()
    signal forgotPasswordRequested(string username)

    // 移除原有大的蓝色背景，保持底层透明
    Item {
        anchors.fill: parent
    }

    // 登录卡片 (Glassmorphism 效果)
    Rectangle {
        id: loginCard
        width: 420
        height: 600
        radius: 28
        color: "#f8fafc"
        anchors.centerIn: parent
        
        layer.enabled: true
        layer.effect: DropShadow {
            transparentBorder: true
            color: "#15000000" // 极度柔和的阴影 (约15%不透明度)
            radius: 25
            samples: 30
            verticalOffset: 6
        }

        // 顶部图标装饰
        Rectangle {
            width: 80; height: 80; radius: 40; color: "#ffffff"
            anchors.horizontalCenter: parent.horizontalCenter
            y: 0 // 在 670px 窗高下的新位置，距离顶边缘约 10px
            layer.enabled: true
            layer.effect: DropShadow { color: "#10000000"; radius: 12; verticalOffset: 4 }
            
            Text {
                anchors.centerIn: parent
                text: "💬"
                font.pixelSize: 42
                renderType: Text.NativeRendering
            }
        }

        // 标题栏拖拽支持 - 将范围限制在卡内，允许点击背景穿透
        MouseArea {
            id: dragArea
            anchors.fill: parent
            property point dragStart
            onPressed: dragStart = Qt.point(mouse.x, mouse.y)
            onPositionChanged: {
                var delta = Qt.point(mouse.x - dragStart.x, mouse.y - dragStart.y)
                window.x += delta.x
                window.y += delta.y
            }
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 40
            spacing: 24

            Item { Layout.preferredHeight: 50 }

            ColumnLayout {
                spacing: 8
                Layout.fillWidth: true
                Text {
                    text: "XWChat"
                    Layout.fillWidth: true
                    font.pixelSize: 32; font.weight: Font.Bold; color: "#1e293b"
                    font.family: "\"Microsoft YaHei\", sans-serif"
                    horizontalAlignment: Text.AlignHCenter
                    renderType: Text.NativeRendering
                }
                Text {
                    text: "安全、极速、私密的交流体验"
                    Layout.fillWidth: true
                    font.pixelSize: 14; color: "#64748b"
                    font.family: "\"Microsoft YaHei\", sans-serif"
                    horizontalAlignment: Text.AlignHCenter
                    renderType: Text.NativeRendering
                }
            }

            Item { Layout.preferredHeight: 10 }

            // 输入区域
            ColumnLayout {
                spacing: 16
                Layout.fillWidth: true

                ModernTextField {
                    id: usernameField
                    Layout.fillWidth: true
                    placeholderText: "请输入用户名"
                    iconText: "👤"
                }

                ModernTextField {
                    id: passwordField
                    Layout.fillWidth: true
                    placeholderText: "请输入密码"
                    iconText: "🔒"
                    echoMode: TextInput.Password
                }

                RowLayout {
                    Layout.fillWidth: true
                    ModernCheckBox {
                        id: rememberCheck
                        text: "记住我的登录信息"
                        checked: ChatService.rememberPasswordEnabled
                        onCheckedChanged: ChatService.rememberPasswordEnabled = checked
                    }
                }
            }

            // 错误提示
            Text {
                id: errorMessage
                Layout.fillWidth: true
                font.pixelSize: 13; color: "#ef4444"
                horizontalAlignment: Text.AlignHCenter
                visible: false
                font.family: "\"Microsoft YaHei\", sans-serif"
            }

            // 登录动作
            ColumnLayout {
                spacing: 16
                Layout.fillWidth: true

                Button {
                    id: loginBtn
                    Layout.fillWidth: true
                    Layout.preferredHeight: 52

                    contentItem: Text {
                        text: "登 录"
                        font.pixelSize: 16; font.weight: Font.DemiBold; color: "white"
                        font.family: "\"Microsoft YaHei\", sans-serif"
                        horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        radius: 12
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: loginBtn.down ? "#4338ca" : "#4f46e5" }
                            GradientStop { position: 1.0; color: loginBtn.down ? "#6d28d9" : "#7c3aed" }
                        }
                    }

                    onClicked: {
                        if (usernameField.text.trim() === "") {
                            showErrorMessage("用户名不能为空")
                            return
                        }
                        if (passwordField.text.trim() === "") {
                            showErrorMessage("密码不能为空")
                            return
                        }
                        // ✅ 更新记住密码的用户名和密码为当前输入的值
                        if (rememberCheck.checked) {
                            ChatService.setLastUsername(usernameField.text.trim())
                            ChatService.setLastPassword(passwordField.text.trim())
                        }
                        loginRequested(usernameField.text.trim(), passwordField.text.trim())
                    }
                }

                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 8
                    Text { text: "还没有账户?"; font.pixelSize: 13; color: "#64748b"; font.family: "\"Microsoft YaHei\", sans-serif" }
                    Text {
                        text: "立即注册"
                        font.pixelSize: 13; font.weight: Font.DemiBold; color: "#4f46e5"
                        font.family: "\"Microsoft YaHei\", sans-serif"
                        MouseArea {
                            anchors.fill: parent; cursorShape: Qt.PointingHandCursor
                            onClicked: registerRequested()
                        }
                    }
                    Text { text: "|"; font.pixelSize: 13; color: "#e2e8f0" }
                    Text {
                        text: "忘记密码?"
                        font.pixelSize: 13; color: "#64748b"
                        font.family: "\"Microsoft YaHei\", sans-serif"
                        MouseArea {
                            anchors.fill: parent; cursorShape: Qt.PointingHandCursor
                            onClicked: forgotPasswordRequested(usernameField.text.trim())
                        }
                    }
                }
            }

            Item { Layout.fillHeight: true }

            Text {
                Layout.fillWidth: true
                text: "© 2026 XWChat · 让沟通更简单"
                font.pixelSize: 11; color: "#94a3b8"
                font.family: "\"Microsoft YaHei\", sans-serif"
                horizontalAlignment: Text.AlignHCenter
            }
        }

        // 关闭按钮 (右上角悬浮)
        Rectangle {
            width: 32; height: 32; radius: 16; color: "#f1f5f9"
            anchors.right: parent.right; anchors.top: parent.top
            anchors.margins: 20
            
            Text { anchors.centerIn: parent; text: "×"; font.pixelSize: 20; color: "#64748b" }
            
            MouseArea {
                anchors.fill: parent; cursorShape: Qt.PointingHandCursor
                onClicked: window.close()
                onEntered: parent.color = "#ffe4e6"
                onExited: parent.color = "#f1f5f9"
            }
        }
    }

    Timer {
        id: errorMessageTimer; interval: 3000
        onTriggered: errorMessage.visible = false
    }

    function showErrorMessage(msg) {
        errorMessage.text = msg
        errorMessage.visible = true
        errorMessageTimer.restart()
    }

    Component.onCompleted: {
        // 加载上次登录的用户名和密码（如果启用了记住密码）
        if (ChatService.rememberPasswordEnabled) {
            usernameField.text = ChatService.lastUsername
            passwordField.text = ChatService.lastPassword
            rememberCheck.checked = true
        }
    }

    // 当用户名或密码变化时，如果启用了记住密码，立即更新
    function updateCredentials() {
        if (ChatService.rememberPasswordEnabled) {
            ChatService.setLastUsername(usernameField.text.trim())
            ChatService.setLastPassword(passwordField.text.trim())
        }
    }
}
