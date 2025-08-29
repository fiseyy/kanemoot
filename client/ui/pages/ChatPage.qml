import QtQuick 2.15
import QtQuick.Controls 2.15
import "../components" as Components
import Themes 1.0

Item {
    anchors.fill: parent
    id: root
    property var currentTheme: DarkTheme

    Rectangle {
        anchors.fill: parent
        color: currentTheme.loadingBackground
    }

    Components.ChatPageComponent { id: chatPageContent; visible: false; currentTheme: root.currentTheme }

    Rectangle {
        id: loadingPage
        objectName: "loadingPage"
        anchors.fill: parent

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
                opacity: 1.0
            }

            SequentialAnimation {
                id: logoAnim
                loops: Animation.Infinite

                NumberAnimation { target: logoBase; property: "opacity"; from: 1.0; to: 0.2; duration: 1000; easing.type: Easing.InOutQuad }
                PauseAnimation { duration: 50 }
                NumberAnimation { target: logoBase; property: "opacity"; from: 0.2; to: 1.0; duration: 1000; easing.type: Easing.InOutQuad }
                PauseAnimation { duration: 300 }

                Component.onCompleted: start()
            }
        }

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

                // Заголовок
                Text {
                    text: "Знаете ли вы?"
                    color: currentTheme.loadingColor
                    font.pixelSize: 16
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                // Основной факт
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

        function hide() {
            // Плавное скрытие loadingPage
            var anim = Qt.createQmlObject('import QtQuick 2.0; NumberAnimation { property: "opacity"; duration: 500 }', loadingPage);
            anim.target = loadingPage;
            anim.from = loadingPage.opacity;
            anim.to = 0;
            anim.onStopped.connect(function() {
                loadingPage.visible = false;

                // Появление chatPageContent
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


    }
}
