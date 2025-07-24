import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: loginPage
    anchors.fill: parent
    Rectangle {
        anchors.fill: parent
        color: "#0e0e0e"
    }

    signal loginAttempted(string username, string password)

    Rectangle {
        anchors.centerIn: parent
        width: 720
        height: 380
        radius: 18
        color: "#1b1b1b"
        border.color: "#3a3a3a"
        border.width: 1
        // layer.enabled: true
        // layer.smooth: true

        Row {
            anchors.fill: parent
            anchors.margins: 30
            spacing: 40

            // Левая колонка: вход
            Column {
                width: parent.width * 0.5
                spacing: 18

                Text {
                    text: "Вход в аккаунт"
                    font.pixelSize: 28
                    font.bold: true
                    color: "#e0e0e0"
                }

                Text {
                    text: "Введите логин и пароль"
                    font.pixelSize: 14
                    color: "#9e9e9e"
                }

                TextField {
                    id: usernameField
                    placeholderText: "Имя пользователя"
                    font.pixelSize: 16
                    color: "#e0e0e0"
                    width: parent.width
                    background: Rectangle {
                        color: "#2c2c2c"
                        radius: 6
                    }
                    leftPadding: 10
                    rightPadding: 10
                    topPadding: 8
                    bottomPadding: 8
                }

                TextField {
                    id: passwordField
                    placeholderText: "Пароль"
                    echoMode: TextInput.Password
                    font.pixelSize: 16
                    color: "#e0e0e0"
                    width: parent.width
                    background: Rectangle {
                        color: "#2c2c2c"
                        radius: 6
                    }
                    leftPadding: 10
                    rightPadding: 10
                    topPadding: 8
                    bottomPadding: 8
                }

                Button {
                    text: "Войти"
                    font.pixelSize: 16
                    width: parent.width
                    background: Rectangle {
                        radius: 6
                        color: parent.pressed ? "#016d73" : "#01818D"
                    }
                    onClicked: {
                        loginPage.loginAttempted(usernameField.text, passwordField.text)
                    }
                }


                MouseArea {
                    id: registerMouseArea
                    width: parent.width
                    height: 30
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    Text {
                        text: "Зарегистрироваться"
                        font.pixelSize: 14
                        color: registerMouseArea.hovered ? "#01818D" : "#b0b0b0"
                        anchors.horizontalCenter: parent.horizontalCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onClicked: {
                        // Сигнал на регистрацию, если понадобится
                    }
                }
            }

            // Правая колонка — QR-код
            Rectangle {
                width: parent.width * 0.4
                height: parent.height - 60
                radius: 12
                color: "#242424"
                border.color: "#3a3a3a"
                border.width: 1
                anchors.verticalCenter: parent.verticalCenter

                Text {
                    anchors.centerIn: parent
                    text: "Здесь должен быть QR-код"
                    color: "#666"
                    font.pixelSize: 18
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
                    width: parent.width * 0.8
                }
            }
        }
    }
}
