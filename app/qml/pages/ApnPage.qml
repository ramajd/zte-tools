import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    property var svc
    Timer { id: deferApn; interval: 300; running: true; repeat: false; onTriggered: list.model = svc.apnProfiles() }
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
            model: [] // ponytail: defer blocking fetch; was svc.apnProfiles() which froze startup 8s
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
