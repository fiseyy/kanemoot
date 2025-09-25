import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: chatArea
    objectName: "chatArea"
    // property string mode: "none" // "dm" / "server" / "none"
    property var serverData: null
    onServerDataChanged: {
        console.log("[ChatArea] Выбран сервер:", serverData ? serverData.name : "null")
    }
    property var dmData: null
    property string chatType: "none"
    anchors.fill: parent

    Rectangle {
        id: chatInfoRectangle
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 72
        color: chatPage.currentTheme.chatInfoRectangle
        visible: chatArea.chatType !== "none"

        Text {
            anchors.centerIn: parent
            text: chatArea.chatType === "server"
                    ? (chatArea.serverData ? chatArea.serverData.name : "Server")
                    : (chatArea.chatType === "dm" ? "Direct message" : "No chat selected")
            color: "white"
        }

    }

    ListView {
        id: messageList
        anchors.top: chatInfoRectangle.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: sendRectangle.top
        visible: chatArea.chatType !== "none"
        model: []
    }

    Rectangle {
        id: sendRectangle
        visible: chatArea.chatType !== "none"
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.leftMargin: 6
        anchors.rightMargin: 14
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 18
        color: chatPage.currentTheme.sendRectangle
        border.color: chatPage.currentTheme.sendRectangleBorder
        border.width: 1
        radius: 15
        height: 40

        TextField {
            id: messageField
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 11
            anchors.rightMargin: 100
            placeholderText: "Send a message..."
            font.family: "Inter"
            font.pixelSize: 14
            color: chatPage.currentTheme.sendText
            placeholderTextColor: chatPage.currentTheme.sendDefaultText
            background: null
        }

        Image {
            id: emojiBtn
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.topMargin: 8
            anchors.rightMargin: 78
            width: 24
            height: 24
            source: chatPage.currentTheme.emojiIcon
            fillMode: Image.PreserveAspectFit
            smooth: true

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: console.log("Emoji button clicked")
                hoverEnabled: true
                onPressed: parent.opacity = 0.5
                onReleased: parent.opacity = 1
                onEntered:  parent.opacity = 0.7
                onExited: parent.opacity = 1
            }
        }

        Image {
            id: addFileBtn
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.topMargin: 8
            anchors.rightMargin: 43
            width: 24
            height: 24
            source: chatPage.currentTheme.fileIcon
            fillMode: Image.PreserveAspectFit
            smooth: true

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: console.log("Add file button clicked")
                hoverEnabled: true
                onPressed: parent.opacity = 0.5
                onReleased: parent.opacity = 1
                onEntered:  parent.opacity = 0.7
                onExited: parent.opacity = 1
            }
        }

        Image {
            id: sendBtn
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.topMargin: 8
            anchors.rightMargin: 8
            width: 24
            height: 24
            source: chatPage.currentTheme.sendIcon
            fillMode: Image.PreserveAspectFit
            smooth: true

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: console.log("Send button clicked")
                hoverEnabled: true
                onPressed: parent.opacity = 0.5
                onReleased: parent.opacity = 1
                onEntered:  parent.opacity = 0.7
                onExited: parent.opacity = 1
            }
        }
    }
}
