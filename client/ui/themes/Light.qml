pragma Singleton
import QtQuick 2.15

QtObject {
    // Panels
    property color leftPanel: "#FFFFFF"
    property color middlePanel: "#F7F7F7"
    property color rightPanel: "#DFDFDF"

    // Search
    property color searchRectangle: "#ccc"
    property color searchText: "#444444"
    property color searchDefaultText: "#555555"

    // Hotkey
    property color hotkeyRectangle: "#BABABA"
    property color hotkeyText: "#333333"

    // Discover / DM
    property color discoverRectangle: "#ccc"
    property color dmText: "#555555"

    // My info
    property color myInfoRectangle: "#FFFFFF"
    property color myInfoRectangleBorder: "#CCCCCC"
    property color myNickname: "#000000"
    property color myInfoEditHoverColor: Qt.rgba(0,0,0,0.1)

    // Chat
    property color chatInfoRectangle: "#EEEEEE"
    property color sendRectangle: "#D9D9D9"
    property color sendText: "#444444"
    property color sendDefaultText: "#555555"
    property color sendRectangleBorder: "#CCCCCC"

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

    property color settingsColor: "#F7F7F7"

    property color sliderOffColor:"#D0D0D0"
    property color settingsText: "#000000"

    property color settingsLineColor: Qt.rgba(0,0,0,0.08)

    property color addServerBtnBg: "#FFFFFF"
    property color addServerBtnBorder: "#CCCCCC"    // чуть темнее для границы

    property string addServerIcon: "qrc:///assets/light/add-server.png"

    // Popup / кнопки внутри popup
    property color popupButtonHover: Qt.rgba(0,0,0,0.05)       // лёгкое подсвечивание для светлой темы
    property color popupButtonText: "#000000"                   // текст кнопок
    property color popupLogoutButton: "#D9534F"                // чуть мягче красный для светлой темы

    property string textChannelImg: "qrc:///assets/light/text-channel.png"
    property string voiceChannelImg: "qrc:///assets/light/voice-channel.png"

    property color channelHoverBg: Qt.rgba(0,0,0,0.03)

    property color defaultPrimaryTextColor: "black"
    property color defaultSecondaryTextColor: "#666"
    property color channelTextColor: "#333"
}
