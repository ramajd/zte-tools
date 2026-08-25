import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "../components"

Item {
    property var svc
    ColumnLayout {
        anchors.fill: parent; anchors.margins: 16; spacing: 12
        RowLayout {
            Layout.fillWidth: true; spacing: 12
            StatCard { title: "Signal"; value: (svc.signalBar || "-") + " bars"; subtitle: svc.rssi + " dBm" }
            StatCard { title: "Network"; value: svc.networkType || "-"; subtitle: svc.provider }
            StatCard { title: "WAN"; value: svc.pppStatus || "-"; subtitle: svc.baseUrl }
        }
        Button { text: "Refresh"; onClicked: svc.refresh() }
        Label { text: "Polling every 3s. Bearer & APN changes require disconnect."; opacity: 0.6; font.pixelSize: 12 }
        Item { Layout.fillHeight: true }
    }
}
