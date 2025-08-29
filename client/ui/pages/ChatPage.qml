import QtQuick 2.15
import QtQuick.Controls 2.15
import "../components" as Components

Item {
    id: chatPage
    anchors.fill: parent

    Rectangle {
        anchors.fill: parent
        color: "#222"

        // Левая панель
        Rectangle {
            id: leftPanel
            width: 66
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            color: "#272729"

            Image {
                id: kanemootLogo
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.leftMargin: 5
                anchors.topMargin: 7
                width: 56
                height: 56
                source: "qrc:///assets/logo.png"
                fillMode: Image.PreserveAspectFit
                smooth: true
            }
        }

        // Средняя панель
        Rectangle {
            id: middlePanel
            width: 336
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: leftPanel.right
            color: "#242424"
            Rectangle {
                id: searchRectangle
                anchors.top: middlePanel.top
                anchors.left: middlePanel.left
                anchors.topMargin: 13
                anchors.leftMargin: 4
                color: "#383838"
                radius: 8
                width: 278
                height: 46
                Image {
                    id: searchIcon
                    anchors.top: searchRectangle.top
                    anchors.left: searchRectangle.left
                    width: 27
                    height: 27
                    anchors.leftMargin: 10
                    anchors.topMargin: 9
                    fillMode: Image.PreserveAspectFit
                    source: "qrc:///assets/search.svg"
                    smooth: true
                }
                Text {
                    id: searchText
                    anchors.top: searchRectangle.top
                    anchors.left: searchRectangle.left
                    font.family: "Inter"
                    color: "#999"
                    font.pixelSize: 14
                    anchors.leftMargin: 42
                    anchors.topMargin: 14
                    text: "Search"
                }
                Rectangle {
                    id: hotkeyRectangle
                    anchors.top: searchRectangle.top
                    anchors.left: searchRectangle.left
                    anchors.leftMargin: 211
                    anchors.topMargin: 5
                    color: "#333333"
                    width: 61
                    height: 35
                    radius: 5
                    Text {
                        id: hotkeyText
                        anchors.top: hotkeyRectangle.top
                        anchors.left: hotkeyRectangle.left
                        font.family: "Inter"
                        color: "#888888"
                        font.pixelSize: 14
                        anchors.leftMargin: 5
                        anchors.topMargin: 9
                        text: "Ctrl + K"
                    }
                }
            }

            Rectangle {
                id: discoverRectangle
                anchors.left: middlePanel.left
                anchors.top: middlePanel.top
                anchors.leftMargin: 286
                anchors.topMargin: 13
                width: 46
                height: 46
                radius: 7
                color: "#383838"
                Image {
                    id: discoverIcon
                    anchors.top: discoverRectangle.top
                    anchors.left: discoverRectangle.left
                    width: 28
                    height: 28
                    anchors.leftMargin: 9
                    anchors.topMargin: 9
                    fillMode: Image.PreserveAspectFit
                    source: "qrc:///assets/explore.svg"
                    smooth: true
                }
            }
            Text {
                id: dmText
                anchors.left: middlePanel.left
                anchors.top: middlePanel.top
                anchors.leftMargin: 14
                anchors.topMargin: 78
                font.family: "Inter"
                color: "#A7A7A7"
                font.pixelSize: 14
                text: "Direct Messages"
            }

            // DMS

            Rectangle {
                id: myInfoRectangle
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.topMargin: 634
                height: 72
                color: "#292929"
                Rectangle {
                    id: myAvatarRectangle
                    radius: 8
                    anchors.top: myInfoRectangle.top
                    anchors.left: myInfoRectangle.left
                    anchors.topMargin: 8
                    anchors.leftMargin: 8
                    width: 55
                    height: 55
                    color: "#E0E0E0"
                    Image {
                        id: myAvatar
                        anchors.fill: parent
                        // source: ""
                        smooth: true
                    }
                }
                Text {
                    id: myNickname
                    anchors.top: parent.top
                    anchors.left: parent.left
                    font.family: "Inter"
                    font.pixelSize: 20
                    font.weight: Font.Medium
                    color: "#FFF"
                    text: "Test"
                    anchors.topMargin: 15
                    anchors.leftMargin: 71
                }
                Text {
                    id: myStatusText
                    anchors.top: parent.top
                    anchors.left: parent.left
                    font.family: "Inter"
                    font.pixelSize: 15
                    font.weight: Font.Regular
                    color: "#FFC300" // в зависимости от статуса
                    anchors.topMargin: 39
                    anchors.leftMargin: 71
                    text: "Non-active" // временно
                }
                Rectangle {
                    id: myStatusIcon
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.topMargin: 47
                    anchors.leftMargin: 47
                    width: 16
                    height: 16
                    radius: width / 2
                    color: "#FFC300"
                }
                Image {
                    id: muteMicroBtn
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.topMargin: 26
                    anchors.leftMargin: 212
                    width: 25
                    height: 25
                    source: "qrc:///assets/mic.svg"
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                }
                Image {
                    id: muteHeadsetBtn
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.topMargin: 26
                    anchors.leftMargin: 251
                    width: 25
                    height: 25
                    source: "qrc:///assets/headset.svg"
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                }
                Image {
                    id: settingsBtn
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.topMargin: 26
                    anchors.leftMargin: 290
                    width: 25
                    height: 25
                    source: "qrc:///assets/settings_old.svg"
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                }
            }

        }

        // Правая панель
        Rectangle {
            id: rightPanel
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: middlePanel.right
            anchors.right: parent.right
            color: "#1e1e1e"
            Rectangle {
                id: chatInfoRectangle // будь то инфо о человеке, или открытом канале сервера
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: 72
                color: "#262626"
            }
            Rectangle {
                id: sendRectangle
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.leftMargin: 6
                anchors.rightMargin: 14
                anchors.topMargin: 648
                color: "#292929"
                radius: 15
                height: 40
                Text {
                    id: sendText
                    text: "Send a message..."
                    font.family: "Inter"
                    font.weight: Font.Regular
                    font.pixelSize: 14
                    color: "#C0C0C0"
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.topMargin: 11
                    anchors.leftMargin: 11
                }
                Image {
                    id: emojiBtn
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.topMargin: 8
                    anchors.leftMargin: 705
                    width: 24
                    height: 24
                    source: "qrc:///assets/Emoji.svg"
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                }
                Image {
                    id: addFileBtn
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.topMargin: 8
                    anchors.leftMargin: 740
                    width: 24
                    height: 24
                    source: "qrc:///assets/File.svg"
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                }
                Image {
                    id: sendBtn
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.topMargin: 8
                    anchors.leftMargin: 775
                    width: 24
                    height: 24
                    source: "qrc:///assets/send.svg"
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                }
            }
        }
    }
}
