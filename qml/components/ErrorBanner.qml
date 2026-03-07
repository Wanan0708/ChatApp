import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

/**
 * @brief 全局错误提示条
 * 
 * 显示在应用顶部的错误提示横幅
 */
Rectangle {
    id: root
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    height: contentColumn.height + 24
    visible: currentError.type !== 0  // ErrorType.Unknown = 0
    
    property var currentError: ({ type: 0, level: 0, message: "", details: "" })
    property color errorColor: getErrorColor(currentError.level)
    
    signal dismissed()
    
    color: errorColor
    border.color: Qt.darker(errorColor, 1.2)
    border.width: 1
    
    function getErrorColor(level) {
        switch (level) {
            case 1: return "#FFF3CD"  // Info - 黄色
            case 2: return "#FEF3C7"  // Warning - 橙色
            case 3: return "#FEE2E2"  // Critical - 红色
            case 4: return "#DC2626"  // Fatal - 深红色
            default: return "#E0E0E0"
        }
    }
    
    function getTextColor(level) {
        switch (level) {
            case 1: return "#856404"
            case 2: return "#B45309"
            case 3: return "#991B1B"
            case 4: return "#FFFFFF"
            default: return "#333333"
        }
    }
    
    Column {
        id: contentColumn
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 12
        spacing: 4
        
        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            
            // 错误图标
            Text {
                Layout.preferredWidth: 24
                Layout.preferredHeight: 24
                text: getErrorIcon(currentError.level)
                font.pixelSize: 20
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            
            // 错误消息
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2
                
                Text {
                    Layout.fillWidth: true
                    text: currentError.message
                    font.pixelSize: 14
                    font.bold: true
                    color: getTextColor(currentError.level)
                    font.family: "Microsoft YaHei, SimSun, sans-serif"
                    wrapMode: Text.Wrap
                }
                
                Text {
                    Layout.fillWidth: true
                    text: currentError.details
                    font.pixelSize: 12
                    color: getTextColor(currentError.level)
                    font.family: "Microsoft YaHei, SimSun, sans-serif"
                    wrapMode: Text.Wrap
                    visible: text !== ""
                }
            }
            
            // 关闭按钮
            Rectangle {
                Layout.preferredWidth: 28
                Layout.preferredHeight: 28
                radius: 14
                color: closeArea.containsMouse ? Qt.darker(root.errorColor, 1.1) : "transparent"
                
                Text {
                    anchors.centerIn: parent
                    text: "×"
                    font.pixelSize: 20
                    color: getTextColor(currentError.level)
                }
                
                MouseArea {
                    id: closeArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        root.dismissed()
                    }
                }
            }
        }
    }
    
    function getErrorIcon(level) {
        switch (level) {
            case 1: return "ℹ️"  // Info
            case 2: return "⚠️"  // Warning
            case 3: return "❌"  // Critical
            case 4: return "🔴"  // Fatal
            default: return "⚪"
        }
    }
    
    // 滑入滑出动画
    states: [
        State {
            name: "visible"
            when: root.visible
            PropertyChanges { target: root; opacity: 1; y: 0 }
        },
        State {
            name: "hidden"
            when: !root.visible
            PropertyChanges { target: root; opacity: 0; y: -root.height }
        }
    ]
    
    transitions: [
        Transition {
            NumberAnimation {
                properties: "opacity,y"
                duration: 300
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
