pragma Singleton
import QtQuick 2.15

QtObject {
    // Panels
    property color leftPanel: "#272729"
    property color middlePanel: "#242424"
    property color rightPanel: "#1e1e1e"

    // Search
    property color searchRectangle: "#383838"
    property color searchText: "#999999"

    // Hotkey
    property color hotkeyRectangle: "#333333"
    property color hotkeyText: "#888888"

    // Discover / DM
    property color discoverRectangle: "#383838"
    property color dmText: "#A7A7A7"

    // My info
    property color myInfoRectangle: "#292929"
    property color myNickname: "#ffffff"

    // Chat
    property color chatInfoRectangle: "#262626"
    property color sendRectangle: "#292929"
    property color sendText: "#C0C0C0"

    // Icons
    property string discoverIcon: "qrc:///assets/dark/discover.svg"
    property string emojiIcon: "qrc:///assets/dark/emoji.svg"
    property string fileIcon: "qrc:///assets/dark/file.svg"
    property string headsetIcon: "qrc:///assets/dark/headset.svg"
    property string muteIcon: "qrc:///assets/dark/mute.svg"
    property string searchIcon: "qrc:///assets/dark/search.svg"
    property string sendIcon: "qrc:///assets/dark/send.svg"
    property string settingsIcon: "qrc:///assets/dark/settings.svg"

    property color loadingBackground: "#1e1e1e"
    property color loadingColor: "#F0F0F0"
    property string logoIcon: "qrc:///assets/logo_white.png"
    property string smallLogoIcon: "qrc:///assets/logo_white56.png"
}
