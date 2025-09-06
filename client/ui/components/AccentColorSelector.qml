import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: accentColorSelector
    width: parent ? parent.width : 400
    implicitHeight: 160

    property color currentAccent: chatPage.accentColor

    Column {
        width: parent.width
        spacing: 12
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 12

        // Заголовок
        Text {
            text: "Акцентный цвет"
            font.pixelSize: 16
            font.bold: true
            color: chatPage.currentTheme.settingsText
        }

        // 6 стандартных цветов
        Row {
            spacing: 12
            Repeater {
                model: [ "#00C7D9", "#FF6B6B", "#FFD93D", "#6BCB77", "#4D96FF", "#9B59B6" ]
                delegate: Rectangle {
                    width: 30
                    height: 30
                    radius: width / 2
                    color: modelData
                    border.width: 2
                    border.color: Qt.rgba(1,1,1,0.3)

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            currentAccent = modelData
                            chatPage.accentColor = modelData
                        }
                    }
                }
            }
        }

        // Свой цвет
        Row {
            spacing: 8

            Text {
                text: "Или сделайте свой:"
                font.pixelSize: 14
                color: chatPage.currentTheme.settingsText
                y: customColorBtn.y + 3

            }

            Rectangle {
                id: customColorBtn
                width: 30
                height: 30
                radius: width / 2
                color: currentAccent
                border.width: 2
                border.color: Qt.rgba(1,1,1,0.3)

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: colorPopup.open()
                }
            }

            // Popup с выбором цвета
            Popup {
                id: colorPopup
                modal: true
                focus: true
                x: customColorBtn.x
                y: customColorBtn.y + customColorBtn.height + 10

                Rectangle {
                    width: 220
                    height: 180
                    color: chatPage.currentTheme.middlePanel
                    radius: 8
                    border.width: 1
                    border.color: chatPage.currentTheme.settingsLineColor

                    Column {
                        id: popupColumn
                        width: parent.width
                        spacing: 12
                        padding: 12

                        property int r: Math.round(currentAccent.r * 255)
                        property int g: Math.round(currentAccent.g * 255)
                        property int b: Math.round(currentAccent.b * 255)

                        Text {
                            text: "Выберите цвет"
                            font.pixelSize: 14
                            color: chatPage.currentTheme.settingsText
                        }

                        Row {
                            spacing: 6
                            Repeater {
                                model: ["R","G","B"]
                                delegate: Column {
                                    spacing: 4
                                    width: 60 // чтобы не вылазили
                                    Text { text: modelData; font.pixelSize: 12; color: chatPage.currentTheme.settingsText }
                                    Slider {
                                        from: 0; to: 255
                                        value: modelData === "R" ? popupColumn.r :
                                               modelData === "G" ? popupColumn.g :
                                                                    popupColumn.b
                                        onValueChanged: {
                                            if(modelData === "R") popupColumn.r = value
                                            else if(modelData === "G") popupColumn.g = value
                                            else popupColumn.b = value

                                            currentAccent = Qt.rgba(popupColumn.r/255, popupColumn.g/255, popupColumn.b/255, 1)
                                            chatPage.accentColor = currentAccent
                                        }
                                        width: 60 // чтобы ползунок вписывался
                                    }
                                }
                            }
                        }

                        // Поле для HEX-кода
                        Column {
                            spacing: 4
                            Text {
                                text: "Или введите HEX-код вручную:"
                                font.pixelSize: 12
                                color: chatPage.currentTheme.settingsText
                            }

                            TextField {
                                id: hexInput
                                placeholderText: "#RRGGBB"
                                text: "#" + ((currentAccent.r*255|0).toString(16).padStart(2,"0")) +
                                             ((currentAccent.g*255|0).toString(16).padStart(2,"0")) +
                                             ((currentAccent.b*255|0).toString(16).padStart(2,"0"))
                                width: parent.width
                                onEditingFinished: {
                                    var colorString = text
                                    if(/^#[0-9A-Fa-f]{6}$/.test(colorString)) {
                                        var r = parseInt(colorString.substr(1,2),16)
                                        var g = parseInt(colorString.substr(3,2),16)
                                        var b = parseInt(colorString.substr(5,2),16)
                                        popupColumn.r = r
                                        popupColumn.g = g
                                        popupColumn.b = b
                                        currentAccent = Qt.rgba(r/255, g/255, b/255, 1)
                                        chatPage.accentColor = currentAccent
                                    } else {
                                        // вернуть предыдущий корректный цвет
                                        text = "#" + ((currentAccent.r*255|0).toString(16).padStart(2,"0")) +
                                                     ((currentAccent.g*255|0).toString(16).padStart(2,"0")) +
                                                     ((currentAccent.b*255|0).toString(16).padStart(2,"0"))
                                    }
                                }
                            }
                        }




                        Button {
                            text: "Закрыть"
                            anchors.horizontalCenter: parent.horizontalCenter
                            onClicked: colorPopup.close()
                        }
                    }
                }
            }
        }

        // Нижняя линия
        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            height: 1
            color: chatPage.currentTheme.settingsLineColor
        }
    }
}
