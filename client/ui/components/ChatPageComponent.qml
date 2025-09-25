import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt5Compat.GraphicalEffects
import "../components" as Components
import Themes 1.0

Item {
    id: chatPageContent
    objectName: "chatPageContent"
    anchors.fill: parent
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
            color: chatPage.currentTheme.leftPanel

            Image {
                id: kanemootLogo
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.leftMargin: 5
                anchors.topMargin: 7
                width: 56
                height: 56
                source: chatPage.currentTheme.smallLogoIcon
                fillMode: Image.PreserveAspectFit
                smooth: true
                layer.enabled: true
                visible: false
            }

            ColorOverlay {
                anchors.fill: kanemootLogo
                source: kanemootLogo
                color: chatPage.accentColor
            }
            Column {
                id: userServersColumn
                objectName: "userServersColumn"
                anchors.top: leftPanel.top
                anchors.left: leftPanel.left
                anchors.leftMargin: 5
                anchors.topMargin: 70
                spacing: 10
            }
            Rectangle {
                id: addServerBtn
                width: 50
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 18
                radius: 8
                color: chatPage.currentTheme.addServerBtnBg
                border.color: chatPage.currentTheme.addServerBtnBorder
                border.width: 1

                Image {
                    anchors.centerIn: parent
                    source: chatPage.currentTheme.addServerIcon  // плюсик
                    width: 48
                    height: 48
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true

                    onClicked: addServerPopup.show()

                    onPressed: addServerBtn.opacity = 0.5
                    onReleased: addServerBtn.opacity = 1
                    onEntered: addServerBtn.opacity = 0.7
                    onExited: addServerBtn.opacity = 1
                }
            }


        }

        // Средняя панель
        Rectangle {
            id: middlePanel
            width: 336
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: leftPanel.right
            color: chatPage.currentTheme.middlePanel
            Rectangle {
                id: searchRectangle
                anchors.top: middlePanel.top
                anchors.left: middlePanel.left
                anchors.topMargin: 13
                anchors.leftMargin: 4
                color: chatPage.currentTheme.searchRectangle
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
                    source: chatPage.currentTheme.searchIcon
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
                    color: chatPage.currentTheme.searchText
                    placeholderTextColor: chatPage.currentTheme.searchDefaultText
                    background: null
                }

                Rectangle {
                    id: hotkeyRectangle
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 5
                    color: chatPage.currentTheme.hotkeyRectangle
                    width: 61
                    height: 35
                    radius: 5

                    Text {
                        anchors.centerIn: parent
                        font.family: "Inter"
                        font.pixelSize: 14
                        color: chatPage.currentTheme.hotkeyText
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
                color: chatPage.currentTheme.discoverRectangle

                Image {
                    id: discoverIcon
                    anchors.centerIn: parent
                    width: 28
                    height: 28
                    fillMode: Image.PreserveAspectFit
                    source: chatPage.currentTheme.discoverIcon
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
                color: chatPage.currentTheme.dmText
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
                anchors.bottomMargin: 10
                anchors.leftMargin: 5
                anchors.rightMargin: 5
                radius: 8
                height: 62
                color: chatPage.currentTheme.myInfoRectangle
                border.color: chatPage.currentTheme.myInfoRectangleBorder
                border.width: 1
                Rectangle {
                    id: myAvatarRectangle
                    radius: 8
                    anchors.top: myInfoRectangle.top
                    anchors.left: myInfoRectangle.left
                    anchors.topMargin: 8
                    anchors.leftMargin: 8
                    width: 45
                    height: 45
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
                Rectangle {
                    id: myInfoEditRectangle
                    anchors.top: myInfoRectangle.top
                    anchors.left: myInfoRectangle.left
                    anchors.bottom: myInfoRectangle.bottom
                    radius: 8
                    width: 210
                    color: "transparent"
                    Rectangle {
                        id: myInfoEditHover
                        color: "transparent"
                        anchors.fill: parent
                        anchors.margins: 5
                        radius: 8
                    }

                    Text {
                        id: myNickname
                        anchors.top: parent.top
                        anchors.left: parent.left
                        font.family: "Inter"
                        font.pixelSize: 17
                        font.weight: Font.Medium
                        color: chatPage.currentTheme.myNickname
                        text: chatPage.username
                        anchors.topMargin: 13
                        anchors.leftMargin: 71
                    }
                    Text {
                        id: myStatusText
                        anchors.top: parent.top
                        anchors.left: parent.left
                        font.family: "Inter"
                        font.pixelSize: 13
                        // font.weight: Font.Regular
                        // color: "#FFC300" // в зависимости от статуса
                        color: "#78797f"
                        anchors.topMargin: 34
                        anchors.leftMargin: 71
                        text: "Non-active" // временно
                    }
                    Rectangle {
                        id: myStatusIcon
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.topMargin: 37
                        anchors.leftMargin: 37
                        width: 16
                        height: 16
                        radius: width / 2
                        color: "#FFC300"
                    }
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked: {
                                if (editPopup.justClosed) {
                                    // игнорируем клик, иначе он откроет popup сразу после закрытия
                                    return;
                                }
                                editPopup.toggle();
                            }
                        onEntered: myInfoEditHover.color = chatPage.currentTheme.myInfoEditHoverColor
                        onExited: myInfoEditHover.color = "transparent"
                    }
                    Components.EditPopup {
                        id: editPopup
                        x: myInfoRectangle.x - 10
                        y: myInfoEditRectangle.y - 5 - height
                    }
                }
                Image {
                    id: muteMicroBtn
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.topMargin: 20
                    anchors.leftMargin: 212
                    width: 25
                    height: 25
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    source: micMuted ? chatPage.currentTheme.mutedMicIcon : chatPage.currentTheme.micIcon

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
                    anchors.topMargin: 20
                    anchors.leftMargin: 251
                    width: 25
                    height: 25
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    source: headsetMuted ? chatPage.currentTheme.mutedHeadsetIcon : chatPage.currentTheme.headsetIcon

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
                    anchors.topMargin: 20
                    anchors.leftMargin: 290
                    width: 25
                    height: 25
                    source: chatPage.currentTheme.settingsIcon
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
            color: chatPage.currentTheme.rightPanel
            Rectangle {
                id: chatInfoRectangle // будь то инфо о человеке, или открытом канале сервера
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: 72
                color: chatPage.currentTheme.chatInfoRectangle
            }
            Rectangle {
                id: sendRectangle
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
                    anchors.rightMargin: 100  // место под кнопки справа
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
    }
    Components.SettingsOverlay {
        id: settingsOverlay
    }
    Components.AddServerPopup {
        id: addServerPopup
    }

}
