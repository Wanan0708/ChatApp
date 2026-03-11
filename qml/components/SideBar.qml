import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: sidebar
    width: 68
    color: "#FFFFFF"

    // 删除无用的 shader，使用右侧边框作为分隔
    Rectangle {
        width: 1
        color: "#E2E8F0" // 更柔和的边框颜色
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
    }

    // 现代主题颜色
    property color primaryColor: "#3B82F6"      // 现代感蓝
    property color primaryLight: "#EFF6FF"      // 选中项的浅蓝背景
    property color hoverColor: "#F3F4F6"        // 基础悬浮灰
    property color textColor: "#6B7280"         // 未选中文字灰色
    property color iconColor: "#9CA3AF"         // 图标默认色
    
    // 当前选中的页面索引 (0=消息, 1=联系人, 2=通知, 3=设置)
    property int currentPageIndex: 0

    signal messagesClicked()
    signal contactsClicked()
    signal mailClicked()
    signal moreClicked()

    // 可复用的按钮组件
    Component {
        id: iconButtonComponent
        Item {
            id: buttonWrapper
            width: 48
            height: 48
            anchors.horizontalCenter: parent.horizontalCenter

            property string icon: "?"
            property string iconSource: ""
            property string tooltipText: ""
            property int buttonIndex: 0
            signal clicked()

            Rectangle {
                id: buttonRect
                anchors.centerIn: parent
                width: 44
                height: 44
                radius: 12  // 更大的默认圆角，类似卡片
                color: {
                    if (sidebar.currentPageIndex === buttonWrapper.buttonIndex) return sidebar.primaryLight
                    if (mouseArea.pressed) return Qt.darker(sidebar.hoverColor, 1.05)
//                    if (mouseArea.containsMouse) return sidebar.hoverColor
                    return "transparent"
                }

                // 添加平滑过渡动画
//                Behavior on color {
//                    ColorAnimation { duration: 150 }
//                }

                // 添加缩放点击效果
                scale: mouseArea.pressed ? 0.92 : (mouseArea.containsMouse ? 1.02 : 1.0)
                Behavior on scale {
                    NumberAnimation { duration: 100; easing.type: Easing.OutQuad }
                }

                // 使用图片图标
                Image {
                    anchors.centerIn: parent
                    source: buttonWrapper.iconSource
                    width: 24
                    height: 24
                    fillMode: Image.PreserveAspectFit
                    opacity: sidebar.currentPageIndex === buttonWrapper.buttonIndex ? 1.0 : 0.6

                    Behavior on opacity {
                        NumberAnimation { duration: 150 }
                    }
                }

                // 左侧指示条 (优化为胶囊状并使用平滑动画)
                Rectangle {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    width: 3
                    height: sidebar.currentPageIndex === buttonWrapper.buttonIndex ? 24 : 0
                    color: sidebar.primaryColor
                    radius: 2
                    opacity: sidebar.currentPageIndex === buttonWrapper.buttonIndex ? 1.0 : 0.0

                    Behavior on height {
                        NumberAnimation { duration: 300; easing.type: Easing.OutElastic; easing.amplitude: 1.5; easing.period: 0.5 }
                    }
                    Behavior on opacity {
                        NumberAnimation { duration: 200 }
                    }
                }
            }

            // Tooltip
            ToolTip {
                visible: mouseArea.containsMouse
                text: buttonWrapper.tooltipText
                timeout: 1000
                x: buttonWrapper.width + 5
                y: (buttonWrapper.height - height) / 2
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    buttonWrapper.clicked()
                }
                cursorShape: Qt.PointingHandCursor
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 16
        anchors.margins: 10
        anchors.topMargin: 20
        anchors.bottomMargin: 20

        // 顶部按钮组
        Column {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            spacing: 8
            width: 48 // 匹配组件宽度

            Loader {
                anchors.horizontalCenter: parent.horizontalCenter
                sourceComponent: iconButtonComponent
                onLoaded: {
                    item.iconSource = "qrc:/new/prefix1/image/message.png"
                    item.tooltipText = "消息"
                    item.buttonIndex = 0
                    item.clicked.connect(function() {
                        sidebar.messagesClicked()
                    })
                }
            }

            Loader {
                anchors.horizontalCenter: parent.horizontalCenter
                sourceComponent: iconButtonComponent
                onLoaded: {
                    item.iconSource = "qrc:/new/prefix1/image/contact.png"
                    item.tooltipText = "联系人"
                    item.buttonIndex = 1
                    item.clicked.connect(function() {
                        sidebar.contactsClicked()
                    })
                }
            }
        }

        // 扩展空间，把底部按钮推到最下面
        Item { Layout.fillHeight: true }

        // 底部按钮组
        Column {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            spacing: 8
            width: 48

            Loader {
                anchors.horizontalCenter: parent.horizontalCenter
                sourceComponent: iconButtonComponent
                onLoaded: {
                    item.iconSource = "qrc:/new/prefix1/image/notification.png"
                    item.tooltipText = "通知"
                    item.buttonIndex = 2
                    item.clicked.connect(function() {
                        sidebar.mailClicked()
                    })
                }
            }

            Loader {
                anchors.horizontalCenter: parent.horizontalCenter
                sourceComponent: iconButtonComponent
                onLoaded: {
                    item.iconSource = "qrc:/new/prefix1/image/setting.png"
                    item.tooltipText = "设置"
                    item.buttonIndex = 3
                    item.clicked.connect(function() {
                        sidebar.moreClicked()
                    })
                }
            }
        }
    }
}
