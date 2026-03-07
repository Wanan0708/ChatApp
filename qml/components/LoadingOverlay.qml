import QtQuick 2.12
import QtQuick.Controls 2.12
import "../components"

/**
 * @brief 全局加载遮罩
 * 
 * 显示半透明遮罩和加载动画，阻止用户交互
 */
Rectangle {
    id: root
    anchors.fill: parent
    visible: loading
    opacity: visible ? 1 : 0
    color: "#80000000"  // 半透明黑色

    property bool loading: false
    property string loadingText: "加载中..."

    // 点击遮罩不传递事件
    MouseArea {
        anchors.fill: parent
        enabled: root.loading
    }

    // 居中加载内容
    Column {
        anchors.centerIn: parent
        spacing: 20
        width: 200

        // 加载动画
        LoadingSpinner {
            anchors.horizontalCenter: parent.horizontalCenter
            width: 50
            height: 50
            loading: root.loading
            color: "#ffffff"
            strokeWidth: 4
        }

        // 加载文本
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.loadingText
            font.pixelSize: 14
            color: "#ffffff"
            font.family: "Microsoft YaHei, SimSun, sans-serif"
        }
    }

    // 淡入淡出动画
    states: [
        State {
            name: "visible"
            when: root.loading
            PropertyChanges { target: root; opacity: 1 }
        },
        State {
            name: "hidden"
            when: !root.loading
            PropertyChanges { target: root; opacity: 0 }
        }
    ]

    transitions: [
        Transition {
            NumberAnimation {
                property: "opacity"
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
