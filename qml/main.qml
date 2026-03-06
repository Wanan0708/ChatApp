import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12
import ChatApp.Models 1.0
import ChatApp.Network 1.0
import "pages"
import "components"

ApplicationWindow {
    id: window
    visible: true
    width: isLoggedIn ? 900 : 460
    height: isLoggedIn ? 650 : 670
    minimumWidth: isLoggedIn ? 900 : 460
    minimumHeight: isLoggedIn ? 650 : 670
    title: "XWChat v1.0"

    flags: Qt.FramelessWindowHint | Qt.Window
    color: isLoggedIn ? "white" : "transparent"

    readonly property var ws: ChatService
    readonly property var currentUser: ({ id: ws.currentUserId, name: ws.currentUserName })
    property bool isLoggedIn: false
    property bool isOnRegisterPage: false
    property bool isOnResetPage: false

    // 幽灵移动 (Ghost Move) 过渡控制
    property real contentOpacity: 1.0
    
    // 个人信息面板引用
    property alias profilePanel: globalProfilePanel
    
    // 全局 UI 容器，用于统一淡入淡出
    Item {
        id: contentContainer
        anchors.fill: parent
        opacity: contentOpacity
        Behavior on opacity { NumberAnimation { duration: 150 } }

        // 自定义主标题栏 (仅在登录后显示)
        Rectangle {
            id: mainTitleBar
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: isLoggedIn ? 40 : 0
            visible: isLoggedIn
            color: "#f5f5f5"
            z: 100
            
            // ... (标题栏内容保持不变)
            MouseArea {
                id: titleDragArea
                anchors.fill: parent
                property point clickPos: "0,0"
                onPressed: clickPos = Qt.point(mouse.x,mouse.y)
                onPositionChanged: {
                    var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
                    window.x += delta.x; window.y += delta.y
                }
                onDoubleClicked: {
                    if(window.visibility === Window.Maximized) window.showNormal();
                    else window.showMaximized();
                }
            }

            RowLayout {
                anchors.fill: parent
                spacing: 0
                Item { Layout.preferredWidth: 16 }
                Text {
                    text: "XWChat"
                    font.pixelSize: 13; font.weight: Font.Medium; color: "#333333"
                    font.family: "Microsoft YaHei, SimSun, sans-serif"
                    Layout.alignment: Qt.AlignVCenter
                }
                Rectangle {
                    Layout.alignment: Qt.AlignVCenter; Layout.leftMargin: 20
                    Layout.preferredHeight: 26; Layout.preferredWidth: userInfoRow.implicitWidth + 20
                    radius: 10; color: userInfoArea.containsMouse ? "#e0e0e0" : "#eaeaea"
                    border.color: userInfoArea.containsMouse ? "#c8c8c8" : "#d8d8d8"; border.width: 1
                    Row {
                        id: userInfoRow; anchors.centerIn: parent; spacing: 5
                        Avatar {
                            isSelf: true
                            width: 22
                            height: 22
                            anchors.verticalCenter: parent.verticalCenter
                            avatarSource: ChatService.currentUserAvatar || "qrc:/new/prefix1/image/boy.png"
                        }
                        Text {
                            text: ChatService.currentUserName || "未登录"
                            color: "#444"; font.pixelSize: 12; font.weight: Font.Medium
                            font.family: "Microsoft YaHei, SimSun, sans-serif"
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                    MouseArea { 
                        id: userInfoArea; 
                        anchors.fill: parent; 
                        hoverEnabled: true; 
                        cursorShape: Qt.PointingHandCursor
                        onClicked: profilePanel.open()
                    }
                }
                Item { Layout.fillWidth: true }
                Row {
                    Layout.alignment: Qt.AlignRight | Qt.AlignTop; height: parent.height
                    Rectangle {
                        width: 46; height: parent.height; color: minArea.containsMouse ? "#e5e5e5" : "transparent"
                        Text { anchors.centerIn: parent; text: "—"; font.pixelSize: 10; color: "#333" }
                        MouseArea { id: minArea; anchors.fill: parent; hoverEnabled: true; onClicked: window.showMinimized() }
                    }
                    Rectangle {
                        width: 46; height: parent.height; color: maxArea.containsMouse ? "#e5e5e5" : "transparent"
                        Text { anchors.centerIn: parent; text: window.visibility === Window.Maximized ? "❐" : "☐"; font.pixelSize: 14; color: "#333" }
                        MouseArea { id: maxArea; anchors.fill: parent; hoverEnabled: true; onClicked: { if(window.visibility === Window.Maximized) window.showNormal(); else window.showMaximized(); } }
                    }
                    Rectangle {
                        width: 46; height: parent.height; color: closeArea.containsMouse ? "#e81123" : "transparent"
                        Text { anchors.centerIn: parent; text: "✕"; font.pixelSize: 14; color: closeArea.containsMouse ? "white" : "#333" }
                        MouseArea { id: closeArea; anchors.fill: parent; hoverEnabled: true; onClicked: window.close() }
                    }
                }
            }
            Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: "#e0e0e0" }
        }

        Loader {
            id: pageLoader
            anchors.top: isLoggedIn ? mainTitleBar.bottom : parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            source: isLoggedIn ? "pages/ChatMain.qml" : (isOnRegisterPage ? "pages/RegisterPage.qml" : (isOnResetPage ? "pages/ResetPasswordPage.qml" : "pages/LoginPage.qml"))
            
            property string resetUsername: ""
            onLoaded: {
                if (!isLoggedIn) {
                    if (isOnRegisterPage) {
                        item.registerRequested.connect(handleRegister)
                        item.backToLogin.connect(() => { isOnRegisterPage = false })
                    } else if (isOnResetPage) {
                        if (item) {
                            if (item.requestCode) item.requestCode.connect(handleRequestReset)
                            if (item.resetPassword) item.resetPassword.connect(handleDoReset)
                            if (item.backToLogin) item.backToLogin.connect(() => { isOnResetPage = false })
                        }
                    } else {
                        item.loginRequested.connect(handleLogin)
                        item.registerRequested.connect(() => { isOnRegisterPage = true })
                        item.forgotPasswordRequested.connect((username) => { isOnResetPage = true; resetUsername = username })
                    }
                }
                if (isLoggedIn && item) {
                    // 连接添加好友请求信号
                    if (item.addContactRequested) {
                        item.addContactRequested.connect(() => { globalAddContactDialog.open() })
                    }
                }
            }
        }

        // 处理来自 ChatService 的异步信号
        Connections {
            target: ChatService

            onCurrentUserIdChanged: {
                // 当用户 ID 变化时（如退出登录），更新登录状态
                if (ChatService.currentUserId === "") {
                    // 退出登录流程
                    contentOpacity = 0.0
                    isLoggedIn = false
                    isOnRegisterPage = false
                    isOnResetPage = false
                    ghostMoveTimer.start()
                }
            }

            onLoginResult: {
                if (success) {
                    // 登录成功，启动幽灵移动流程：先淡出
                    contentOpacity = 0.0
                    isLoggedIn = true // 切换逻辑状态，触发 Loader 换页
                    ghostMoveTimer.start()

                    // 连接到聊天服务器 (使用服务器 IP)
                    ws.connectToDefaultChatServer()
                } else {
                    if (pageLoader.item && pageLoader.item.showErrorMessage) {
                        pageLoader.item.showErrorMessage(error || "用户名或密码错误")
                    }
                }
            }
            
            onRegisterResult: {
                if (success) {
                    isOnRegisterPage = false
                    if (pageLoader.item && pageLoader.item.showErrorMessage) {
                        pageLoader.item.showErrorMessage("注册成功，请登录")
                    }
                } else {
                    if (pageLoader.item && pageLoader.item.showErrorMessage) {
                        pageLoader.item.showErrorMessage(error || "注册失败，用户名可能已存在")
                    }
                }
            }
            
            // 密码重置结果处理
            onPasswordResetCodeGenerated: {
                if (success) {
                    if (pageLoader.item && pageLoader.item.setGeneratedCode) {
                        pageLoader.item.setGeneratedCode(resetCode)
                    } else if (pageLoader.item && pageLoader.item.showMessage) {
                        pageLoader.item.showMessage("验证码已生成：" + resetCode + "（调试用）")
                    }
                } else {
                    if (pageLoader.item && pageLoader.item.showMessage) {
                        pageLoader.item.showMessage("无法生成验证码：" + error)
                    }
                }
            }
            
            onPasswordResetResult: {
                if (success) {
                    isOnResetPage = false
                    if (pageLoader.item && pageLoader.item.showErrorMessage) {
                        pageLoader.item.showErrorMessage("密码已重置，请登录")
                    }
                } else {
                    if (pageLoader.item && pageLoader.item.showMessage) {
                        pageLoader.item.showMessage("重置失败：" + error)
                    }
                }
            }
            
            onPasswordUpdateResult: {
                if (success) {
                    if (pageLoader.item && pageLoader.item.showMessage) {
                        pageLoader.item.showMessage("密码已更新")
                    }
                } else {
                    if (pageLoader.item && pageLoader.item.showMessage) {
                        pageLoader.item.showMessage("更新失败：" + error)
                    }
                }
            }
        }
    }

    // 全局居中弹窗：添加好友
    AddContactDialog {
        id: globalAddContactDialog
        x: (window.width - width) / 2
        y: (window.height - height) / 2
        onAddContact: {
            if (isLoggedIn && pageLoader.item && pageLoader.item.addContact) {
                pageLoader.item.addContact(user)
            }
        }
    }

    // 个人信息面板
    ProfilePanel {
        id: globalProfilePanel
        
        // 连接到 SettingsPage 的方法
        openChangePasswordDialog: function() {
            if (pageLoader.item && pageLoader.item.settingsPage) {
                pageLoader.item.settingsPage.openChangePasswordDialog()
            }
        }
        
        openLogoutDialog: function() {
            if (pageLoader.item && pageLoader.item.settingsPage) {
                pageLoader.item.settingsPage.openLogoutDialog()
            }
        }
    }

    // 幽灵移动 (Ghost Move) 核心计时器
    Timer {
        id: ghostMoveTimer
        interval: 10
        repeat: false
        onTriggered: {
            if (isLoggedIn) {
                // 1. 在完全透明状态下执行窗口大小和位置变更（登录→聊天）
                window.width = 900
                window.height = 650
                window.minimumWidth = 900
                window.minimumHeight = 650
                centerWindow()

                // 2. 给予非常短的渲染缓冲，随后淡入
                entranceTimer.start()
            } else {
                // 退出登录流程（聊天→登录）
                window.width = 460
                window.height = 670
                window.minimumWidth = 460
                window.minimumHeight = 670
                centerWindow()
                
                // 确保窗口可见并显示
                window.show()
                window.raise()
                window.requestActivate()
                
                // 淡入登录界面
                entranceTimer.start()
            }
        }
    }

    Timer {
        id: entranceTimer
        interval: 10
        repeat: false
        onTriggered: contentOpacity = 1.0
    }

    function handleLogin(username, password) {
        ws.validateUserLogin(username, password)
    }

    function handleRegister(username, password, nickname) {
        ws.registerNewUser(username, password)
    }

    function handleRequestReset(username) {
        ws.requestPasswordReset(username)
        // 结果将在 onPasswordResetCodeGenerated 中处理
    }

    function handleDoReset(code, newPwd) {
        ws.resetPassword(code, newPwd)
        // 结果将在 onPasswordResetResult 中处理
    }

    function centerWindow() {
        var screen = Qt.application.screens[0]
        window.x = (screen.width - window.width) / 2
        window.y = (screen.height - window.height) / 2
    }

    onIsLoggedInChanged: {
        // 窗口大小和位置的调整已经由 ghostMoveTimer 处理
        // 这里不需要重复设置
    }

    onIsOnRegisterPageChanged: {
        if (!isLoggedIn) {
            window.height = 670
            window.minimumHeight = 670
            centerWindow()
        }
    }

    // 如果已有用户信息或开启自动登录，直接进入聊天界面
    Component.onCompleted: {
        if (ws.currentUserId !== "") {
            isLoggedIn = true
            ws.connectToDefaultChatServer()
        } else if (ws.autoLoginEnabled && ws.lastUsername !== "" && ws.lastPassword !== "") {
            // 尝试自动登录
            handleLogin(ws.lastUsername, ws.lastPassword)
        }
    }

    // 无边框模式下的拉伸支持
    MouseArea {
        width: 8; anchors.top: parent.top; anchors.bottom: parent.bottom; anchors.right: parent.right
        cursorShape: Qt.SizeHorCursor
        visible: isLoggedIn && window.visibility !== Window.Maximized
        property int startX
        onPressed: startX = mouse.x
        onPositionChanged: {
            if (window.width + (mouse.x - startX) > window.minimumWidth) {
                window.width += mouse.x - startX
            }
        }
    }
    MouseArea {
        height: 8; anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom
        cursorShape: Qt.SizeVerCursor
        visible: isLoggedIn && window.visibility !== Window.Maximized
        property int startY
        onPressed: startY = mouse.y
        onPositionChanged: {
            if (window.height + (mouse.y - startY) > window.minimumHeight) {
                window.height += mouse.y - startY
            }
        }
    }
    MouseArea {
        width: 12; height: 12; anchors.right: parent.right; anchors.bottom: parent.bottom
        cursorShape: Qt.SizeFDiagCursor
        visible: isLoggedIn && window.visibility !== Window.Maximized
        property point startPos
        onPressed: startPos = Qt.point(mouse.x, mouse.y)
        onPositionChanged: {
            var newWidth = window.width + (mouse.x - startPos.x)
            var newHeight = window.height + (mouse.y - startPos.y)
            if (newWidth > window.minimumWidth) window.width = newWidth
            if (newHeight > window.minimumHeight) window.height = newHeight
        }
    }
}
