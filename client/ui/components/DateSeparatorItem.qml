import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: root
    property string dateText: ""
    property var currentTheme: null

    width: parent ? parent.width : 300
    height: 24

    Row {
        anchors.fill: parent
        anchors.margins: 0
        spacing: 6
        anchors.horizontalCenter: parent.horizontalCenter

        Rectangle {
            width: (parent.width - dateLabel.width - 12)/2
            height: 1
            color: currentTheme ? currentTheme.dateSeparatorColor : "#555"
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            id: dateLabel
            text: root.dateText
            font.family: "Inter"
            font.pixelSize: 12
            color: currentTheme ? currentTheme.dateSeparatorColor : "#aaa"
            anchors.verticalCenter: parent.verticalCenter
        }

        Rectangle {
            width: (parent.width - dateLabel.width - 12)/2
            height: 1
            color: currentTheme ? currentTheme.dateSeparatorColor : "#555"
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
