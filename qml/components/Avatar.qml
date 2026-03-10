import QtQuick 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.12

Item {
    id: root
    width: 40
    height: 40
    property bool isSelf: false
    property string avatarSource: ""
    readonly property bool isLoading: avatarImage.status === Image.Loading
    readonly property bool hasAvatar: avatarSource !== "" && avatarImage.status === Image.Ready

    Rectangle {
        id: avatarFrame
        anchors.fill: parent
        radius: Math.min(width, height) / 2
        color: "#e8f1ff"
        border.color: "#d0e0ff"
        border.width: 1

        Item {
            id: avatarViewport
            anchors.fill: parent
            anchors.margins: avatarFrame.border.width
            visible: root.hasAvatar

            Image {
                id: avatarImage
                anchors.fill: parent
                source: avatarSource
                fillMode: Image.PreserveAspectCrop
                asynchronous: true
                cache: true
                mipmap: true
                smooth: true
                visible: false
            }

            OpacityMask {
                anchors.fill: parent
                source: avatarImage
                maskSource: Rectangle {
                    width: avatarViewport.width
                    height: avatarViewport.height
                    radius: Math.min(width, height) / 2
                    color: "black"
                }
            }
        }

        BusyIndicator {
            anchors.centerIn: parent
            width: parent.width * 0.45
            height: width
            running: root.isLoading
            visible: running
        }

        Text {
            anchors.centerIn: parent
            text: root.isSelf ? "👤" : "👤"
            font.pixelSize: parent.width * 0.5
            visible: !root.hasAvatar && !root.isLoading
        }
    }
}
