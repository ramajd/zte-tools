import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Frame {
    property string title
    property string value
    property string subtitle
    Layout.fillWidth: true
    padding: 12
    ColumnLayout {
        anchors.fill: parent
        Label { text: title; font.pixelSize: 11; opacity: 0.6 }
        Label { text: value; font.pixelSize: 20; font.bold: true }
        Label { text: subtitle; font.pixelSize: 11; opacity: 0.5; visible: subtitle.length>0 }
    }
}
