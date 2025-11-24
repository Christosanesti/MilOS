import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: blockedTransmissionsViewer
    color: "transparent"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: 16

            Text {
                text: "Blocked Transmissions"
                color: "#ffffff"
                font.pixelSize: 20
                font.bold: true
                Layout.fillWidth: true
            }

            Button {
                text: "Refresh"
                variant: "secondary"
                Layout.preferredWidth: 120
                enabled: dbusInterface && dbusInterface.serviceAvailable

                onClicked: {
                    if (dataGuardManager) {
                        dataGuardManager.refreshBlockedTransmissions()
                    }
                }
            }
        }

        // Blocked transmissions list
        Card {
            Layout.fillWidth: true
            Layout.fillHeight: true
            variant: "dashboard-card"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                Text {
                    text: "Blocked Transmission History"
                    color: "#ffffff"
                    font.pixelSize: 16
                    font.bold: true
                    Layout.fillWidth: true
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    ListView {
                        id: blockedList
                        model: dataGuardManager ? dataGuardManager.blockedTransmissions : []
                        spacing: 8

                        delegate: Card {
                            width: blockedList.width
                            height: 100
                            variant: "status-card"

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 12
                                spacing: 8

                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 12

                                    StatusIndicator {
                                        Layout.preferredWidth: 40
                                        Layout.preferredHeight: 40
                                        variant: "status-light"
                                        state: "critical"
                                    }

                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 4

                                        Text {
                                            text: "Source: " + (modelData.source || "Unknown") + " → Destination: " + (modelData.destination || "Unknown")
                                            color: "#ffffff"
                                            font.pixelSize: 14
                                            font.bold: true
                                            Layout.fillWidth: true
                                        }

                                        Text {
                                            text: "Protocol: " + (modelData.protocol || "Unknown") + " | Port: " + (modelData.port || "Unknown")
                                            color: "#cccccc"
                                            font.pixelSize: 12
                                            Layout.fillWidth: true
                                        }

                                        Text {
                                            text: "Timestamp: " + (modelData.timestamp || "Unknown")
                                            color: "#888888"
                                            font.pixelSize: 11
                                            Layout.fillWidth: true
                                        }
                                    }
                                }

                                Text {
                                    text: "Reason: " + (modelData.reason || "Unencrypted transmission blocked")
                                    color: "#ff0000"
                                    font.pixelSize: 11
                                    Layout.fillWidth: true
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        if (dataGuardManager) {
            dataGuardManager.refreshBlockedTransmissions()
        }
    }
}

