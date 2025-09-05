import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: settingsItem
    property string title: ""
    property string subtitle: ""
    property bool enabled: false

    width: parent ? parent.width : 400
    height: 64

    Row {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12

        // Тексты слева
        Column {
            spacing: 2
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width - toggleWrapper.width - 24

            Text {
                text: settingsItem.title
                font.pixelSize: 16
                font.bold: true
                color: chatPage.currentTheme.settingsText
                elide: Text.ElideRight
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: settingsItem.enabled = !settingsItem.enabled
                }
            }

            Text {
                text: settingsItem.subtitle
                font.pixelSize: 13
                color: Qt.lighter(chatPage.currentTheme.settingsText, 1.5)
                wrapMode: Text.WordWrap
                elide: Text.ElideRight
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: settingsItem.enabled = !settingsItem.enabled
                }
            }
        }

        // Тумблер справа
        Item {
            id: toggleWrapper
            width: 52
            height: 28
            anchors.verticalCenter: parent.verticalCenter

            property color onColor: chatPage.currentTheme.sliderOnColor
            property color offColor: chatPage.currentTheme.sliderOffColor

            Rectangle {
                id: track
                anchors.fill: parent
                radius: height / 2
                color: settingsItem.enabled ? toggleWrapper.onColor : toggleWrapper.offColor
                Behavior on color { ColorAnimation { duration: 200 } }
            }

            Rectangle {
                id: thumb
                width: toggleWrapper.height - 4
                height: toggleWrapper.height - 4
                radius: width / 2
                anchors.verticalCenter: parent.verticalCenter
                x: settingsItem.enabled ? parent.width - width - 2 : 2
                color: "white"
                border.color: Qt.rgba(0, 0, 0, 0.2)

                Behavior on x { NumberAnimation { duration: 200; easing.type: Easing.InOutQuad } }
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: settingsItem.enabled = !settingsItem.enabled
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
}
