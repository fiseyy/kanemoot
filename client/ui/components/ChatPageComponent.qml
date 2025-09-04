import QtQuick 2.15
import QtQuick.Controls 2.15
import "../components" as Components
import Themes 1.0

Item {
    id: chatPageContent
    anchors.fill: parent
    property var currentTheme: DarkTheme
    property string openedChatType: "dm" // "server" / "dm"

    property bool micMuted: false
    property bool headsetMuted: false

    Rectangle {
        anchors.fill: parent
        color: "#292929"

        // Левая панель
        Rectangle {
            id: leftPanel
            width: 66
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            // color: "#272729"
            color: currentTheme.leftPanel

            Image {
                id: kanemootLogo
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.leftMargin: 5
                anchors.topMargin: 7
                width: 56
                height: 56
                source: currentTheme.smallLogoIcon
                fillMode: Image.PreserveAspectFit
                smooth: true
                layer.enabled: true
            }
        }

        // Средняя панель
        Rectangle {
            id: middlePanel
            width: 336
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: leftPanel.right
            color: currentTheme.middlePanel
            Rectangle {
                id: searchRectangle
                anchors.top: middlePanel.top
                anchors.left: middlePanel.left
                anchors.topMargin: 13
                anchors.leftMargin: 4
                color: currentTheme.searchRectangle
                radius: 8
                width: 278
                height: 46

                Image {
                    id: searchIcon
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    width: 27
                    height: 27
                    fillMode: Image.PreserveAspectFit
                    source: currentTheme.searchIcon
                    smooth: true
                }

                TextField {
                    id: searchField
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: searchIcon.right
                    anchors.leftMargin: 8
                    anchors.right: hotkeyRectangle.left
                    anchors.rightMargin: 8
                    placeholderText: "Search"
                    font.family: "Inter"
                    font.pixelSize: 14
                    color: currentTheme.searchText
                    background: null
                }

                Rectangle {
                    id: hotkeyRectangle
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 5
                    color: currentTheme.hotkeyRectangle
                    width: 61
                    height: 35
                    radius: 5

                    Text {
                        anchors.centerIn: parent
                        font.family: "Inter"
                        font.pixelSize: 14
                        color: currentTheme.hotkeyText
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
                color: currentTheme.discoverRectangle

                Image {
                    id: discoverIcon
                    anchors.centerIn: parent
                    width: 28
                    height: 28
                    fillMode: Image.PreserveAspectFit
                    source: currentTheme.discoverIcon
                    smooth: true
                }

                MouseArea {
                    id: discoverArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        console.log("Discover clicked")
                    }
                    onPressed: parent.opacity = 0.5
                    onReleased: parent.opacity = 1
                    onEntered: parent.opacity = 0.7
                    onExited: parent.opacity = 1
                }
            }


            Text {
                id: dmText
                anchors.left: middlePanel.left
                anchors.top: middlePanel.top
                anchors.leftMargin: 14
                anchors.topMargin: 78
                font.family: "Inter"
                color: currentTheme.dmText
                font.pixelSize: 14
                // text: "Direct Messages"
                text: chatPageContent.openedChatType === "dm" ? "Direct Messages" : "Server"
            }

            // DMS

            Rectangle {
                id: myInfoRectangle
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                height: 72
                color: currentTheme.myInfoRectangle
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
                        // source: "qrc:///assets/account.svg"
                        smooth: true
                        width: 55
                        height: 55
                    }
                }
                Text {
                    id: myNickname
                    anchors.top: parent.top
                    anchors.left: parent.left
                    font.family: "Inter"
                    font.pixelSize: 20
                    font.weight: Font.Medium
                    color: currentTheme.myNickname
                    text: chatPage.username
                    anchors.topMargin: 15
                    anchors.leftMargin: 71
                }
                Text {
                    id: myStatusText
                    anchors.top: parent.top
                    anchors.left: parent.left
                    font.family: "Inter"
                    font.pixelSize: 15
                    // font.weight: Font.Regular
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
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    source: micMuted ? currentTheme.mutedMicIcon : currentTheme.micIcon

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true

                        onClicked: {
                            micMuted = !micMuted
                            console.log("Mic muted:", micMuted)
                        }

                        onPressed: parent.opacity = 0.5
                        onReleased: parent.opacity = 1
                        onEntered: parent.opacity = 0.7
                        onExited: parent.opacity = 1
                    }
                }

                Image {
                    id: muteHeadsetBtn
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.topMargin: 26
                    anchors.leftMargin: 251
                    width: 25
                    height: 25
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    source: headsetMuted ? currentTheme.mutedHeadsetIcon : currentTheme.headsetIcon

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true

                        onClicked: {
                            headsetMuted = !headsetMuted
                            console.log("Headset muted:", headsetMuted)
                        }

                        onPressed: parent.opacity = 0.5
                        onReleased: parent.opacity = 1
                        onEntered: parent.opacity = 0.7
                        onExited: parent.opacity = 1
                    }
                }

                Image {
                    id: settingsBtn
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.topMargin: 26
                    anchors.leftMargin: 290
                    width: 25
                    height: 25
                    source: currentTheme.settingsIcon
                    fillMode: Image.PreserveAspectFit
                    smooth: true

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: settingsOverlay.show()
                        hoverEnabled: true
                        onPressed: parent.opacity = 0.5
                        onReleased: parent.opacity = 1
                        onEntered:  parent.opacity = 0.7
                        onExited: parent.opacity = 1
                    }
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
            color: currentTheme.rightPanel
            Rectangle {
                id: chatInfoRectangle // будь то инфо о человеке, или открытом канале сервера
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: 72
                color: currentTheme.chatInfoRectangle
            }
            Rectangle {
                id: sendRectangle
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.leftMargin: 6
                anchors.rightMargin: 14
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 18
                color: currentTheme.sendRectangle
                radius: 15
                height: 40

                TextField {
                    id: messageField
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 11
                    anchors.rightMargin: 100  // место под кнопки справа
                    placeholderText: "Send a message..."
                    font.family: "Inter"
                    font.pixelSize: 14
                    color: currentTheme.sendText
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
                    source: currentTheme.emojiIcon
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
                    source: currentTheme.fileIcon
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
                    source: currentTheme.sendIcon
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
    }
    Item {
        id: settingsOverlay
        anchors.fill: parent
        visible: false
        z: 99

        // Полупрозрачный фон
        Rectangle {
            id: dimBackground
            anchors.fill: parent
            color: Qt.rgba(0,0,0,0)

            MouseArea {
                anchors.fill: parent
                onClicked: settingsOverlay.hide()
            }
        }

        // Основное окно настроек
        Rectangle {
            id: settingsWindow
            width: parent.width * 0.9
            height: parent.height * 0.9
            anchors.centerIn: parent
            radius: 12
            color: currentTheme.settingsColor
            opacity: 0
            scale: 0.8

            MouseArea { anchors.fill: parent; acceptedButtons: Qt.AllButtons }

            Column {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 10

                Text { text: "Settings"; font.pixelSize: 20; color: currentTheme.sendText }

                Button { text: "Close"; onClicked: settingsOverlay.hide() }
            }
        }

        // Методы управления
        function show() {
            visible = true

            dimBackgroundAnim.to = Qt.rgba(0,0,0,0.5)
            windowOpacityAnim.to = 1
            windowScaleAnim.to = 1

            dimBackgroundAnim.start()
            windowOpacityAnim.start()
            windowScaleAnim.start()
        }

        function hide() {
            dimBackgroundAnim.to = Qt.rgba(0,0,0,0)
            windowOpacityAnim.to = 0
            windowScaleAnim.to = 0.8

            dimBackgroundAnim.start()
            windowOpacityAnim.start()
            windowScaleAnim.start()
        }

        // Анимация для окна
        NumberAnimation {
            id: windowOpacityAnim
            target: settingsWindow
            property: "opacity"
            duration: 200
            easing.type: Easing.OutQuad
            onStopped: {
                if (windowOpacityAnim.to === 0) {
                    settingsOverlay.visible = false
                }
            }
        }

        NumberAnimation {
            id: windowScaleAnim
            target: settingsWindow
            property: "scale"
            duration: 200
            easing.type: Easing.OutQuad
        }

        // Анимация для фона
        ColorAnimation {
            id: dimBackgroundAnim
            target: dimBackground
            property: "color"
            duration: 200
        }
    }

}
