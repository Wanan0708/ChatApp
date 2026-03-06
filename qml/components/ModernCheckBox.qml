import QtQuick 2.12
import QtQuick.Controls 2.12

CheckBox {
    id: control
    
    indicator: Rectangle {
        implicitWidth: 22
        implicitHeight: 22
        x: control.leftPadding
        y: parent.height / 2 - height / 2
        radius: 6
        border.color: control.checked ? "#3b82f6" : "#cbd5e1"
        color: control.checked ? "#3b82f6" : "transparent"
        
        Behavior on border.color { ColorAnimation { duration: 200 } }
        Behavior on color { ColorAnimation { duration: 200 } }
        
        Text {
            anchors.centerIn: parent
            text: "✓"
            font.pixelSize: 14
            color: "white"
            visible: control.checked
        }
    }
    
    contentItem: Text {
        text: control.text
        font.pixelSize: 14
        font.family: "\"Microsoft YaHei\", sans-serif"
        color: "#64748b"
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
    }
}
