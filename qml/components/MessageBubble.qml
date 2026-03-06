import QtQuick 2.12

Item {
    id: root
    width: parent.width
    height: bubble.height + timestamp.height + 20
    property string content: "Hello"
    property bool isSelf: false
    property var timestamp: Date.now()
    property string timeAlignment: "left"

    readonly property real maxWidth: parent.width * 0.75

    // 气泡
    Rectangle {
        id: bubble
        x: root.isSelf ? (parent.width - width - 12) : 12
        y: 8
        width: Math.min(textItem.implicitWidth + 24, root.maxWidth)
        height: textItem.height + 16
        radius: 12
        color: root.isSelf ? "#DCF8C6" : "#ECECEC"
        smooth: true

        Text {
            id: textItem
            anchors.centerIn: parent
            text: root.content
            wrapMode: Text.Wrap
            width: parent.width - 24
            font.pixelSize: 14
            color: "#333"
            font.family: "Microsoft YaHei, SimSun, sans-serif"
        }
    }

    // 时间戳（简单定位，无锚定）
    Text {
        id: timestamp
        // 后端返回的是秒时间戳，需要转换为毫秒
        text: {
            var ts = root.timestamp
            // 如果时间戳是 10 位数字（秒），转换为毫秒
            if (ts && ts < 10000000000) {
                ts = ts * 1000
            }
            Qt.formatDateTime(new Date(ts), "hh:mm:ss")
        }
        font.pixelSize: 11
        color: "#888"
        x: bubble.x + (bubble.width - width) / 2  // 气泡水平居中
        y: bubble.y + bubble.height + 4

        anchors {
            bottom: parent.bottom
            left: timeAlignment === "left" ? parent.left : undefined
            leftMargin: timeAlignment === "left" ? 20 : 0
            right: timeAlignment === "right" ? parent.right : undefined
            rightMargin: timeAlignment === "right" ? 20 : 0
            margins: 4
        }
        horizontalAlignment: timeAlignment === "right" ? Text.AlignRight : Text.AlignLeft
    }
}
