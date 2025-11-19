import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: window
    width: 1200
    height: 800
    visible: true
    title: "MilOS Personnel Scheduler - Device Management"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20

        Text {
            text: "Biometric Device Management"
            font.pixelSize: 24
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        RowLayout {
            Layout.fillWidth: true

            Button {
                text: "Detect Devices"
                onClicked: {
                    // Trigger device detection
                    deviceManager.checkForDevices()
                }
            }

            Button {
                text: "Refresh Status"
                onClicked: {
                    // Refresh device status
                }
            }
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: deviceList
                model: deviceManager ? deviceManager.getRegisteredDevices() : []
                delegate: Rectangle {
                    width: deviceList.width
                    height: 100
                    color: index % 2 === 0 ? "#1a1a1a" : "#0a0a0a"
                    border.color: "#00ff88"
                    border.width: 1

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 10

                        Text {
                            text: "Device: " + modelData
                            font.pixelSize: 16
                            color: "#00ff88"
                        }

                        Text {
                            text: "Status: Connected"
                            font.pixelSize: 12
                            color: "#ffffff"
                        }
                    }
                }
            }
        }
    }
}

