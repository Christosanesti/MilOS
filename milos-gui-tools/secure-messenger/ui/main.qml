import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: window
    width: 1200
    height: 800
    visible: true
    title: "MilOS Secure Messenger"

    Rectangle {
        anchors.fill: parent
        color: "#0a0a0a"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10

            Text {
                text: "MilOS Secure Messenger"
                font.pixelSize: 24
                font.bold: true
                color: "#00ff88"
            }

            Loader {
                source: "key_management.qml"
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }
}

