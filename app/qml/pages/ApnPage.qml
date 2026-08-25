import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    property var svc
    ColumnLayout {
        anchors.fill: parent; anchors.margins: 16; spacing: 8
        RowLayout {
            Label { text: "APN Profiles"; font.bold: true; font.pixelSize: 16; Layout.fillWidth: true }
            Button { text: "Reload"; onClicked: list.model = svc.apnProfiles() }
        }
        ListView {
            id: list
            Layout.fillWidth: true; Layout.fillHeight: true
            clip: true
            model: svc.apnProfiles()
            delegate: Frame {
                width: ListView.view.width
                RowLayout {
                    anchors.fill: parent
                    Label { text: "#" + modelData.index + " " + (modelData.name || "(empty)"); Layout.fillWidth: true }
                    Label { text: modelData.apn; opacity: 0.7 }
                    Label { text: modelData.pdpType; opacity: 0.6 }
                }
            }
        }
    }
}
