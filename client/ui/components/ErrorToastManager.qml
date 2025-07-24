import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: toastManager
    anchors.fill: parent
    property int maxToasts: 1
    property var toastQueue: []

    function enqueue(title, message, durationMs = 3000) {
        toastQueue.push({ title: title, message: message, duration: durationMs });
        processQueue();
    }

    function processQueue() {
        if (toastQueue.length === 0 || toastManager.children.some(child => child.visible)) {
            // если очередь пустая или есть видимый тост — ждем
            return;
        }

        const data = toastQueue.shift();
        let toast = toastComponent.createObject(toastManager);

        toast.show(data.title, data.message, data.duration);

        toast.dismissed.connect(() => {
            toast.destroy();
            processQueue();
        });
    }

    Component {
        id: toastComponent
        ErrorToast {}
    }
}
