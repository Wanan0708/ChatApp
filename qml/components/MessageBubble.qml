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
    property bool isOffline: false
    property string errorText: ""
    property string fileName: ""
    property string fileSize: ""
    property string fileUrl: ""
    property string thumbnailUrl: ""
    property string messageId: ""  // 消息 ID，用于撤回
    property string internalMessageId: ""
    
    // 撤回相关
    property bool isRecalled: false
    property bool canRecall: false  // 2 分钟内可撤回

    readonly property real maxWidth: parent.width * 0.75
    readonly property string statusText: {
        if (!root.isSelf) return ""
        if (messageStatus === 3) return "发送失败"
        if (messageStatus === 2) return "已读"
        if (messageStatus === 0) return root.isOffline ? "待发送" : "发送中"
        return "已发送"
    }

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
    signal retryRequested(string messageId)

    readonly property bool showRetrySpinner: root.isSelf && !root.isRecalled && messageStatus === 0
    // 当发送失败（3）或处于离线排队（发送中且 isOffline=true）时，显示黄色感叹并允许点击重发
    readonly property bool showRetryAction: root.isSelf && !root.isRecalled && (messageStatus === 3 || (messageStatus === 0 && isOffline))

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

    Item {
        visible: root.isSelf && (showRetrySpinner || showRetryAction)
        width: 24
        height: 24
        x: contentColumn.x - width - 8
        y: contentColumn.y + Math.max(0, (contentColumn.height - height) / 2)

        // 当处于离线（排队）时，显示更明显的黄色环以提示发送未完成
        Rectangle {
            anchors.fill: parent
            color: "transparent"
            visible: showRetrySpinner && isOffline

            Rectangle {
                anchors.centerIn: parent
                width: 20
                height: 20
                radius: 10
                color: "#fff6e0"
                border.color: "#f59e0b"
                border.width: 2
            }
        }

        BusyIndicator {
            anchors.fill: parent
            running: showRetrySpinner
            visible: showRetrySpinner
        }

        Rectangle {
            visible: showRetryAction
            anchors.centerIn: parent
            width: 18
            height: 18
            radius: 9
            color: "#facc15"
            border.color: "#d97706"
            border.width: 1

            Text {
                anchors.centerIn: parent
                text: "!"
                color: "#7c2d12"
                font.pixelSize: 13
                font.bold: true
            }

            MouseArea {
                id: retryMouseArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: root.retryRequested(root.internalMessageId)
            }

            ToolTip.visible: retryMouseArea.containsMouse && root.errorText !== ""
            ToolTip.text: root.errorText
            ToolTip.delay: 150
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

    Text {
        visible: root.isSelf && statusText !== "" && !showRetrySpinner && !showRetryAction
        text: statusText
        font.pixelSize: 11
        color: messageStatus === 3 ? "#d32f2f" : (messageStatus === 0 ? "#f57c00" : "#888")
        anchors {
            right: timestamp.left
            rightMargin: 6
            verticalCenter: timestamp.verticalCenter
        }
    }
}
