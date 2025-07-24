import QtQuick 2.15

Item {
    id: root
    property bool enabled: true

    anchors.fill: parent

    Canvas {
        id: canvas
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)

            if (!root.enabled) {
                ctx.fillStyle = "#000"
                ctx.fillRect(0, 0, width, height)
                return
            }

            // Рисуем чёрный фон **первым делом**
            ctx.fillStyle = "#000"
            ctx.fillRect(0, 0, width, height)

            var gradient1 = ctx.createRadialGradient(offsetX * width, offsetY * height, 0, offsetX * width, offsetY * height, 200)
            gradient1.addColorStop(0, "rgba(0,255,255,0.15)")
            gradient1.addColorStop(1, "transparent")

            var gradient2 = ctx.createRadialGradient((1 - offsetX) * width, (1 - offsetY) * height, 0, (1 - offsetX) * width, (1 - offsetY) * height, 200)
            gradient2.addColorStop(0, "rgba(255,0,255,0.15)")
            gradient2.addColorStop(1, "transparent")

            ctx.fillStyle = gradient1
            ctx.fillRect(0, 0, width, height)

            ctx.fillStyle = gradient2
            ctx.fillRect(0, 0, width, height)

        }
    }

    property real offsetX: 0.5
    property real offsetY: 0.5
    property real targetX: 0.5
    property real targetY: 0.5
    property real directionX: 0.001
    property real directionY: 0.001
    property real dampingFactor: 0.9

    Timer {
        interval: 16
        running: true
        repeat: true
        onTriggered: {
            if (!root.enabled)
                return;

            var distX = Math.abs(targetX - offsetX)
            var distY = Math.abs(targetY - offsetY)

            if(distX < 0.01 && distY < 0.01) {
                directionX *= dampingFactor
                directionY *= dampingFactor
            } else {
                targetX += directionX
                targetY += directionY
            }
            if(targetX >= 1 || targetX <= 0) directionX *= -1
            if(targetY >= 1 || targetY <= 0) directionY *= -1

            offsetX += (targetX - offsetX) * 0.1
            offsetY += (targetY - offsetY) * 0.1

            canvas.requestPaint()
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onPositionChanged: function(mouse) {
            if (!root.enabled) return;
            targetX = mouse.x / root.width;
            targetY = mouse.y / root.height;
        }
    }

}
