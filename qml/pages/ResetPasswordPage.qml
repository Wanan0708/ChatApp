import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12
import "../components"

Item {
    id: resetPage
    signal requestCode(string username)
    signal resetPassword(string code, string newPassword)
    signal backToLogin()

    // 重置密码卡片
    Rectangle {
        id: resetCard
        width: 420
        height: 600
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

        // 顶部图标装饰 (🔑)
        Rectangle {
            width: 80; height: 80; radius: 40; color: "#ffffff"
            anchors.horizontalCenter: parent.horizontalCenter
            y: 0
            layer.enabled: true
            layer.effect: DropShadow { color: "#10000000"; radius: 12; verticalOffset: 4 }
            
            Text {
                anchors.centerIn: parent
                text: "🔑"
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
                    text: "找回您的账户"
                    Layout.fillWidth: true
                    font.pixelSize: 28; font.weight: Font.Bold; color: "#1e293b"
                    font.family: "\"Microsoft YaHei\", sans-serif"
                    horizontalAlignment: Text.AlignHCenter
                }
                Text {
                    text: "通过验证码重置您的登录密码"
                    Layout.fillWidth: true
                    font.pixelSize: 14; color: "#64748b"
                    font.family: "\"Microsoft YaHei\", sans-serif"
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            Item { Layout.preferredHeight: 10 }

            // 输入区域
            ColumnLayout {
                spacing: 16
                Layout.fillWidth: true

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12
                    ModernTextField {
                        id: usernameField
                        Layout.fillWidth: true
                        placeholderText: "输入用户名"
                        iconText: "🆔"
                    }
                    Button {
                        id: sendCodeBtn
                        Layout.preferredHeight: 52
                        Layout.preferredWidth: 100
                        contentItem: Text {
                            text: "获取代码"
                            font.pixelSize: 13; font.weight: Font.Medium; color: "white"
                            horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            radius: 12
                            color: sendCodeBtn.pressed ? "#4338ca" : "#4f46e5"
                        }
                        onClicked: {
                            if (usernameField.text.trim() === "") {
                                showMessage("请先输入用户名")
                                return
                            }
                            requestCode(usernameField.text.trim())
                        }
                    }
                }

                ModernTextField {
                    id: codeField
                    Layout.fillWidth: true
                    placeholderText: "输入 6 位验证码"
                    iconText: "🔢"
                }

                ModernTextField {
                    id: newPasswordField
                    Layout.fillWidth: true
                    placeholderText: "设置您的新密码"
                    iconText: "🔒"
                    echoMode: TextInput.Password
                }
            }

            // 状态/错误消息
            Text {
                id: messageDisplay
                Layout.fillWidth: true
                font.pixelSize: 13; color: "#ef4444"
                horizontalAlignment: Text.AlignHCenter
                visible: false
                font.family: "\"Microsoft YaHei\", sans-serif"
            }

            // 提交动作
            ColumnLayout {
                spacing: 16
                Layout.fillWidth: true

                Button {
                    id: resetBtn
                    Layout.fillWidth: true
                    Layout.preferredHeight: 52
                    
                    contentItem: Text {
                        text: "重 置 密 码"
                        font.pixelSize: 16; font.weight: Font.DemiBold; color: "white"
                        font.family: "\"Microsoft YaHei\", sans-serif"
                        horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                    }
                    
                    background: Rectangle {
                        radius: 12
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: resetBtn.down ? "#4338ca" : "#4f46e5" }
                            GradientStop { position: 1.0; color: resetBtn.down ? "#6d28d9" : "#7c3aed" }
                        }
                    }
                    
                    onClicked: {
                        if (codeField.text.trim() === "") {
                            showMessage("请输入验证码")
                            return
                        }
                        if (newPasswordField.text.length < 6) {
                            showMessage("密码至少需要6位")
                            return
                        }
                        resetPassword(codeField.text.trim(), newPasswordField.text)
                    }
                }

                Text {
                    text: "回想起密码了? 返回登录"
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
        id: messageTimer; interval: 3000
        onTriggered: messageDisplay.visible = false
    }

    function showMessage(msg) {
        messageDisplay.text = msg
        messageDisplay.visible = true
        messageTimer.restart()
    }

    function setGeneratedCode(code) {
        codeField.text = code
        messageDisplay.color = "#10b981" // 成功绿色
        showMessage("验证码已自动填入: " + code)
        // 延迟恢复红色供错误使用
        Qt.callLater(function() { messageDisplay.color = "#ef4444" })
    }
}
