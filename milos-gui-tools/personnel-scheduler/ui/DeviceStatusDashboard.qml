import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: dashboard
    color: "#0a0a0a"
    border.color: "#00ff88"
    border.width: 2
    radius: 5

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        Text {
            text: "Device Status Dashboard"
            font.pixelSize: 18
            font.bold: true
            color: "#00ff88"
        }

        GridLayout {
            columns: 2
            Layout.fillWidth: true

            Text {
                text: "Connected Devices:"
                color: "#ffffff"
            }

            Text {
                text: "0"
                color: "#00ff88"
            }

            Text {
                text: "Face Recognition:"
                color: "#ffffff"
            }

            Text {
                text: "0"
                color: "#00ff88"
            }

            Text {
                text: "Fingerprint Scanners:"
                color: "#ffffff"
            }

            Text {
                text: "0"
                color: "#00ff88"
            }
        }
    }
}

