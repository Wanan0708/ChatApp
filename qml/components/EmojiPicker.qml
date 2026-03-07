import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import ChatApp.Utils 1.0

Rectangle {
    id: root
    width: 360
    height: 420
    color: "#ffffff"
    radius: 8
    border.color: "#e0e0e0"
    border.width: 1

    property string selectedEmoji: ""

    signal emojiSelected(string emoji)

    // 表情数据
    property var categories: EmojiProvider.getCategories()
    property var recentEmojis: EmojiProvider.getRecentEmojis()
    property var favoriteEmojis: EmojiProvider.getFavoriteEmojis()
    property var currentEmojis: []

    // 当前选中的分类
    property int currentCategoryIndex: 0

    // 搜索文本
    property string searchText: ""

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        // 搜索框
        TextField {
            id: searchField
            Layout.fillWidth: true
            placeholderText: "搜索表情..."
            text: root.searchText
            onTextChanged: {
                root.searchText = text
                if (text.trim() !== "") {
                    currentEmojis = EmojiProvider.searchEmojis(text)
                } else {
                    loadCategory(currentCategoryIndex)
                }
            }
        }

        // 分类标签
        ListView {
            id: categoryView
            Layout.fillWidth: true
            Layout.preferredHeight: 36
            orientation: ListView.Horizontal
            spacing: 4
            clip: true

            model: ["最近", "收藏", ...root.categories]

            delegate: Rectangle {
                width: categoryLabel.implicitWidth + 16
                height: 32
                radius: 16
                color: index === root.currentCategoryIndex ? "#4f46e5" : "transparent"

                Text {
                    id: categoryLabel
                    anchors.centerIn: parent
                    text: modelData
                    color: index === root.currentCategoryIndex ? "white" : "#666"
                    font.pixelSize: 13
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        root.currentCategoryIndex = index
                        searchField.text = ""
                        loadCategory(index)
                    }
                }
            }
        }

        // 表情网格
        GridView {
            id: emojiGrid
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            cellWidth: 44
            cellHeight: 44

            model: root.currentEmojis

            delegate: Rectangle {
                width: emojiGrid.cellWidth - 4
                height: emojiGrid.cellHeight - 4
                radius: 8
                color: mouseArea.containsMouse ? "#f0f0f0" : "transparent"

                Text {
                    anchors.centerIn: parent
                    text: modelData.emoji
                    font.pixelSize: 28
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        root.selectedEmoji = modelData.emoji
                        root.emojiSelected(modelData.emoji)
                        EmojiProvider.addRecent(modelData.code)
                    }
                }
            }

            // 滚动条
            ScrollBar.vertical: ScrollBar {
                active: true
            }
        }

        // 底部按钮
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Button {
                Layout.fillWidth: true
                text: "关闭"
                onClicked: root.visible = false
            }
        }
    }

    // 加载分类
    function loadCategory(index) {
        if (index === 0) {
            // 最近
            root.currentEmojis = root.recentEmojis
        } else if (index === 1) {
            // 收藏
            root.currentEmojis = root.favoriteEmojis
        } else {
            // 其他分类
            var category = root.categories[index - 2]
            root.currentEmojis = EmojiProvider.getEmojis(category)
        }
    }

    // 初始化
    Component.onCompleted: {
        loadCategory(0)
    }

    // 监听收藏变化
    Connections {
        target: EmojiProvider
        function onFavoriteChanged() {
            if (root.currentCategoryIndex === 1) {
                root.currentEmojis = root.favoriteEmojis
            }
        }
    }
}
