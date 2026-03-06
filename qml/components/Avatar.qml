import QtQuick 2.12

Item {
    id: root
    width: 40
    height: 40
    property bool isSelf: false
    property string avatarSource: ""

    Rectangle {
        anchors.centerIn: parent
        width: parent.width * 0.9
        height: parent.height * 0.9
        radius: width / 2
        color: "#e8f1ff"
        border.color: "#d0e0ff"
        border.width: 1
        clip: true

        Image {
            anchors.centerIn: parent
            source: avatarSource
            width: parent.width - 4
            height: parent.height - 4
            fillMode: Image.PreserveAspectCrop
            visible: avatarSource !== ""
        }

        Text {
            anchors.centerIn: parent
            text: root.isSelf ? "👤" : "👤"
            font.pixelSize: parent.width * 0.5
            visible: avatarSource === ""
        }
    }
}
