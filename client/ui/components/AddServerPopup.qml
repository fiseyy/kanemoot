import QtQuick 2.15
import QtQuick.Controls 2.15
import Themes 1.0
import Core 1.0
Item {
    id: addServerOverlay
    objectName: "addServerOverlay"
    anchors.fill: parent
    visible: false
    z: 99
    property bool clearFieldsAfterHide: false
    signal joinServerRequested(string invite)
    signal createServerRequested(string name)

    // --- затемнённый фон ---
    Rectangle {
        id: dimBackground
        anchors.fill: parent
        color: Qt.rgba(0,0,0,0)

        MouseArea {
            anchors.fill: parent
            onClicked: addServerOverlay.hide()
        }
    }

    // --- главное окно ---
    Rectangle {
        id: addServerWindow
        width: parent.width * 0.5
        height: parent.height * 0.4
        anchors.centerIn: parent
        radius: 12
        color: chatPage.currentTheme.settingsColor
        border.color: Qt.rgba(1,1,1,0.1)
        border.width: 1
        opacity: 0
        scale: 0.8

        MouseArea { anchors.fill: parent; acceptedButtons: Qt.AllButtons }

        // --- крестик закрытия ---
        Button {
            id: closeBtn
            width: 32
            height: 32
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.topMargin: 8
            anchors.rightMargin: 8
            background: Rectangle {
                anchors.fill: parent
                radius: width/2
                color: closeBtn.hovered ? Qt.rgba(1,1,1,0.1) : "transparent"
            }
            contentItem: Text {
                text: " ✕"
                anchors.centerIn: parent
                font.pixelSize: 16
                font.bold: true
                color: chatPage.currentTheme.settingsText
            }
            hoverEnabled: true
            onClicked: addServerOverlay.hide()
        }

        Column {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 22

            // --- заголовок ---
            Text {
                text: "Create or Join a Server"
                font.pixelSize: 20
                font.bold: true
                color: chatPage.currentTheme.settingsText
                horizontalAlignment: Text.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }

            // --- блоки Join / Create ---
            Row {
                spacing: 24
                anchors.horizontalCenter: parent.horizontalCenter

                // --- Join Server ---
                Rectangle {
                    width: addServerWindow.width / 2.3
                    height: 160
                    radius: 10
                    color: chatPage.currentTheme.middlePanel
                    border.color: Qt.rgba(1,1,1,0.1)
                    border.width: 1

                    Column {
                        anchors.fill: parent
                        anchors.margins: 16
                        spacing: 12

                        Text {
                            text: "Join Server"
                            font.pixelSize: 16
                            font.bold: true
                            color: chatPage.currentTheme.sendText
                        }

                        TextField {
                            id: joinServerField
                            placeholderText: "Invite link"
                            width: parent.width
                            font.pixelSize: 14
                            height: 36
                            color: chatPage.currentTheme.sendText
                            placeholderTextColor: chatPage.currentTheme.sendDefaultText
                            background: Rectangle {
                                radius: 6
                                color: chatPage.currentTheme.rightPanel
                            }
                        }

                        Button {
                            width: parent.width
                            height: 36
                            background: Rectangle {
                                id: joinBtnBg
                                radius: 6
                                color: "#5865F2"
                            }
                            contentItem: Text {
                                text: "Join"
                                anchors.centerIn: parent
                                color: "#FFFFFF"
                                font.pixelSize: 16
                                font.bold: true
                            }
                            hoverEnabled: true
                            onPressed: joinBtnBg.color = "#4752C4"
                            onReleased: joinBtnBg.color = joinBtnBg.hovered ? "#4752C4" : "#5865F2"
                            onClicked: {
                                console.log("Join server:", joinServerField.text)
                                if (joinServerField.text !== "") {
                                    joinServerRequested(joinServerField.text)
                                    clearFieldsAfterHide = true
                                    addServerOverlay.hide()
                                }
                            }
                        }
                    }
                }

                // --- Create Server ---
                Rectangle {
                    width: addServerWindow.width / 2.3
                    height: 160
                    radius: 10
                    color: chatPage.currentTheme.middlePanel
                    border.color: Qt.rgba(1,1,1,0.1)
                    border.width: 1

                    Column {
                        anchors.fill: parent
                        anchors.margins: 16
                        spacing: 12

                        Text {
                            text: "Create Server"
                            font.pixelSize: 16
                            font.bold: true
                            color: chatPage.currentTheme.sendText
                        }

                        TextField {
                            id: createServerField
                            placeholderText: "Server name"
                            width: parent.width
                            font.pixelSize: 14
                            height: 36
                            color: chatPage.currentTheme.sendText
                            placeholderTextColor: chatPage.currentTheme.sendDefaultText
                            background: Rectangle {
                                radius: 6
                                color: chatPage.currentTheme.rightPanel
                            }
                        }

                        Button {
                            width: parent.width
                            height: 36
                            background: Rectangle {
                                id: createBtnBg
                                radius: 6
                                color: "#43B581"
                            }
                            contentItem: Text {
                                text: "Create"
                                anchors.centerIn: parent
                                color: "#FFFFFF"
                                font.pixelSize: 16
                                font.bold: true
                            }
                            hoverEnabled: true
                            onPressed: createBtnBg.color = "#369970"
                            onReleased: createBtnBg.color = createBtnBg.hovered ? "#369970" : "#43B581"
                            onClicked: {
                                console.log("Create server:", createServerField.text)
                                if (createServerField.text !== "") {
                                    createServerRequested(createServerField.text)
                                    clearFieldsAfterHide = true
                                    addServerOverlay.hide()
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // --- функции показа/скрытия ---
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

    // --- анимации ---
    NumberAnimation {
        id: windowOpacityAnim
        target: addServerWindow
        property: "opacity"
        duration: 200
        easing.type: Easing.OutQuad
        onStopped: {
            if (windowOpacityAnim.to === 0) {
                addServerOverlay.visible = false
                if (clearFieldsAfterHide) {
                    joinServerField.text = ""
                    createServerField.text = ""
                    clearFieldsAfterHide = false
                }
            }
        }
    }

    NumberAnimation {
        id: windowScaleAnim
        target: addServerWindow
        property: "scale"
        duration: 200
        easing.type: Easing.OutQuad
    }

    ColorAnimation {
        id: dimBackgroundAnim
        target: dimBackground
        property: "color"
        duration: 200
    }
}
