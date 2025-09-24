import QtQuick 2.15
import QtQuick.Controls 2.15
import "../components"

Item {
    id: loginPage
    anchors.fill: parent
    signal loginAttempted(string username, string password)
    signal regRedirectRequested()
    property bool regRedirectPending: false
    property bool skipAnimations: false

    function checkRedirect() {
        if (!leftColumnHideAnim.running &&
            !yAnimLeftHide.running &&
            !rightColumnHideAnim.running &&
            !yAnimRightHide.running &&
            regRedirectPending) {
            regRedirectRequested();
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

            Column {
                id: leftColumn
                width: parent.width * 0.5
                spacing: 18

                opacity: 0
                y: 20

                Component.onCompleted: {
                    leftColumnAnim.start();
                }

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
                        loginAttempted(usernameField.text, passwordField.text)
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
                        regRedirectPending = true
                        leftColumnHideAnim.start()
                        rightColumnHideAnim.start()
                    }
                }
            }

            // Правая колонка — QR-код
            Rectangle {
                id: rightColumn
                width: parent.width * 0.4
                height: parent.height - 60
                radius: 12
                color: "#242424"
                border.color: "#3a3a3a"
                border.width: 1
                anchors.verticalCenter: parent.verticalCenter

                opacity: 0
                y: 20

                Component.onCompleted: {
                            rightColumnAnim.start();
                }

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

            NumberAnimation {
                    id: leftColumnAnim
                    target: leftColumn
                    property: "opacity"
                    from: 0
                    to: 1
                    duration: skipAnimations ? 0 : 300
                    easing.type: Easing.InOutQuad
                    onRunningChanged: {
                        if (running) {
                            // параллельно анимируем Y
                            yAnimLeft.start();
                        }
                    }
                }
            NumberAnimation {
                    id: yAnimLeft
                    target: leftColumn
                    property: "y"
                    from: 20
                    to: 0
                    duration: skipAnimations ? 0 : 300
                    easing.type: Easing.InOutQuad
            }

            NumberAnimation {
                    id: rightColumnAnim
                    target: rightColumn
                    property: "opacity"
                    from: 0
                    to: 1
                    duration: skipAnimations ? 0 : 300
                    easing.type: Easing.InOutQuad
                    onRunningChanged: {
                        if (running) {
                            yAnimRight.start();
                        }
                    }
            }
            NumberAnimation {
                    id: yAnimRight
                    target: rightColumn
                    property: "y"
                    from: 20
                    to: 0
                    duration: skipAnimations ? 0 : 300
                    easing.type: Easing.InOutQuad
            }

            NumberAnimation {
                id: leftColumnHideAnim
                target: leftColumn
                property: "opacity"
                from: 1
                to: 0
                duration: 300
                easing.type: Easing.InOutQuad
                onRunningChanged: {
                    if (running)
                        yAnimLeftHide.start();
                }
                onStopped: checkRedirect()
            }

            NumberAnimation {
                id: yAnimLeftHide
                target: leftColumn
                property: "y"
                from: 0
                to: 20
                duration: 300
                easing.type: Easing.InOutQuad
                onStopped: checkRedirect()
            }

            NumberAnimation {
                id: rightColumnHideAnim
                target: rightColumn
                property: "opacity"
                from: 1
                to: 0
                duration: 300
                easing.type: Easing.InOutQuad
                onRunningChanged: {
                    if (running)
                        yAnimRightHide.start();
                }
                onStopped: checkRedirect()
            }

            NumberAnimation {
                id: yAnimRightHide
                target: rightColumn
                property: "y"
                from: 0
                to: 20
                duration: 300
                easing.type: Easing.InOutQuad
                onStopped: checkRedirect()
            }
        }
    }
}
