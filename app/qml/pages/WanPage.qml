import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    property var svc
    ColumnLayout {
        anchors.fill: parent; anchors.margins: 16; spacing: 12
        Label { text: "WAN Control"; font.bold: true; font.pixelSize: 16 }
        RowLayout {
            ComboBox { id: bearer; model: ["NETWORK_auto","Only_LTE","Only_WCDMA","Only_GSM"]; implicitWidth: 200 }
            Button { text: "Set Bearer"; onClicked: svc.setBearer(bearer.currentText) }
            Label { text: "(device must be disconnected)"; opacity: 0.6 }
        }
        RowLayout {
            ComboBox { id: mode; model: ["auto_dial","manual_dial"]; implicitWidth: 160 }
            Button { text: "Set Mode"; onClicked: svc.setPolling(false); /* svc.setConnectionMode via client */ }
        }
        Item { Layout.fillHeight: true }
    }
}
