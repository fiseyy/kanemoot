import QtQuick 2.15
import QtQuick.Controls 2.15
import "components"
ApplicationWindow {
    id: rootWindow
    width: 1229
    height: 706
    minimumWidth: 800
    minimumHeight: 600
    visible: true
    title: qsTr("KANEMOOT")
    Item {
        id: pageContainer
        objectName: "pageContainer"
        anchors.fill: parent
    }
    ErrorToastManager {
        id: errorToastManager
        anchors.fill: parent
    }
    function handleError(title, message) {
        errorToastManager.enqueue(title, message)
    }
}
