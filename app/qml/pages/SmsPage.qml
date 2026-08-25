import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    property var svc
    Timer { id: deferSms; interval: 400; running: true; repeat: false; onTriggered: list.model = svc.smsMessages(1,10) }
    ColumnLayout {
        anchors.fill: parent; anchors.margins: 16; spacing: 8
        RowLayout {
            Label { text: "SMS"; font.bold: true; font.pixelSize: 16; Layout.fillWidth: true }
            Button { text: "Inbox"; onClicked: list.model = svc.smsMessages(1,10) }
            Button { text: "All"; onClicked: list.model = svc.smsMessages(1,10) }
        }
        ListView {
            id: list
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredHeight: 300
            clip: true
            model: [] // ponytail: defer blocking fetch; was svc.smsMessages() which froze startup 8s
            delegate: Frame {
                width: ListView.view.width
                ColumnLayout {
                    anchors.fill: parent
                    Label { text: modelData.number + "  •  " + modelData.date; font.pixelSize: 11; opacity: 0.6 }
                    Label { text: modelData.text; wrapMode: Text.Wrap; Layout.fillWidth: true }
                    RowLayout {
                        Button { text: "Delete"; flat: true; onClicked: { svc.deleteSms(modelData.id); list.model = svc.smsMessages(1,10) } }
                    }
                }
            }
        }
        GroupBox {
            title: "Send SMS"
            Layout.fillWidth: true
            RowLayout {
                anchors.fill: parent
                TextField { id: numField; placeholderText: "+62..."; implicitWidth: 160 }
                TextField { id: msgField; placeholderText: "message"; Layout.fillWidth: true }
                Button { text: "Send"; onClicked: { svc.sendSms(numField.text, msgField.text); msgField.clear() } }
            }
        }
    }
}
