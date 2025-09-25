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

    Image {
        id: avatar
        anchors.fill: parent
        source: ""
        smooth: true
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: serverItem.serverClicked(serverItem.serverData)
    }

    onServerDataChanged: {
        if (serverData && serverData.hasOwnProperty("avatar_url")) {
            avatar.source = serverData.avatar_url
        } else {
            avatar.source = ""
        }
    }

}
