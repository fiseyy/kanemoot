import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    width: 56
    height: 56
    radius: 8
    color: chatPage.currentTheme.addServerBtnBg
    border.color: chatPage.currentTheme.addServerBtnBorder
    border.width: 1

    Image {
        anchors.fill: parent
        source: serverData.avatar_url
        smooth: true
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: console.log("Сервер выбран:", serverData.name)
    }
}
