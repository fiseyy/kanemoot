import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: chatListArea

    property var serverOptionsModel: [
        qsTr("Server settings"),
        qsTr("Invite people"),
        qsTr("Create channel"),
        qsTr("Notification settings"),
        qsTr("Leave server")
    ]

    property string chatType: "none"
    property var serverData

    property bool serverPopupSelected: false
    onServerDataChanged: {
        console.log("[ChatListArea] Выбран сервер:", serverData ? serverData.name : "null")
    }
    property color accentColorTransparent: Qt.rgba(
        Qt.darker(chatPage.accentColor, 0.7).r,
        Qt.darker(chatPage.accentColor, 0.7).g,
        Qt.darker(chatPage.accentColor, 0.7).b,
        0.3
    )

    anchors.topMargin: 72
    anchors.leftMargin: 4

    Column {
        visible: chatType === "dm"
        spacing: 4
    }

    Column {
        id: serverColumn
        visible: chatType === "server"
        spacing: 6
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 8

        Rectangle {
            id: serverHeader
            width: parent.width
            height: 32
            radius: 6
            color: (hovered || serverPopupSelected)
                   ? chatPage.currentTheme.channelHoverBg
                   : "transparent"

            property bool hovered: false

            RowLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 6

                Text {
                    text: serverData ? serverData.name : "Server"
                    font.bold: true
                    font.pixelSize: 16
                    color: chatPage.currentTheme.defaultPrimaryTextColor
                    verticalAlignment: Text.AlignVCenter
                }

                Item {
                    Layout.fillWidth: true
                }

                Image {
                    width: 20
                    height: 20
                    fillMode: Image.PreserveAspectFit
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    source: serverPopupSelected
                            ? chatPage.currentTheme.closeIcon
                            : chatPage.currentTheme.chevronDown
                }

            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onEntered: serverHeader.hovered = true
                onExited: serverHeader.hovered = false
                onClicked: {
                    chatListArea.serverPopupSelected = true
                    serverOptionsPopup.open()
                }
            }

            Popup {
                    id: serverOptionsPopup
                    x: (chatListArea.width - width) / 2
                    y: serverHeader.y + serverHeader.height + 4
                    modal: true
                    focus: true
                    closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape
                    dim: false

                    // --- анимация появления ---
                    enter: Transition {
                        NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 140; easing.type: Easing.OutCubic }
                        ScaleAnimator { from: 0.9; to: 1.0; duration: 140; easing.type: Easing.OutCubic }
                    }
                    exit: Transition {
                        NumberAnimation { property: "opacity"; from: 1; to: 0; duration: 100; easing.type: Easing.InCubic }
                        ScaleAnimator { from: 1.0; to: 0.95; duration: 100; easing.type: Easing.InCubic }
                    }
                    onClosed: {
                        chatListArea.serverPopupSelected = false
                    }
                    background: Rectangle {
                        color: chatPage.currentTheme.addServerBtnBg
                        border.color: chatPage.currentTheme.addServerBtnBorder
                        radius: 6
                    }

                    Column {
                        spacing: 2
                        padding: 4

                        Repeater {
                            model: chatListArea.serverOptionsModel

                            delegate: Rectangle {
                                width: 180
                                height: 28
                                radius: 4
                                color: "transparent"

                                Text {
                                    anchors.centerIn: parent
                                    text: modelData
                                    color: chatPage.currentTheme.defaultPrimaryTextColor
                                    font.pixelSize: 14
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: {
                                        console.log(modelData, "clicked for server", serverData ? serverData.name : "")
                                        serverOptionsPopup.close()
                                    }
                                    onEntered: parent.color = chatPage.currentTheme.channelHoverBg
                                    onExited: parent.color = "transparent"
                                }
                            }
                        }
                    }
                }
        }


        Text {
            text: "Channels"
            font.bold: true
            font.pixelSize: 14
            color: chatPage.currentTheme.defaultSecondaryTextColor
            anchors.leftMargin: 5
        }

        Repeater {
            model: serverData ? serverData.channels : []
            delegate: Rectangle {
                id: channelRect
                width: parent.width
                height: 28
                radius: 8
                anchors.horizontalCenter: parent.horizontalCenter

                property bool hovered: false

                color: hovered
                       ? chatPage.currentTheme.channelHoverBg
                       : (chatArea.currentChannelId === modelData.id
                          ? Qt.rgba(Qt.darker(chatPage.accentColor, 0.7).r,
                                    Qt.darker(chatPage.accentColor, 0.7).g,
                                    Qt.darker(chatPage.accentColor, 0.7).b,
                                    0.3)
                          : "transparent")

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onEntered: channelRect.hovered = true
                    onExited: channelRect.hovered = false
                    onClicked: {
                        chatArea.currentChannelId = modelData.id
                        chatArea.currentServerId = serverData.id
                        chatArea.chatType = "server"
                        chatArea.currentChannel = modelData
                    }
                }

                Row {
                    spacing: 6
                    anchors.fill: parent
                    anchors.margins: 4
                    anchors.verticalCenter: parent.verticalCenter

                    Image {
                        width: 24
                        height: 24
                        source: (modelData.type === "text" ? chatPage.currentTheme.textChannelImg
                                 : chatPage.currentTheme.voiceChannelImg) || ""
                        fillMode: Image.PreserveAspectFit
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Text {
                        text: modelData.name
                        color: chatPage.currentTheme.channelTextColor
                        font.pixelSize: 15
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
    }

    Text {
        visible: chatType === "none"
        text: "Select a chat"
        anchors.centerIn: parent
        color: "gray"
    }

}
