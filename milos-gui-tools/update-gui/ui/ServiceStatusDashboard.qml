import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: serviceStatusDashboard
    color: "transparent"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // Header
        Text {
            text: "Service Status"
            color: "#ffffff"
            font.pixelSize: 20
            font.bold: true
            Layout.fillWidth: true
        }

        // Service availability card
        Card {
            id: availabilityCard
            Layout.fillWidth: true
            Layout.preferredHeight: 100
            variant: "status-card"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 16

                StatusIndicator {
                    variant: "status-light"
                    state: dbusInterface && dbusInterface.serviceAvailable ? "active" : "critical"
                    Layout.preferredWidth: 60
                    Layout.preferredHeight: 60
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 4

                    Text {
                        text: "Service Availability"
                        color: "#ffffff"
                        font.pixelSize: 16
                        font.bold: true
                        Layout.fillWidth: true
                    }

                    Text {
                        text: dbusInterface && dbusInterface.serviceAvailable ? 
                              "Update Service is available and running" : 
                              "Update Service is not available"
                        color: dbusInterface && dbusInterface.serviceAvailable ? "#00cc66" : "#ff4444"
                        font.pixelSize: 14
                        Layout.fillWidth: true
                    }
                }

                Button {
                    Layout.preferredWidth: 120
                    Layout.preferredHeight: 40
                    text: "Refresh"
                    variant: "secondary"
                    onClicked: {
                        if (dbusInterface) {
                            dbusInterface.refreshConnection()
                        }
                    }
                }
            }
        }

        // Current status card
        Card {
            id: statusCard
            Layout.fillWidth: true
            Layout.preferredHeight: 120
            variant: "dashboard-card"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 8

                Text {
                    text: "Current Update Status"
                    color: "#ffffff"
                    font.pixelSize: 16
                    font.bold: true
                    Layout.fillWidth: true
                }

                Text {
                    text: dbusInterface ? dbusInterface.currentStatus : "Unknown"
                    color: "#b0b0b0"
                    font.pixelSize: 14
                    Layout.fillWidth: true
                }

                Text {
                    text: "Current Update ID: " + (updateManager ? updateManager.currentUpdateId : "None")
                    color: "#b0b0b0"
                    font.pixelSize: 12
                    Layout.fillWidth: true
                    visible: updateManager && updateManager.currentUpdateId
                }
            }
        }

        // Health status card
        Card {
            id: healthCard
            Layout.fillWidth: true
            Layout.preferredHeight: 200
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

                // Health status will be displayed here
                // This would be populated from GetHealthStatus() response
                Text {
                    text: "Health status information will be displayed here"
                    color: "#b0b0b0"
                    font.pixelSize: 14
                    Layout.fillWidth: true
                }
            }
        }

        // Last check time
        Card {
            id: lastCheckCard
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            variant: "detail-card"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                Text {
                    text: "Last Update Check:"
                    color: "#ffffff"
                    font.pixelSize: 14
                }

                Text {
                    text: "Not checked yet"
                    color: "#b0b0b0"
                    font.pixelSize: 14
                    Layout.fillWidth: true
                }
            }
        }
    }

    Component.onCompleted: {
        // Refresh health status on load
        if (dbusInterface) {
            dbusInterface.getHealthStatus()
        }
    }
}


