import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ZteTools

ApplicationWindow {
    id: win
    visible: true
    width: 900; height: 640
    title: "ZTE MF910 Tools"

    ZteService { id: svc }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            Label { text: "ZTE MF910"; font.bold: true; leftPadding: 8 }
            Item { Layout.fillWidth: true }
            TextField { id: hostField; text: svc.baseUrl; placeholderText: "http://192.168.0.1"; implicitWidth: 180; onEditingFinished: svc.baseUrl = text }
            TextField { id: passField; echoMode: TextInput.Password; placeholderText: "password"; implicitWidth: 140 }
            Button { text: "Login"; onClicked: svc.login(passField.text) }
            Button { text: svc.pppStatus === "ppp_connected" ? "Disconnect" : "Connect"; onClicked: svc.pppStatus === "ppp_connected" ? svc.disconnectWan() : svc.connectWan() }
            BusyIndicator { running: svc.busy; implicitWidth: 22; implicitHeight: 22 }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        TabBar {
            id: bar
            Layout.fillWidth: true
            TabButton { text: "Dashboard" }
            TabButton { text: "WAN" }
            TabButton { text: "APN" }
            TabButton { text: "SMS" }
        }
        StackLayout {
            Layout.fillWidth: true; Layout.fillHeight: true
            currentIndex: bar.currentIndex
            Dashboard { id: dash; svc: svc }
            WanPage { svc: svc }
            ApnPage { svc: svc }
            SmsPage { svc: svc }
        }
        Label { text: svc.lastError; color: "crimson"; visible: svc.lastError.length>0; leftPadding: 8; bottomPadding: 4 }
    }

    Component.onCompleted: svc.setPolling(true, 3000)
}
