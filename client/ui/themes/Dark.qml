pragma Singleton
import QtQuick 2.15

QtObject {
    // Panels
    property color leftPanel: "#0C0C0C"      // было #272729
    property color middlePanel: "#0F0F0F"    // было #242424
    property color rightPanel: "#0A0A0A"     // было #1A1A1A

    // Search
    property color searchRectangle: "#2C2C2C"    // темнее #383838
    property color searchText: "#AAAAAA"
    property color searchDefaultText: "#999999"

    // Hotkey
    property color hotkeyRectangle: "#292929"     // темнее #333333
    property color hotkeyText: "#888888"

    // Discover / DM
    property color discoverRectangle: "#2C2C2C"  // темнее #383838
    property color dmText: "#A7A7A7"

    // My info
    property color myInfoRectangle: "#101010" // было #292929
    property color myInfoRectangleBorder: "#1f1f23"
    property color myNickname: "#ffffff"
    property color myInfoEditHoverColor: Qt.rgba(1,1,1,0.1)

    // Chat
    property color chatInfoRectangle: "#101010"  // темнее #212121
    property color sendRectangle: "#151515"
    property color sendText: "#CFCFCF"
    property color sendDefaultText: "#C0C0C0"
    property color sendRectangleBorder: "#1f1f23"

    // Icons
    property string discoverIcon: "qrc:///assets/dark/discover.svg"
    property string emojiIcon: "qrc:///assets/dark/emoji.svg"
    property string fileIcon: "qrc:///assets/dark/file.svg"
    property string headsetIcon: "qrc:///assets/dark/headset.svg"
    property string mutedHeadsetIcon: "qrc:///assets/dark/headset-off.svg"
    property string micIcon: "qrc:///assets/dark/mic.svg"
    property string mutedMicIcon: "qrc:///assets/dark/mic-off.svg"
    property string searchIcon: "qrc:///assets/dark/search.svg"
    property string sendIcon: "qrc:///assets/dark/send.svg"
    property string settingsIcon: "qrc:///assets/dark/settings.svg"

    // Loading
    property color loadingBackground: "#1E1E1E"
    property color loadingColor: "#F0F0F0"
    property string logoIcon: "qrc:///assets/logo_white.png"
    property string smallLogoIcon: "qrc:///assets/logo_white56.png"
    // Settings
    property color settingsColor: "#101010"
    property color sliderOffColor:"#2A2A2A"
    property color settingsText: "#ffffff"

    property color settingsLineColor: Qt.rgba(1,1,1,0.08)

    property color addServerBtnBg: "#101010"
    property color addServerBtnBorder: "#1f1f23"

    property string addServerIcon: "qrc:///assets/dark/add-server.png"

    // Popup / кнопки внутри popup
    property color popupButtonHover: Qt.rgba(1,1,1,0.05)       // лёгкое подсвечивание
    property color popupButtonText: "#FFFFFF"                   // текст кнопок
    property color popupLogoutButton: "#FF4C4C"                // красная кнопка выхода

    property string textChannelImg: "qrc:///assets/dark/text-channel.png"
    property string voiceChannelImg: "qrc:///assets/dark/voice-channel.png"

    property color channelHoverBg: "#1A1A1A"

    property color defaultPrimaryTextColor: "#FFFFFF"
    property color defaultSecondaryTextColor: "gray"
    property color channelTextColor: "lightgray"

    property string chevronDown: "qrc:///assets/dark/arrow-down.png"
    property string closeIcon: "qrc:///assets/dark/close.png"

    property string defaultAvatar: "qrc:///assets/dark/default-user.svg"

    property color usernameColor: "#C2C2C2"
    property color timestampColor: "#999999"
    property color messageBackground: "#292929"
    property color messageTextColor: "#FFFFFF"
}
