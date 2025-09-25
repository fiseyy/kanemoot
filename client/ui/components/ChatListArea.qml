import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: chatListArea
    property string chatType: "none"
    property var serverData
    onServerDataChanged: {
        console.log("[ChatListArea] Выбран сервер:", serverData ? serverData.name : "null")
    }
    property color accentColorTransparent: Qt.rgba(
        Qt.darker(chatPage.accentColor, 0.7).r,
        Qt.darker(chatPage.accentColor, 0.7).g,
        Qt.darker(chatPage.accentColor, 0.7).b,
        0.3
    )

    anchors.topMargin: 72
    anchors.leftMargin: 4

    Column {
        visible: chatType === "dm"
        spacing: 4
    }

    Column {
        id: serverColumn
        visible: chatType === "server"
        spacing: 6
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 8

        Text {
            text: serverData ? serverData.name : "Server"
            font.bold: true
            font.pixelSize: 20
            color: chatPage.currentTheme.defaultPrimaryTextColor
        }

        Text {
            text: "Channels"
            font.bold: true
            font.pixelSize: 14
            color: chatPage.currentTheme.defaultSecondaryTextColor
            anchors.leftMargin: 5
        }

        Repeater {
            model: serverData ? serverData.channels : []
            delegate: Rectangle {
                id: channelRect
                width: parent.width
                height: 28
                radius: 8
                anchors.horizontalCenter: parent.horizontalCenter

                property bool hovered: false

                color: hovered
                       ? chatPage.currentTheme.channelHoverBg
                       : (chatArea.currentChannelId === modelData.id
                          ? Qt.rgba(Qt.darker(chatPage.accentColor, 0.7).r,
                                    Qt.darker(chatPage.accentColor, 0.7).g,
                                    Qt.darker(chatPage.accentColor, 0.7).b,
                                    0.3)
                          : "transparent")

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onEntered: channelRect.hovered = true
                    onExited: channelRect.hovered = false
                    onClicked: {
                        chatArea.currentChannelId = modelData.id
                        chatArea.currentServerId = serverData.id
                        chatArea.chatType = "server"
                        chatArea.currentChannel = modelData
                    }
                }

                Row {
                    spacing: 6
                    anchors.fill: parent
                    anchors.margins: 4
                    anchors.verticalCenter: parent.verticalCenter

                    Image {
                        width: 24
                        height: 24
                        source: (modelData.type === "text" ? chatPage.currentTheme.textChannelImg
                                 : chatPage.currentTheme.voiceChannelImg) || ""
                        fillMode: Image.PreserveAspectFit
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Text {
                        text: modelData.name
                        color: chatPage.currentTheme.channelTextColor
                        font.pixelSize: 15
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
    }

    Text {
        visible: chatType === "none"
        text: "Select a chat"
        anchors.centerIn: parent
        color: "gray"
    }

}
