import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: root
    property string username: ""
    property string messageText: ""
    property string timestamp: ""
    property bool isOwnMessage: false
    property bool hideHeader: false
    property var currentTheme: null

    width: parent ? parent.width : 400
    implicitHeight: column.implicitHeight

    readonly property int avatarSize: 36
    readonly property int hPadding: 12

    Column {
        id: column
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 8
        spacing: 4

        // Header (имя + время). height = 0 когда скрыт, чтобы не занимать место.
        Item {
            id: header
            width: parent.width
            height: hideHeader ? 0 : 18
            visible: !hideHeader

            Text {
                id: userText
                text: username
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 13
                font.bold: true
                color: currentTheme ? currentTheme.usernameColor : "#888"
                visible: !isOwnMessage && !hideHeader
            }

            Text {
                id: timeText
                text: timestamp ? (timestamp.length >= 16 ? timestamp.slice(11,16) : timestamp) : ""
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 11
                color: currentTheme ? currentTheme.timestampColor : "#aaa"
            }
        }

        // Ряд с пузырём и аватарками.
        Item {
            id: bubbleRow
            width: parent.width
            // учитываем либо высоту пузыря, либо аватара, чтобы не накладывались
            height: Math.max(bubble.implicitHeight, avatarSize)

            // левый аватар (для чужих)
            Rectangle {
                id: avatarLeft
                visible: !isOwnMessage
                width: avatarSize; height: avatarSize; radius: avatarSize/2
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                color: "transparent"
                Image { anchors.fill: parent; source: currentTheme ? currentTheme.defaultAvatar : ""; fillMode: Image.PreserveAspectCrop }
            }

            // правый аватар (для своих)
            Rectangle {
                id: avatarRight
                visible: isOwnMessage
                width: avatarSize; height: avatarSize; radius: avatarSize/2
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                color: "transparent"
                Image { anchors.fill: parent; source: currentTheme ? currentTheme.defaultAvatar : ""; fillMode: Image.PreserveAspectCrop }
            }

            // Пузырь сообщения
            Rectangle {
                id: bubble
                anchors.verticalCenter: parent.verticalCenter

                // привязка влево/вправо
                anchors.left: isOwnMessage ? undefined : parent.left
                anchors.right: isOwnMessage ? parent.right : undefined

                // резервируем место под аватарки через margins
                anchors.leftMargin: !isOwnMessage ? avatarSize + 10 : 10
                anchors.rightMargin: isOwnMessage ? avatarSize + 10 : 10

                color: isOwnMessage ? (currentTheme?.accent ?? "#3a86ff") : (currentTheme?.surfaceVariant ?? "#e5e5e5")
                radius: 10

                // ширина = текст + паддинги, но не больше 72% родителя
                width: Math.min(messageTextItem.implicitWidth + hPadding*2, root.width * 0.72)
                height: messageTextItem.implicitHeight + 12

                Text {
                    id: messageTextItem
                    text: messageText
                    anchors.fill: parent
                    anchors.margins: hPadding
                    wrapMode: Text.Wrap
                    font.pixelSize: 14
                    color: isOwnMessage ? "white" : "black"
                }
            }
        }
    }
}
