import QtQuick 2.15
import QtQuick.Controls 2.15
import "../components" as Components
import Themes 1.0

Item {
    id: root
    anchors.fill: parent
    signal requestClose()
    property string selectedCategory: "general"

    // --- Заголовок ---
    Rectangle {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 48
        color: chatPage.currentTheme.settingsColor

        // Заголовок текста
        Text {
            text: "Settings"
            anchors.left: parent.left
            anchors.leftMargin: 12
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 8
            font.pixelSize: 18
            color: chatPage.currentTheme.settingsText
            font.bold: true
        }

        // Кнопка закрытия
        Button {
            id: closeBtn
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: 12
            width: 32
            height: 32
            background: Rectangle {
                anchors.fill: parent
                radius: width/2
                color: closeBtn.hovered ? Qt.rgba(1,1,1,0.08) : "transparent"
            }
            contentItem: Text {
                text: "✕"
                anchors.centerIn: parent
                font.pixelSize: 16
                color: chatPage.currentTheme.settingsText
            }
            hoverEnabled: true
            onClicked: root.requestClose()
        }

    }


    Row {
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        spacing: 0

        // --- Левый столбец категорий ---
        ListView {
            id: categoryList
            width: 180
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            model: [
                { name: "General", id: "general" },
                { name: "Appearance", id: "appearance" },
                { name: "Notifications", id: "notifications" }
            ]
            clip: true
            delegate: Rectangle {
                width: categoryList.width
                height: 44
                radius: 8
                color: root.selectedCategory === modelData.id ? chatPage.currentTheme.middlePanel : "transparent"

                MouseArea {
                    id: categoryMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: root.selectedCategory = modelData.id
                }

                Rectangle { // полоска слева для активного
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: 4
                    radius: 2
                    color: selectedCategory === modelData.id ? chatPage.currentTheme.accentColor : "transparent"
                }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    text: modelData.name
                    font.pixelSize: 15
                    color: chatPage.currentTheme.settingsText
                }

                Rectangle { // hover overlay
                    anchors.fill: parent
                    color: categoryMouse.containsMouse ? Qt.rgba(1,1,1,0.05) : "transparent"
                    z: 1
                }
            }
        }


        // Контент справа
        Loader {
            id: contentLoader
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: parent.width - categoryList.width
            sourceComponent: {
                switch (root.selectedCategory) {
                    case "general": return generalPage
                    case "appearance": return appearancePage
                    case "notifications": return notificationsPage
                    default: return generalPage
                }
            }
        }
    }

    // Pages
    Component {
        id: generalPage
        Item {
            anchors.fill: parent
            Item { anchors.fill: parent; anchors.margins: 20;
                Column { spacing: 12; width: parent.width
                    Components.SettingToggle {
                        title: "Запускать KANEMOOT при запуске системы"
                        subtitle: "Приложение автоматически откроется при входе в систему"
                        enabled: true
                        width: parent.width
                    }
                    Components.SettingToggle {
                        title: "Включить обновления"
                        subtitle: "Загружать и устанавливать новые версии автоматически"
                        enabled: false
                        width: parent.width
                    }
                }
            }
        }
    }

    Component {
        id: appearancePage
        Item {
            anchors.fill: parent
            Item { anchors.fill: parent; anchors.margins: 20;
                Column { spacing: 12; width: parent.width
                    Components.SettingDropdown {
                        title: "Тема приложения"
                        model: [
                            { text: "Светлая" },
                            { text: "Тёмная" }
                        ]

                        Component.onCompleted: {
                            currentIndex = chatPage.currentTheme === LightTheme ? 0 : 1
                        }

                        onCurrentIndexChanged: {
                            if (currentIndex === 0) {
                                chatPage.currentTheme = LightTheme
                            } else {
                                chatPage.currentTheme = DarkTheme
                            }
                        }
                    }


                    Components.SettingToggle {
                        title: "Использовать компактные иконки"
                        subtitle: ""
                        enabled: false
                        width: parent.width
                    }
                }
            }
        }
    }

    Component {
        id: notificationsPage
        Item {
            anchors.fill: parent
            Item { anchors.fill: parent; anchors.margins: 20;
                Column { spacing: 12; width: parent.width
                    Components.SettingToggle { title: "Enable sound"; subtitle: ""; enabled: true; width: parent.width }
                    Components.SettingToggle { title: "Show desktop notifications"; subtitle: ""; enabled: false; width: parent.width }
                }
            }
        }
    }

}
