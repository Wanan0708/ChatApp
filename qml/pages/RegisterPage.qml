import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12
import "../components"

Item {
    id: registerPage
    signal registerRequested(string username, string password, string nickname)
    signal backToLogin()

    // 注册卡片 (与登录页保持风格高度统一)
    Rectangle {
        id: registerCard
        width: 420
        height: 650
        radius: 28
        color: "#f8fafc"
        anchors.centerIn: parent
        
        layer.enabled: true
        layer.effect: DropShadow {
            transparentBorder: true
            color: "#15000000"
            radius: 25
            samples: 30
            verticalOffset: 6
        }

        // 顶部图标装饰 (📝)
        Rectangle {
            width: 80; height: 80; radius: 40; color: "#ffffff"
            anchors.horizontalCenter: parent.horizontalCenter
            y: 0
            layer.enabled: true
            layer.effect: DropShadow { color: "#10000000"; radius: 12; verticalOffset: 4 }
            
            Text {
                anchors.centerIn: parent
                text: "📝"
                font.pixelSize: 42
                renderType: Text.NativeRendering
            }
        }

        // 窗口拖拽支持
        MouseArea {
            anchors.fill: parent
            property point dragStart
            onPressed: dragStart = Qt.point(mouse.x, mouse.y)
            onPositionChanged: {
                var delta = Qt.point(mouse.x - dragStart.x, mouse.y - dragStart.y)
                window.x += delta.x; window.y += delta.y
            }
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 40
            spacing: 20

            Item { Layout.preferredHeight: 50 }

            ColumnLayout {
                spacing: 8
                Layout.fillWidth: true
                Text {
                    text: "加入 ESChat"
                    Layout.fillWidth: true
                    font.pixelSize: 28; font.weight: Font.Bold; color: "#1e293b"
                    font.family: "\"Microsoft YaHei\", sans-serif"
                    horizontalAlignment: Text.AlignHCenter
                }
                Text {
                    text: "开启安全私密的沟通之旅"
                    Layout.fillWidth: true
                    font.pixelSize: 14; color: "#64748b"
                    font.family: "\"Microsoft YaHei\", sans-serif"
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            Item { Layout.preferredHeight: 5 }

            // 输入区域
            ColumnLayout {
                spacing: 12
                Layout.fillWidth: true

                ModernTextField {
                    id: nicknameField
                    Layout.fillWidth: true
                    placeholderText: "设置您的昵称 (必填)"
                    iconText: "👤"
                }

                ModernTextField {
                    id: usernameField
                    Layout.fillWidth: true
                    placeholderText: "设置用户名 (至少3位)"
                    iconText: "🆔"
                }

                ModernTextField {
                    id: passwordField
                    Layout.fillWidth: true
                    placeholderText: "设置登录密码"
                    iconText: "🔒"
                    echoMode: TextInput.Password
                }

                ModernTextField {
                    id: confirmPasswordField
                    Layout.fillWidth: true
                    placeholderText: "再次确认密码"
                    iconText: "🛡️"
                    echoMode: TextInput.Password
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

            // 注册动作
            ColumnLayout {
                spacing: 16
                Layout.fillWidth: true

                Button {
                    id: registerBtn
                    Layout.fillWidth: true
                    Layout.preferredHeight: 52
                    
                    contentItem: Text {
                        text: "注 册"
                        font.pixelSize: 16; font.weight: Font.DemiBold; color: "white"
                        font.family: "\"Microsoft YaHei\", sans-serif"
                        horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                    }
                    
                    background: Rectangle {
                        radius: 12
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: registerBtn.down ? "#4338ca" : "#4f46e5" }
                            GradientStop { position: 1.0; color: registerBtn.down ? "#6d28d9" : "#7c3aed" }
                        }
                    }
                    
                    onClicked: {
                        if (nicknameField.text.trim() === "") {
                            showErrorMessage("昵称不能为空")
                            return
                        }
                        if (usernameField.text.trim().length < 3) {
                            showErrorMessage("用户名至少需要3个字符")
                            return
                        }
                        if (passwordField.text.trim().length < 6) {
                            showErrorMessage("密码太短，至少需要6位")
                            return
                        }
                        if (passwordField.text !== confirmPasswordField.text) {
                            showErrorMessage("两次输入的密码不一致")
                            return
                        }
                        
                        registerRequested(usernameField.text.trim(), 
                                          passwordField.text.trim(), 
                                          nicknameField.text.trim())
                    }
                }

                Text {
                    text: "已有账户? 返回登录"
                    Layout.alignment: Qt.AlignHCenter
                    font.pixelSize: 13; font.weight: Font.DemiBold; color: "#64748b"
                    font.family: "\"Microsoft YaHei\", sans-serif"
                    MouseArea {
                        anchors.fill: parent; cursorShape: Qt.PointingHandCursor
                        onClicked: backToLogin()
                    }
                }
            }

            Item { Layout.fillHeight: true }

            Text {
                Layout.fillWidth: true
                text: "© 2026 ESChat · 安全可靠的聊天体验"
                font.pixelSize: 11; color: "#94a3b8"
                font.family: "\"Microsoft YaHei\", sans-serif"
                horizontalAlignment: Text.AlignHCenter
            }
        }

        // 关闭按钮
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
}
