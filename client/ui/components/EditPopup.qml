import QtQuick 2.15
import QtQuick.Controls 2.15
import Themes 1.0
import Core 1.0

Popup {
    id: editPopup
    objectName: "editPopup"
    property string username: chatPage.username
    property string userAvatar: chatPage.userAvatar
    property bool statusMenuOpened: false
    width: 336
    height: 280
    background: Rectangle { color: "transparent" }
    opacity: 0
    scale: 0.8
    property bool isOpen: false

    signal logoutRequested()

    function toggle() {
        if (isOpen) { close(); isOpen = false }
        else { open(); isOpen = true }
    }

    onOpened: { opacity = 1; scale = 1; isOpen = true}
    onClosed: { opacity = 0; scale = 0.8; isOpen = false}

    Behavior on opacity { NumberAnimation { duration: 180; easing.type: Easing.InOutQuad } }
    Behavior on scale { NumberAnimation { duration: 180; easing.type: Easing.InOutQuad } }

    Rectangle {
        anchors.fill: parent
        color: chatPage.currentTheme.myInfoRectangle
        border.color: chatPage.currentTheme.myInfoRectangleBorder
        border.width: 1
        radius: 12

        // Верх: аватар + ник + member since
        Row {
            id: topRow
            anchors.top: parent.top
            anchors.topMargin: 16
            anchors.left: parent.left
            anchors.leftMargin: 16

            spacing: 12

            Rectangle {
                width: 60
                height: 60
                radius: 30
                color: "#E0E0E0"
                Image {
                    id: avatarImage
                    anchors.fill: parent
                    smooth: true
                    fillMode: Image.PreserveAspectFit
                }
            }

            Column {
                spacing: 4
                Text {
                    id: usernameText
                    font.family: "Inter"
                    font.pixelSize: 17
                    font.weight: Font.Medium
                    color: chatPage.currentTheme.myNickname
                    text: username
                }
                Text {
                    text: "Member since: year"
                    font.family: "Inter"
                    font.pixelSize: 12
                    color: Qt.rgba(1,1,1,0.3)
                }
            }
        }

        // Кнопки
        Column {
            anchors.top: topRow.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 8

            // задаем максимальную ширину для кнопок
            width: parent.width - 32  // чтобы были отступы по бокам

            // Change status
            Rectangle {
                width: parent.width
                height: 36
                radius: 8
                color: chatPage.currentTheme.myInfoRectangle
                border.color: chatPage.currentTheme.myInfoRectangleBorder
                border.width: 1

                // Текст по центру
                Row {
                    anchors.centerIn: parent
                    spacing: 8
                    Rectangle { width: 16; height: 16; radius: 8; color: "#FFC300" } // кружок
                    Text {
                        text: "Change status"
                        color: chatPage.currentTheme.popupButtonText
                        // font.pixelSize: 14
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true
                    onEntered: parent.color = chatPage.currentTheme.popupButtonHover
                    onExited: parent.color = chatPage.currentTheme.myInfoRectangle
                    onClicked: {
                        if (statusMenuOpened === false) {
                            statusMenu.open()
                            statusMenuOpened = true
                        }
                        else if (statusMenuOpened === true) {
                            statusMenu.close()
                            statusMenuOpened = false
                        }
                    }
                }

                // Подменю справа
                Popup {
                    id: statusMenu
                    x: parent.width + 4
                    y: parent.y
                    width: 140
                    modal: false
                    focus: true
                    onOpened: {
                        statusMenuOpened = true
                    }

                    onClosed: {
                        statusMenuOpened = false
                    }

                    Rectangle {
                        anchors.fill: parent
                        width: parent.width
                        implicitHeight: column.implicitHeight

                        radius: 10
                        color: chatPage.currentTheme.myInfoRectangle
                        border.color: chatPage.currentTheme.myInfoRectangleBorder

                        Column {
                            id: column
                            width: parent.width
                            spacing: 4

                            // пункты подменю
                            Repeater {
                                model: ["Online", "Away", "Do not disturb"]
                                Rectangle {
                                    radius: 8
                                    width: parent.width
                                    height: 30
                                    color: chatPage.currentTheme.myInfoRectangle
                                    border.color: chatPage.currentTheme.myInfoRectangleBorder
                                    border.width: 1
                                    Text {
                                        anchors.centerIn: parent
                                        text: modelData
                                        color: chatPage.currentTheme.popupButtonText
                                    }
                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: { console.log(modelData + " clicked");
                                            statusMenu.close()
                                            statusMenuOpened = false
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }


            // Edit profile
            Rectangle {
                width: parent.width
                height: 36
                radius: 8
                color: chatPage.currentTheme.myInfoRectangle
                border.color: chatPage.currentTheme.myInfoRectangleBorder
                border.width: 1
                Text { anchors.centerIn: parent; text: "Edit profile"; color: chatPage.currentTheme.popupButtonText }
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true
                    onEntered: parent.color = chatPage.currentTheme.popupButtonHover
                    onExited: parent.color = chatPage.currentTheme.myInfoRectangle
                    onClicked: console.log("Edit profile clicked")
                }
            }

            // Log out
            Rectangle {
                width: parent.width
                height: 36
                radius: 8
                color: chatPage.currentTheme.popupLogoutButton
                border.color: chatPage.currentTheme.myInfoRectangleBorder
                border.width: 1
                Text { anchors.centerIn: parent; text: "Log out"; color: chatPage.currentTheme.popupButtonText }
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true
                    onEntered: parent.color = chatPage.currentTheme.popupButtonHover
                    onExited: parent.color = chatPage.currentTheme.popupLogoutButton
                    onClicked: {
                        console.log("Logout");

                        // Анимируем закрытие popup
                        var animPopup = Qt.createQmlObject('import QtQuick 2.0; NumberAnimation { property: "opacity"; duration: 300 }', editPopup);
                        animPopup.target = editPopup;
                        animPopup.from = editPopup.opacity;
                        animPopup.to = 0;
                        animPopup.onStopped.connect(function() {
                            editPopup.close(); // скрываем popup
                            // Анимируем закрытие chatPageContent
                            if (chatPageContent) {
                                var animContent = Qt.createQmlObject('import QtQuick 2.0; NumberAnimation { property: "opacity"; duration: 300 }', chatPageContent);
                                animContent.target = chatPageContent;
                                animContent.from = chatPageContent.opacity;
                                animContent.to = 0;
                                animContent.onStopped.connect(function() {
                                    chatPageContent.visible = false;
                                    // Сигнал в C++ только после завершения анимаций
                                    logoutRequested();
                                });
                                animContent.start();
                            } else {
                                logoutRequested();
                            }
                        });
                        animPopup.start();
                    }
                }
            }


            // Copy user ID
            Rectangle {
                width: parent.width
                height: 36
                radius: 8
                color: chatPage.currentTheme.myInfoRectangle
                border.color: chatPage.currentTheme.myInfoRectangleBorder
                border.width: 1
                Text { anchors.centerIn: parent; text: "Copy user ID"; color: chatPage.currentTheme.popupButtonText }
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true
                    onEntered: parent.color = chatPage.currentTheme.popupButtonHover
                    onExited: parent.color = chatPage.currentTheme.myInfoRectangle
                    onClicked: console.log("Copy user ID clicked")
                }
            }
        }

    }
}
