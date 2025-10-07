import QtQuick 2.15
import QtQuick.Controls 2.15
import "../components" as Components

ListView {
    id: listView
    objectName: "chatList"
    anchors.fill: parent
    clip: true
    spacing: 0
    anchors.topMargin: 80

    // Твой модель — тут как было
    model: ListModel { id: chatModel }

    property var currentTheme: null

    delegate: Item {
        width: parent.width
        property int gap: model.hideHeader ? 6 : 14
        height: childItem.implicitHeight + gap

        Loader {
            id: childItem
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            sourceComponent: model.type === "date" ? dateSeparatorComp : messageComp
        }

        Component {
            id: messageComp
            Components.MessageItem {
                username: model.username
                messageText: model.text
                timestamp: model.timestamp
                isOwnMessage: model.isOwn
                hideHeader: model.hideHeader
                currentTheme: listView.currentTheme
            }
        }

        Component {
            id: dateSeparatorComp
            Components.DateSeparatorItem {
                dateText: model.text
                currentTheme: listView.currentTheme
            }
        }
    }

    function _findLastMessageIndex() {
        for (var i = chatModel.count - 1; i >= 0; --i) {
            var it = chatModel.get(i)
            if (it && it.type === "message") return i
        }
        return -1
    }

    function addMessage(msg) {
        console.log("QML.addMessage called, msg =", JSON.stringify(msg))
        var username = msg.username !== undefined ? msg.username
                     : msg.user !== undefined ? msg.user
                     : msg.author !== undefined ? msg.author
                     : "unknown"
        var text = msg.text !== undefined ? msg.text
                : msg.body !== undefined ? msg.body
                : ""
        var timestamp = msg.timestamp !== undefined ? msg.timestamp
                      : msg.ts !== undefined ? msg.ts
                      : ""
        var isOwn = msg.isOwn !== undefined ? msg.isOwn
                  : msg.is_own !== undefined ? msg.is_own
                  : false

        var hideHeader = false
        var lastIndex = _findLastMessageIndex()
        if (lastIndex !== -1) {
            var last = chatModel.get(lastIndex)
            hideHeader = (last.username === username)
        }

        chatModel.append({
            type: "message",
            username: username,
            text: text,
            timestamp: timestamp,
            isOwn: isOwn,
            hideHeader: hideHeader
        })

        listView.positionViewAtEnd()
    }

    function addDateSeparator(dateStr) {
        chatModel.append({
            type: "date",
            text: dateStr
        })
        listView.positionViewAtEnd()
    }
    function clearMessages() {
        chatModel.clear()
    }
}
