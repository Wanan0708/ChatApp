import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Item {
    id: root
    width: parent.width
    height: contentColumn.height + timestamp.height + 16
    property string content: "Hello"
    property bool isSelf: false
    property var timestamp: Date.now()
    property string timeAlignment: "left"
    
    // 新增：消息类型和状态
    property int messageType: 0  // 0=文本，1=图片，2=文件，3=系统
    property int messageStatus: 1  // 0=发送中，1=已发送，2=已读，3=失败
    property string fileName: ""
    property string fileSize: ""
    property string fileUrl: ""
    property string thumbnailUrl: ""
    property string messageId: ""  // 消息 ID，用于撤回
    
    // 撤回相关
    property bool isRecalled: false
    property bool canRecall: false  // 2 分钟内可撤回

    readonly property real maxWidth: parent.width * 0.75

    // 气泡颜色
    readonly property string bubbleColor: {
        if (isRecalled) return "#f5f5f5"
        if (messageType === 3) return "#FFF3CD"  // 系统消息
        return root.isSelf ? "#DCF8C6" : "#ECECEC"
    }
    
    // 右键菜单
    Menu {
        id: contextMenu
        MenuItem {
            text: "撤回消息"
            visible: root.isSelf && root.canRecall && !root.isRecalled
            onTriggered: {
                // 调用撤回
                if (root.messageId) {
                    // 需要通过父级调用 ChatService.recallMessage
                    root.messageRecalled()
                }
            }
        }
    }
    
    signal messageRecalled()

    // 长按显示菜单
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        onPressed: {
            contextMenu.popup()
        }
    }

    Column {
        id: contentColumn
        x: root.isSelf ? (parent.width - width - 12) : 12
        y: 8
        width: contentWidth
        spacing: 4

        readonly property real contentWidth: {
            if (messageType === 1) {
                // 图片消息
                return Math.min(imageItem.sourceSize.width + 24, root.maxWidth)
            } else if (messageType === 2) {
                // 文件消息
                return Math.min(fileRow.implicitWidth + 24, root.maxWidth)
            } else {
                // 文本消息
                return Math.min(textItem.implicitWidth + 24, root.maxWidth)
            }
        }

        // 消息状态指示器（仅文本消息显示）
        Row {
            visible: messageType === 0 && root.isSelf
            spacing: 4
            anchors.right: parent.right

            // 发送状态图标
            Text {
                visible: messageStatus === 0  // 发送中
                text: "⏳"
                font.pixelSize: 12
                
                BusyIndicator {
                    anchors.centerIn: parent
                    width: 14
                    height: 14
                    running: visible
                }
            }

            Text {
                visible: messageStatus === 1  // 已发送
                text: "✓"
                font.pixelSize: 12
                color: "#4CAF50"
            }

            Text {
                visible: messageStatus === 2  // 已读
                text: "✓✓"
                font.pixelSize: 12
                color: "#2196F3"
            }

            Text {
                visible: messageStatus === 3  // 失败
                text: "⚠"
                font.pixelSize: 12
                color: "#F44336"
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        console.log("重发消息")
                        // TODO: 触发重发
                    }
                }
            }
        }

        // 图片消息
        Item {
            id: imageContainer
            visible: messageType === 1
            width: visible ? contentColumn.contentWidth - 24 : 0
            height: visible ? Math.min(imageItem.sourceSize.height, 200) : 0

            Rectangle {
                anchors.fill: parent
                radius: 8
                color: "#f5f5f5"
                border.color: "#e0e0e0"
                border.width: 1

                Image {
                    id: imageItem
                    anchors.fill: parent
                    anchors.margins: 1
                    source: root.thumbnailUrl || root.fileUrl
                    fillMode: Image.PreserveAspectFit
                    cache: true
                    clip: true

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            console.log("打开图片:", root.fileUrl)
                            // TODO: 打开图片查看器
                        }
                    }
                }

                // 加载指示器
                BusyIndicator {
                    anchors.centerIn: parent
                    running: imageItem.status === Image.Loading
                    visible: running
                }

                // 加载失败提示
                Text {
                    visible: imageItem.status === Image.Error
                    text: "图片加载失败"
                    anchors.centerIn: parent
                    font.pixelSize: 12
                    color: "#999"
                }
            }
        }

        // 文件消息
        RowLayout {
            id: fileRow
            visible: messageType === 2
            spacing: 8

            Rectangle {
                Layout.preferredWidth: 40
                Layout.preferredHeight: 40
                radius: 8
                color: root.isSelf ? "#C8E6C9" : "#E3F2FD"

                Text {
                    anchors.centerIn: parent
                    text: "📄"
                    font.pixelSize: 20
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2

                Text {
                    Layout.fillWidth: true
                    text: root.fileName || "未知文件"
                    font.pixelSize: 13
                    font.bold: true
                    color: "#333"
                    elide: Text.ElideMiddle
                    font.family: "Microsoft YaHei, SimSun, sans-serif"
                }

                Text {
                    text: root.fileSize || ""
                    font.pixelSize: 11
                    color: "#888"
                    font.family: "Microsoft YaHei, SimSun, sans-serif"
                }
            }

            // 下载按钮
            Rectangle {
                Layout.preferredWidth: 32
                Layout.preferredHeight: 32
                radius: 16
                color: "#f5f5f5"

                Text {
                    anchors.centerIn: parent
                    text: "⬇"
                    font.pixelSize: 16
                    color: "#2196F3"
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        console.log("下载文件:", root.fileUrl)
                        // TODO: 下载文件
                        Qt.openUrlExternally(root.fileUrl)
                    }
                }
            }
        }

        // 文本消息
        Text {
            id: textItem
            visible: messageType === 0 || messageType === 3
            wrapMode: Text.Wrap
            width: parent.width - 24
            text: isRecalled ? "消息已撤回" : root.content
            font.pixelSize: 14
            color: isRecalled ? "#999" : (messageType === 3 ? "#856404" : "#333")  // 已撤回用灰色
            font.family: "Microsoft YaHei, SimSun, sans-serif"
            font.italic: isRecalled
        }
    }

    // 时间戳
    Text {
        id: timestamp
        text: {
            var ts = root.timestamp
            if (ts && ts < 10000000000) {
                ts = ts * 1000
            }
            Qt.formatDateTime(new Date(ts), "hh:mm:ss")
        }
        font.pixelSize: 11
        color: "#888"
        x: contentColumn.x + (contentColumn.width - width) / 2
        y: contentColumn.y + contentColumn.height + 4

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
