pragma Singleton
import QtQuick 2.15

QtObject {
    // Panels
    property color leftPanel: "#FFFFFF"
    property color middlePanel: "#F7F7F7"
    property color rightPanel: "#D0D0D0"

    // Search
    property color searchRectangle: "#ccc"
    property color searchText: "#999999"

    // Hotkey
    property color hotkeyRectangle: "#BABABA"
    property color hotkeyText: "#333333"

    // Discover / DM
    property color discoverRectangle: "#ccc"
    property color dmText: "#555555"

    // My info
    property color myInfoRectangle: "#FFFFFF"
    property color myNickname: "#000000"

    // Chat
    property color chatInfoRectangle: "#E0E0E0"
    property color sendRectangle: "#D9D9D9"
    property color sendText: "#555555"

    // Icons
    property string discoverIcon: "qrc:///assets/light/discover.svg"
    property string emojiIcon: "qrc:///assets/light/emoji.svg"
    property string fileIcon: "qrc:///assets/light/file.svg"
    property string headsetIcon: "qrc:///assets/light/headset.svg"
    property string mutedHeadsetIcon: "qrc:///assets/light/headset-off.svg"
    property string micIcon: "qrc:///assets/light/mic.svg"
    property string mutedMicIcon: "qrc:///assets/light/mic-off.svg"
    property string searchIcon: "qrc:///assets/light/search.svg"
    property string sendIcon: "qrc:///assets/light/send.svg"
    property string settingsIcon: "qrc:///assets/light/settings.svg"

    property color loadingBackground: "#F0F0F0"
    property color loadingColor: "#000000"
    property string logoIcon: "qrc:///assets/logo_black.png"
    property string smallLogoIcon: "qrc:///assets/logo_black56.png"

    property color accentColor: "#00B3C2"
    property color settingsColor: "#F7F7F7"

    property color sliderOffColor:"#D0D0D0"
    property color sliderOnColor: accentColor
    property color settingsText: "#000000"

    property color settingsLineColor: Qt.rgba(0,0,0,0.08)
}
