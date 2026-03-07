import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import ChatApp.Network 1.0

/**
 * @brief 网络状态指示器
 * 
 * 显示当前网络连接状态，支持断网提示
 */
Rectangle {
    id: root
    width: parent.width
    height: visible ? 40 : 0
    visible: !ChatService.isConnected
    color: "#FEF3C7"
    border.color: "#F59E0B"
    border.width: 1

    property string statusText: ChatService.isConnected ? "已连接" : "未连接"
    property bool canReconnect: true

    signal reconnectClicked

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 12

        // 状态图标
        Text {
            text: visible ? "⚠" : "✓"
            font.pixelSize: 20
            Layout.preferredWidth: 24
            horizontalAlignment: Text.AlignHCenter
        }

        // 状态文本
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2

            Text {
                Layout.fillWidth: true
                text: "网络连接已断开"
                font.pixelSize: 14
                font.bold: true
                color: "#92400E"
                font.family: "Microsoft YaHei, SimSun, sans-serif"
            }

            Text {
                Layout.fillWidth: true
                text: "请检查网络设置，或点击重试按钮重新连接"
                font.pixelSize: 12
                color: "#B45309"
                font.family: "Microsoft YaHei, SimSun, sans-serif"
            }
        }

        // 重连按钮
        Button {
            Layout.preferredWidth: 80
            Layout.preferredHeight: 32

            visible: root.canReconnect
            text: "重试"
            font.pixelSize: 13
            font.family: "Microsoft YaHei, SimSun, sans-serif"

            background: Rectangle {
                radius: 6
                color: parent.pressed ? "#1E40AF" : "#3B82F6"
            }

            contentItem: Text {
                text: parent.text
                font.pixelSize: parent.font.pixelSize
                font.family: parent.font.family
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            onClicked: {
                root.reconnectClicked()
                ChatService.connectToDefaultChatServer()
            }
        }
    }

    // 淡入淡出动画
    states: [
        State {
            name: "visible"
            when: root.visible
            PropertyChanges { target: root; opacity: 1 }
        },
        State {
            name: "hidden"
            when: !root.visible
            PropertyChanges { target: root; opacity: 0 }
        }
    ]

    transitions: [
        Transition {
            NumberAnimation {
                property: "opacity"
                duration: 300
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
