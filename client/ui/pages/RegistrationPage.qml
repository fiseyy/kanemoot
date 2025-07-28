import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components"

Item {
    id: regPage
    anchors.fill: parent
    signal regAttemped(string username, string password, string email)
    signal loginRedirectRequested()
    property bool loginRedirectPending: false

    function checkRedirect() {
        if (!leftColumnHideAnim.running &&
            !yAnimLeftHide.running &&
            !rightColumnHideAnim.running &&
            !yAnimRightHide.running &&
            loginRedirectPending) {
            loginRedirectRequested();
        }
    }

    Image {
        anchors.fill: parent
        source: "qrc:///assets/background.png"
        fillMode: Image.PreserveAspectCrop
    }

    Rectangle {
        anchors.centerIn: parent
        width: 720
        height: 380
        radius: 18
        color: "#1b1b1b"
        opacity: 0.9
        border.color: "#3a3a3a"
        border.width: 1

        Row {
            anchors.fill: parent
            anchors.margins: 30
            spacing: 40

            // Левая колонка
            Column {
                id: leftColumn
                width: parent.width * 0.44
                spacing: 18

                opacity: 0
                y: 20

                Component.onCompleted: {
                    leftColumnAnim.start()
                }

                Text {
                    text: "Регистрация"
                    font.pixelSize: 28
                    font.bold: true
                    color: "#e0e0e0"
                }

                Text {
                    text: "Введите данные для создания аккаунта"
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
                    id: emailField
                    placeholderText: "Email"
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
                    text: "Зарегистрироваться"
                    font.pixelSize: 16
                    width: parent.width
                    background: Rectangle {
                        radius: 6
                        color: parent.pressed ? "#016d73" : "#01818D"
                    }
                    onClicked: {
                        regAttemped(usernameField.text, passwordField.text, emailField.text)
                    }
                }
            }

            Rectangle {
                id: separator
                width: 1
                height: parent.height
                color: "#3a3a3a"
                anchors.verticalCenter: parent.verticalCenter
            }

            // Правая колонка
            Column {
                id: rightColumn
                width: parent.width * 0.44
                spacing: 20
                anchors.verticalCenter: parent.verticalCenter

                opacity: 0
                y: 20

                Component.onCompleted: {
                    rightColumnAnim.start()
                }

                Text {
                    text: "Уже есть аккаунт?"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#e0e0e0"
                    horizontalAlignment: Text.AlignHCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    text: "Войдите в существующий аккаунт, чтобы продолжить пользоваться приложением."
                    wrapMode: Text.WordWrap
                    font.pixelSize: 14
                    color: "#aaaaaa"
                    horizontalAlignment: Text.AlignHCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width * 0.85
                }

                Button {
                    text: "Войти"
                    font.pixelSize: 16
                    width: parent.width * 0.5
                    anchors.horizontalCenter: parent.horizontalCenter
                    background: Rectangle {
                        radius: 6
                        color: parent.pressed ? "#016d73" : "#01818D"
                    }
                    onClicked: {
                        loginRedirectPending = true
                        leftColumnHideAnim.start()
                        rightColumnHideAnim.start()
                        separatorHideAnim.start()
                    }

                }
            }

            // Анимации

            NumberAnimation {
                id: leftColumnAnim
                target: leftColumn
                property: "opacity"
                from: 0
                to: 1
                duration: 400
                easing.type: Easing.InOutQuad
                onRunningChanged: if (running) yAnimLeft.start()
            }

            NumberAnimation {
                id: yAnimLeft
                target: leftColumn
                property: "y"
                from: 20
                to: 0
                duration: 400
                easing.type: Easing.InOutQuad
            }

            NumberAnimation {
                id: rightColumnAnim
                target: rightColumn
                property: "opacity"
                from: 0
                to: 1
                duration: 400
                easing.type: Easing.InOutQuad
                onRunningChanged: if (running) yAnimRight.start()
            }

            NumberAnimation {
                id: yAnimRight
                target: rightColumn
                property: "y"
                from: 20
                to: 0
                duration: 400
                easing.type: Easing.InOutQuad
            }
            NumberAnimation {
                id: leftColumnHideAnim
                target: leftColumn
                property: "opacity"
                from: 1
                to: 0
                duration: 400
                easing.type: Easing.InOutQuad
                onStopped: checkRedirect()
                onRunningChanged: if (running) yAnimLeftHide.start()
            }

            NumberAnimation {
                id: yAnimLeftHide
                target: leftColumn
                property: "y"
                from: 0
                to: 20
                duration: 400
                easing.type: Easing.InOutQuad
                onStopped: checkRedirect()
            }

            NumberAnimation {
                id: rightColumnHideAnim
                target: rightColumn
                property: "opacity"
                from: 1
                to: 0
                duration: 400
                easing.type: Easing.InOutQuad
                onStopped: checkRedirect()
                onRunningChanged: if (running) yAnimRightHide.start()
            }

            NumberAnimation {
                id: yAnimRightHide
                target: rightColumn
                property: "y"
                from: 0
                to: 20
                duration: 400
                easing.type: Easing.InOutQuad
                onStopped: checkRedirect()
            }

            NumberAnimation {
                id: separatorHideAnim
                target: separator
                property: "opacity"
                from: 1
                to: 0
                duration: 400
                easing.type: Easing.InOutQuad
            }
        }
    }
}
