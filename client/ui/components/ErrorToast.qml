import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: toast
    width: parent ? parent.width * 0.4 : 300
    height: 80
    radius: 10
    color: "#cc2e2e2e"
    anchors.right: parent ? parent.right : undefined
    anchors.top: parent ? parent.top : undefined
    anchors.margins: 20
    visible: false
    z: 1000
    opacity: 0.0
    signal dismissed()
    Image {
        source: "qrc:///assets/icons/error.png"
        width: 24
        height: 24
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 16
    }

    Column {
        width: toast.width * 0.85
        spacing: 4
        anchors.left: parent.left
        anchors.leftMargin: 52
        anchors.verticalCenter: parent.verticalCenter

        Text {
            text: toast.title
            font.bold: true
            font.pixelSize: 16
            color: "white"
        }

        Text {
            text: toast.message
            font.pixelSize: 13
            color: "#dddddd"
            wrapMode: Text.Wrap
            width: parent.width
        }
    }

    property string title: ""
    property string message: ""

    Timer {
        id: hideTimer
        interval: 3000
        repeat: false
        onTriggered: toastBehaviorReverse.start()
    }

    function show(_title, _message, durationMs = 3000) {
        title = _title
        message = _message
        toast.opacity = 0.0
        toast.visible = true
        toastShowAnimation.start()

        hideTimer.stop()
        hideTimer.interval = durationMs
        hideTimer.start()
    }

    NumberAnimation {
        id: toastShowAnimation
        target: toast
        property: "opacity"
        from: 0.0
        to: 1.0
        duration: 200
    }

    NumberAnimation {
        id: toastBehaviorReverse
        target: toast
        property: "opacity"
        from: 1.0
        to: 0.0
        duration: 300
        onStopped: {
            toast.visible = false
            toast.dismissed()
        }
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (toastBehaviorReverse.running)
                return;
            hideTimer.stop();
            toastBehaviorReverse.start();
        }
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
    }

}
