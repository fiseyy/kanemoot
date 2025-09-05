import QtQuick 2.15
import QtQuick.Controls 2.15
import "../components" as Components
import Themes 1.0 as Themes

Item {
    anchors.fill: parent
    id: chatPage
    property var currentTheme: Themes.DarkTheme
    property string accentColor: "#00C7D9"
    property string username: ""

    Rectangle {
        anchors.fill: parent
        color: currentTheme.loadingBackground
        z:1
    }

    Components.ChatPageComponent { z:3; id: chatPageContent; visible: false}

    Rectangle {
        id: loadingPage
        z:2
        objectName: "loadingPage"
        anchors.fill: parent
        color: currentTheme.loadingBackground

        // --- Лого ---
        Item {
            id: logoContainer
            width: 150
            height: 150
            anchors.centerIn: parent

            Image {
                id: logoBase
                source: currentTheme.logoIcon
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                layer.enabled: true
                opacity: 1.0
                smooth: true
            }

            SequentialAnimation {
                id: logoAnim
                loops: Animation.Infinite
                ParallelAnimation {
                    NumberAnimation { target: logoBase; property: "opacity"; from: 1.0; to: 0.2; duration: 1000; easing.type: Easing.InOutQuad }
                }
                PauseAnimation { duration: 50 }
                ParallelAnimation {
                    NumberAnimation { target: logoBase; property: "opacity"; from: 0.2; to: 1.0; duration: 1000; easing.type: Easing.InOutQuad }
                }
                PauseAnimation { duration: 300 }
                Component.onCompleted: start()
            }
        }

        // --- FunFact ---
        Item {
            id: funFactContainer
            anchors.top: logoContainer.bottom
            anchors.horizontalCenter: logoContainer.horizontalCenter
            width: parent.width
            height: 60

            Column {
                anchors.fill: parent
                anchors.margins: 5
                spacing: 4

                Text {
                    text: "Знаете ли вы?"
                    color: currentTheme.loadingColor
                    font.pixelSize: 16
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    id: funFact
                    text: funFact.facts[Math.floor(Math.random() * funFact.facts.length)]
                    color: currentTheme.loadingColor
                    font.pixelSize: 14
                    anchors.horizontalCenter: parent.horizontalCenter
                    wrapMode: Text.Wrap
                    property var facts: [
                        "Эмодзи появились в 1982 году.",
                        "Каждую минуту в мире отправляется более 200 миллионов сообщений!",
                        "Wi-Fi был изобретён не для интернета, а для безопасной связи с кассами.",
                        "Если бы интернет был страной, он был бы третьей по численности населения.",
                        "Первый веб-браузер назывался WorldWideWeb и появился в 1990 году.",
                        "Слово 'spam' для нежелательной почты произошло от одноимённого комедийного скетча Monty Python.",
                        "Первая SMS была отправлена в 1992 году и содержала текст 'Merry Christmas'.",
                        "Около 80% интернет-трафика приходится на видеоконтент.",
                        "Электронная почта старше Всемирной паутины — ей уже больше 50 лет.",
                        "На Земле насчитывается более 4,7 миллиарда интернет-пользователей.",
                        "Каждую секунду отправляется более 40 тысяч твитов.",
                        "Электронные смайлы были впервые предложены как наборы символов :) и :( в 1982 году.",
                        "IoT уже насчитывает более 14 миллиардов подключённых устройств.",
                        "Первая камера для видеозвонков была создана ещё в 1964 году для компьютерной лаборатории в Кембридже."
                    ]

                    Timer {
                        interval: 10000
                        running: true
                        repeat: true
                        onTriggered: funFact.text = funFact.facts[Math.floor(Math.random() * funFact.facts.length)]
                    }
                }
            }
        }

        // --- Connection Warning ---
        Item {
            id: connectionWarningContainer
            anchors.top: funFactContainer.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
            visible: false
            anchors.topMargin: 20 // смещение блока ниже

            Column {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 4
                width: parent.width


                Text {
                    id: warningText
                    text: "Проблемы с сетью?"
                    color: currentTheme.loadingColor
                    font.pixelSize: 17 // больше, чем раньше
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    wrapMode: Text.Wrap
                }

                Text {
                    id: warningLink
                    text: "Посмотреть статус серверов можно на сайте"
                    color: chatPage.accentColor
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    wrapMode: Text.Wrap

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: Qt.openUrlExternally("https://status.kanemoot.ru")
                    }
                }
            }

            Timer {
                interval: 10000 // 10 секунд
                running: true
                repeat: false
                onTriggered: connectionWarningContainer.visible = true
            }
        }

        // --- Hide function ---
        function hide() {
            var anim = Qt.createQmlObject('import QtQuick 2.0; NumberAnimation { property: "opacity"; duration: 500 }', loadingPage);
            anim.target = loadingPage;
            anim.from = loadingPage.opacity;
            anim.to = 0;
            anim.onStopped.connect(function() {
                loadingPage.visible = false;
                chatPageContent.opacity = 0;
                chatPageContent.visible = true;
                var fadeIn = Qt.createQmlObject('import QtQuick 2.0; NumberAnimation { property: "opacity"; duration: 500 }', chatPageContent);
                fadeIn.target = chatPageContent;
                fadeIn.from = 0;
                fadeIn.to = 1;
                fadeIn.start();
            });
            anim.start();
        }
        function show() {
            loadingPage.visible = true;
            loadingPage.opacity = 0;

            var fadeIn = Qt.createQmlObject('import QtQuick 2.0; NumberAnimation { property: "opacity"; duration: 500 }', loadingPage);
            fadeIn.target = loadingPage;
            fadeIn.from = 0;
            fadeIn.to = 1;
            fadeIn.start();

            if (chatPageContent) {
                var fadeOut = Qt.createQmlObject('import QtQuick 2.0; NumberAnimation { property: "opacity"; duration: 500 }', chatPageContent);
                fadeOut.target = chatPageContent;
                fadeOut.from = chatPageContent.opacity;
                fadeOut.to = 0;
                fadeOut.onStopped.connect(function() { chatPageContent.visible = false; });
                fadeOut.start();
            }
        }

    }

}
