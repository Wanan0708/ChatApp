import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12
import ChatApp.Network 1.0

Dialog {
    id: root
    width: 360
    height: 500
    
    modal: true
    dim: true
    
    // 进场/退场动画
    enter: Transition {
        ParallelAnimation {
            OpacityAnimator { from: 0.0; to: 1.0; duration: 400; easing.type: Easing.OutCubic }
            ScaleAnimator { from: 0.92; to: 1.0; duration: 400; easing.type: Easing.OutBack }
        }
    }
    
    exit: Transition {
        ParallelAnimation {
            OpacityAnimator { from: 1.0; to: 0.0; duration: 250; easing.type: Easing.InCubic }
            ScaleAnimator { from: 1.0; to: 0.95; duration: 250; easing.type: Easing.InQuad }
        }
    }

    // 遮罩层自定义
    Overlay.modal: Rectangle {
        color: "#50000000" // 稍深一点，增强沉浸感
        Behavior on opacity { NumberAnimation { duration: 300 } }
    }

    background: Rectangle {
        id: bgRect
        color: "#ffffff"
        radius: 20
        clip: true
        
        // 柔和的大投影
        layer.enabled: true
        layer.effect: DropShadow {
            transparentBorder: true
            color: "#25000000"
            radius: 40
            samples: 35
            verticalOffset: 12
        }
    }

    header: Item {
        height: 64
        
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 32
            anchors.rightMargin: 24
            spacing: 0

            ColumnLayout {
                spacing: 2
                Layout.fillWidth: true
                
                Text {
                    text: "发现新朋友"
                    font.pixelSize: 20
                    font.weight: Font.Bold
                    color: "#0f172a" // Slate 900
                    font.family: "Microsoft YaHei, system-ui"
                }
                Text {
                    text: "输入 ID 或昵称，开启社交新篇章"
                    font.pixelSize: 13
                    color: "#64748b" // Slate 500
                }
            }

            // 时尚关闭按钮
            Rectangle {
                id: closeBtn
                width: 36
                height: 36
                radius: 12
                color: closeBtnMA.containsMouse ? "#f1f5f9" : "transparent"
                
                Text {
                    anchors.centerIn: parent
                    text: "✕"
                    font.pixelSize: 14
                    font.weight: Font.Light
                    color: closeBtnMA.containsMouse ? "#ef4444" : "#94a3b8"
                }
                
                MouseArea {
                    id: closeBtnMA
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: root.close()
                }
                
                Behavior on color { ColorAnimation { duration: 200 } }
                
                RotationAnimation on rotation {
                    running: closeBtnMA.containsMouse
                    from: 0; to: 90; duration: 250; easing.type: Easing.OutBack
                }
            }
        }
    }

    contentItem: ColumnLayout {
        spacing: 20
        anchors.margins: 24

        // 搜索区域
        RowLayout {
            Layout.fillWidth: true
            spacing: 16

            ModernTextField {
                id: searchInput
                Layout.fillWidth: true
                placeholderText: "搜索 ID 或用户名..."
                iconText: "🔍"
                focus: true
                
                Keys.onReturnPressed: performSearch()
            }

            // 渐变主按钮
            Rectangle {
                id: searchBtnAction
                Layout.preferredWidth: 100
                Layout.preferredHeight: 46
                radius: 14
                gradient: Gradient {
                    orientation: Gradient.Horizontal
                    GradientStop { position: 0.0; color: searchBtnActionMA.pressed ? "#4338ca" : "#6366f1" }
                    GradientStop { position: 1.0; color: searchBtnActionMA.pressed ? "#7c3aed" : "#8b5cf6" }
                }

                Text {
                    anchors.centerIn: parent
                    text: "查找"
                    color: "white"
                    font.pixelSize: 14
                    font.weight: Font.DemiBold
                }

                MouseArea {
                    id: searchBtnActionMA
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: performSearch()
                }

                scale: searchBtnActionMA.pressed ? 0.95 : (searchBtnActionMA.containsMouse ? 1.04 : 1.0)
                Behavior on scale { NumberAnimation { duration: 200; easing.type: Easing.OutBack } }
            }
        }

        // 处理来自 ChatService 的搜索结果
        Connections {
            target: ChatService
            onSearchResult: {
                searchResults = results
                isSearching = false
            }
            onIsFriendResult: {
                // 如果需要细化 UI 可以处理
            }
            onFriendRequestSentResult: {
                console.log("[AddContactDialog] Friend request sent result:", success, error)
                if (success) {
                    console.log("[AddContactDialog] Closing dialog")
                    root.close()
                } else {
                    console.log("[AddContactDialog] Request failed:", error)
                    // 可以显示错误提示
                }
            }
        }

        // 状态引导条
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 24
            visible: searchResults.length > 0 || isSearching

            RowLayout {
                spacing: 10
                anchors.left: parent.left
                
                Rectangle {
                    width: 4; height: 16; radius: 2; color: "#8b5cf6"
                }
                
                Text {
                    text: isSearching ? "星际信号探索中..." : "为您找到 " + searchResults.length + " 位潜在联系人"
                    font.pixelSize: 13
                    font.weight: Font.Medium
                    color: "#475569"
                }
            }
        }

        // 滚动列表容器 (卡片式背景)
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#f8fafc" // 浅 Slate 50 背景
            radius: 20
            clip: true
            border.color: "#f1f5f9"
            border.width: 1
            
            ListView {
                id: resultsList
                anchors.fill: parent
                anchors.margins: 12
                model: searchResults
                spacing: 12
                clip: true
                
                delegate: Rectangle {
                    id: resCard
                    width: resultsList.width
                    height: 80
                    color: "white"
                    radius: 16
//                    border.color: resCardMA.containsMouse ? "#e0e7ff" : "transparent"
                    border.width: 1
                    
                    // 动态悬浮投影
//                    layer.enabled: true
//                    layer.effect: DropShadow {
//                        color: resCardMA.containsMouse ? "#15000000" : "#05000000"
//                        radius: resCardMA.containsMouse ? 15 : 8
//                        samples: 15
//                        verticalOffset: resCardMA.containsMouse ? 4 : 2
//                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 16
                        spacing: 16

                        // 现代感头像占位
                        Rectangle {
                            width: 58
                            height: 58
                            radius: 20
                            color: "#f5f3ff"
                            
                            Rectangle {
                                anchors.fill: parent
                                anchors.margins: 2
                                radius: 18
                                color: "white"
                                
                                Text {
                                    anchors.centerIn: parent
                                    text: "👤"
                                    font.pixelSize: 32
                                }
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 4
                            Text {
                                text: modelData.username
                                font.pixelSize: 17
                                font.weight: Font.Bold
                                color: "#1e293b"
                            }
                            Text {
                                text: "@" + modelData.userId
                                font.pixelSize: 13
                                color: "#b4befe"
                                font.family: "Consolas, monospace"
                                font.weight: Font.Medium
                            }
                        }

                        // 功能按钮
                        Rectangle {
                            id: actionBtn
                            implicitWidth: 86
                            implicitHeight: 40
                            radius: 12
                            z: 1  // 确保按钮在上层

                            property bool isFriend: false // 简化处理，或后续通过 friendListLoaded 同步

                            color: isFriend ? "#f1f5f9" : (actionBtnMA.containsMouse ? "#6366f1" : "#eff6ff")

                            Text {
                                anchors.centerIn: parent
                                text: actionBtn.isFriend ? "已相识" : "打招呼"
                                font.pixelSize: 13
                                font.weight: Font.Bold
                                color: actionBtn.isFriend ? "#94a3b8" : (actionBtnMA.containsMouse ? "white" : "#6366f1")
                            }

                            MouseArea {
                                id: actionBtnMA
                                anchors.fill: parent
                                hoverEnabled: !parent.isFriend
                                cursorShape: parent.isFriend ? Qt.ArrowCursor : Qt.PointingHandCursor
                                z: 1  // 确保按钮 MouseArea 在最上层
                                onClicked: {
                                    console.log("[AddContactDialog] Button clicked, userId:", modelData.userId)
                                    console.log("[AddContactDialog] Calling ChatService.sendFriendRequest")
                                    // Simplified: send request, ChatService will handle if already friends
                                    ChatService.sendFriendRequest(modelData.userId)
                                    // 不立即关闭，等请求成功后再关闭
                                    // root.addContact(modelData)
                                    // root.close()
                                }
                            }

                            Behavior on color { ColorAnimation { duration: 200 } }
                        }
                    }

                    // 卡片背景 MouseArea（用于 hover 效果和缩放）
//                    MouseArea {
//                        id: resCardMA
//                        anchors.fill: parent
//                        hoverEnabled: true
//                        z: 0  // 确保在下层

//                        onClicked: {
//                            // 卡片点击处理（如果有）
//                        }
//                    }
                    
//                    scale: resCardMA.pressed ? 0.98 : 1.0
                    Behavior on scale { NumberAnimation { duration: 150; easing.type: Easing.OutBack } }
                }
                
                // 空状态插画风
                ColumnLayout {
                    anchors.centerIn: parent
                    visible: searchResults.length === 0 && !isSearching
                    spacing: 24

                    Text {
                        text: searchInput.text === "" ? "🛰️" : "🔭"
                        font.pixelSize: 72
                        Layout.alignment: Qt.AlignHCenter
                        opacity: 0.15
                    }
                    ColumnLayout {
                        spacing: 8
                        Layout.alignment: Qt.AlignHCenter
                        Text {
                            text: searchInput.text === "" ? "发现新朋友" : "没能捕捉到对方的信号"
                            font.pixelSize: 16
                            font.weight: Font.Bold
                            color: "#94a3b8"
                            Layout.alignment: Qt.AlignHCenter
                        }
                        Text {
                            text: searchInput.text === "" ? "输入对方的专属 ID 或昵称，即刻互粉" : "检查一下拼写是否正确，或尝试搜索其他关键字"
                            font.pixelSize: 13
                            color: "#cbd5e1"
                            horizontalAlignment: Text.AlignHCenter
                            Layout.preferredWidth: 280
                            Layout.alignment: Qt.AlignHCenter
                        }
                    }
                }
            }
        }
    }

    footer: Item { height: 12 }

    property var searchResults: []
    property bool isSearching: false

    signal addContact(var user)

    function performSearch() {
        var term = searchInput.text.trim()
        if (term === "") {
            searchResults = []
            isSearching = false // Ensure isSearching is false if term is empty
            return
        }
        
        isSearching = true
        ChatService.searchUsers(term)
    }

    onOpened: {
        searchInput.text = ""
        searchResults = []
        isSearching = false
        searchInput.forceActiveFocus()
    }
}
