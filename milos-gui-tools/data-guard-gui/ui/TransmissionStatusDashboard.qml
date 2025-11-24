import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: transmissionStatusDashboard
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
                text: "Transmission Status"
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
                        dataGuardManager.refreshTransmissionStatus()
                    }
                }
            }
        }

        // Status cards
        GridLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            columns: 4
            columnSpacing: 16
            rowSpacing: 16

            Card {
                Layout.fillWidth: true
                Layout.fillHeight: true
                variant: "dashboard-card"

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 8

                    Text {
                        text: "Monitoring Status"
                        color: "#cccccc"
                        font.pixelSize: 14
                        Layout.fillWidth: true
                    }

                    StatusIndicator {
                        Layout.preferredWidth: 100
                        Layout.preferredHeight: 40
                        variant: "status-light"
                        state: dataGuardManager && dataGuardManager.transmissionStatus.monitoring_active ? "active" : "inactive"
                        text: dataGuardManager && dataGuardManager.transmissionStatus.monitoring_active ? "Active" : "Inactive"
                    }
                }
            }

            Card {
                Layout.fillWidth: true
                Layout.fillHeight: true
                variant: "dashboard-card"

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 8

                    Text {
                        text: "Blocked Transmissions"
                        color: "#cccccc"
                        font.pixelSize: 14
                        Layout.fillWidth: true
                    }

                    Text {
                        text: dataGuardManager && dataGuardManager.transmissionStatus.blocked_count ? dataGuardManager.transmissionStatus.blocked_count : "0"
                        color: "#ff0000"
                        font.pixelSize: 32
                        font.bold: true
                        Layout.fillWidth: true
                    }
                }
            }

            Card {
                Layout.fillWidth: true
                Layout.fillHeight: true
                variant: "dashboard-card"

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 8

                    Text {
                        text: "Allowed Transmissions"
                        color: "#cccccc"
                        font.pixelSize: 14
                        Layout.fillWidth: true
                    }

                    Text {
                        text: dataGuardManager && dataGuardManager.transmissionStatus.allowed_count ? dataGuardManager.transmissionStatus.allowed_count : "0"
                        color: "#00ff00"
                        font.pixelSize: 32
                        font.bold: true
                        Layout.fillWidth: true
                    }
                }
            }

            Card {
                Layout.fillWidth: true
                Layout.fillHeight: true
                variant: "dashboard-card"

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 8

                    Text {
                        text: "Total Transmissions"
                        color: "#cccccc"
                        font.pixelSize: 14
                        Layout.fillWidth: true
                    }

                    Text {
                        text: {
                            var blocked = dataGuardManager && dataGuardManager.transmissionStatus.blocked_count ? dataGuardManager.transmissionStatus.blocked_count : 0
                            var allowed = dataGuardManager && dataGuardManager.transmissionStatus.allowed_count ? dataGuardManager.transmissionStatus.allowed_count : 0
                            return (blocked + allowed).toString()
                        }
                        color: "#00ffff"
                        font.pixelSize: 32
                        font.bold: true
                        Layout.fillWidth: true
                    }
                }
            }
        }

        // Health status
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 150
            variant: "dashboard-card"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                Text {
                    text: "Service Health"
                    color: "#ffffff"
                    font.pixelSize: 16
                    font.bold: true
                    Layout.fillWidth: true
                }

                StatusIndicator {
                    Layout.preferredWidth: 300
                    Layout.preferredHeight: 40
                    variant: "status-light"
                    state: dataGuardManager && dataGuardManager.healthStatus.healthy ? "active" : "critical"
                    text: dataGuardManager && dataGuardManager.healthStatus.healthy ? "Healthy" : "Unhealthy"
                }

                Text {
                    text: dataGuardManager && dataGuardManager.healthStatus.status ? dataGuardManager.healthStatus.status : "Unknown"
                    color: "#cccccc"
                    font.pixelSize: 12
                    Layout.fillWidth: true
                }
            }
        }
    }

    Component.onCompleted: {
        if (dataGuardManager) {
            dataGuardManager.refreshTransmissionStatus()
            dataGuardManager.refreshHealthStatus()
        }
    }
}

