import QtQuick 2.13
import QtQuick.Window 2.13

Window {
    id: background
    width: Screen.width
    height: Screen.height
    flags: Qt.FramelessWindowHint
    visible: true

    Image {
        anchors.fill: parent
        source: './images/bg_scooterson_vertical.png'
    }
}
