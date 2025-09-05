import QtQuick 2.15
import QtQuick.Controls 2.15
import "../components" as Components

Item {
    id: settingsOverlay
    anchors.fill: parent
    visible: false
    z: 99
    Rectangle {
        id: dimBackground
        anchors.fill: parent
        color: Qt.rgba(0,0,0,0)

        MouseArea {
            anchors.fill: parent
            onClicked: settingsOverlay.hide()
        }
    }

    Rectangle {
        id: settingsWindow
        width: parent.width * 0.9
        height: parent.height * 0.9
        anchors.centerIn: parent
        radius: 12
        color: chatPage.currentTheme.settingsColor
        opacity: 0
        scale: 0.8

        MouseArea { anchors.fill: parent; acceptedButtons: Qt.AllButtons }

        Components.SettingsPanel {
            id: settingsPanel
            anchors.fill: parent
            anchors.margins: 20

            onRequestClose: settingsOverlay.hide()
        }
    }

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

    ColorAnimation {
        id: dimBackgroundAnim
        target: dimBackground
        property: "color"
        duration: 200
    }
}
