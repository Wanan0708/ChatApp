import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12

TextField {
    id: control
    property string iconText: ""
    
    font.pixelSize: 15
    font.family: "\"Microsoft YaHei\", sans-serif"
    color: "#1e293b"
    selectByMouse: true
    leftPadding: iconRect.width + 12
    rightPadding: 16
    
    placeholderTextColor: "#94a3b8"
    
    background: Rectangle {
        implicitHeight: 52
        radius: 12
        color: control.activeFocus ? "#ffffff" : "#f1f5f9"
        border.color: control.activeFocus ? "#3b82f6" : "#e2e8f0"
        border.width: control.activeFocus ? 2 : 1
        
        Behavior on border.color { ColorAnimation { duration: 200 } }
        Behavior on color { ColorAnimation { duration: 200 } }
        
        layer.enabled: control.activeFocus
        layer.effect: DropShadow {
            transparentBorder: true
            color: "#203b82f6"
            radius: 15
            samples: 25
            verticalOffset: 4
        }
    }
    
    Rectangle {
        id: iconRect
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        width: 44
        height: 52
        color: "transparent"
        
        Text {
            anchors.centerIn: parent
            text: control.iconText
            font.pixelSize: 18
            color: control.activeFocus ? "#3b82f6" : "#64748b"
            Behavior on color { ColorAnimation { duration: 200 } }
        }
    }
}
