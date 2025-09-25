import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: serverItem
    property var serverData
    signal serverClicked(var serverData)
    width: 56
    height: 56
    radius: 8
    color: chatPage.currentTheme.addServerBtnBg
    border.color: chatPage.currentTheme.addServerBtnBorder
    border.width: 1

    Rectangle {
        id: fallbackAvatar
        anchors.fill: parent
        radius: 8
        color: chatPage.currentTheme.addServerBtnBg
        border.color: chatPage.currentTheme.addServerBtnBorder
        border.width: 1
        visible: !serverAvatarRectangle.visible

        Text {
            id: initialsText
            anchors.centerIn: parent
            color: "white"
            font.pixelSize: 20
            text: {
                if (!serverItem.serverData || !serverItem.serverData.name) return ""
                var words = serverItem.serverData.name.trim().split(" ")
                if (words.length === 1)
                    return words[0][0].toUpperCase()
                else
                    return (words[0][0] + words[1][0]).toUpperCase()
            }
        }
    }

    Rectangle {
        id: serverAvatarRectangle
        border.color: chatPage.currentTheme.addServerBtnBorder
        border.width: 1
        anchors.fill: parent
        visible: false
        Image {
            id: avatar
            anchors.fill: parent
            smooth: true
        }
    }



    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: serverItem.serverClicked(serverItem.serverData)
    }

    onServerDataChanged: {
        if (serverData && serverData.hasOwnProperty("avatar_url") && serverData.avatar_url) {
            avatar.source = serverData.avatar_url
            avatar.visible = true
            fallbackAvatar.visible = false
        } else {
            avatar.source = ""
            avatar.visible = false
            fallbackAvatar.visible = true
        }
    }
}
