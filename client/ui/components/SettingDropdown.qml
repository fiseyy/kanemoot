import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: dropdown
    property string title: ""
    property var model: []
    property int currentIndex: 0

    width: parent ? parent.width : 400
    height: 64

    Row {
        anchors.fill: parent
        anchors.margins: 12
        // spacing: 12

        // Текст слева
        Item {
            width: parent.width - selector.width - 12
            height: parent.height

            Text {
                anchors.topMargin: 12
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 0
                anchors.fill: parent
                text: dropdown.title
                font.pixelSize: 16
                font.bold: true
                color: chatPage.currentTheme.settingsText
                elide: Text.ElideRight
            }
        }

        // Селектор справа
        Rectangle {
            id: selector
            width: 150
            height: parent.height
            anchors.verticalCenter: parent.verticalCenter
            radius: 6
            color: "transparent"
            border.color: chatPage.currentTheme.settingsLineColor

            Row {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 8

                Text {
                    id: currentText
                    text: dropdown.model.length > 0 ? dropdown.model[dropdown.currentIndex].text : ""
                    font.pixelSize: 14
                    color: chatPage.currentTheme.settingsText
                    anchors.verticalCenter: parent.verticalCenter
                    elide: Text.ElideRight
                }

                Text {
                    text: popup.visible ? "▲" : "▼"
                    font.pixelSize: 12
                    color: chatPage.currentTheme.settingsText
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: popup.open()
            }
        }
    }

    // Нижняя линия
    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1
        color: chatPage.currentTheme.settingsLineColor
    }

    // Popup
    Popup {
        id: popup
        width: selector.width
        modal: false
        focus: true
        x: selector.x
        y: selector.y - 20
        opacity: 0.0
        scale: 0.9
        background: Rectangle { color: "transparent" }

        Behavior on opacity { NumberAnimation { duration: 180; easing.type: Easing.InOutQuad } }
        Behavior on scale { NumberAnimation { duration: 180; easing.type: Easing.InOutQuad } }

        onVisibleChanged: {
            if (visible) { opacity = 1.0; scale = 1.0 }
            else { opacity = 0.0; scale = 0.9 }
        }

        Rectangle {
            id: backgroundRect
            width: parent.width
            implicitHeight: column.implicitHeight
            radius: 8
            color: chatPage.currentTheme.middlePanel

            Column {
                id: column
                width: parent.width
                spacing: 2

                Repeater {
                    model: dropdown.model

                    Rectangle {
                        width: parent.width
                        height: 36
                        radius: 6
                        color: index === dropdown.currentIndex
                            ? chatPage.accentColor
                            : chatPage.currentTheme.middlePanel

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.leftMargin: 8
                            text: modelData.text
                            font.pixelSize: 14
                            color: chatPage.currentTheme.settingsText
                        }

                        MouseArea {
                            id: itemArea
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            hoverEnabled: true
                            onClicked: {
                                dropdown.currentIndex = index
                                popup.close()
                            }

                            Rectangle { // hover overlay
                                anchors.fill: parent
                                color: itemArea.containsMouse ? Qt.rgba(1,1,1,0.05) : "transparent"
                            }
                        }
                    }
                }
            }
        }
    }
}
