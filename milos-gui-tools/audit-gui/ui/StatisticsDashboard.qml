import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: statisticsDashboard
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
                text: "Event Statistics"
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
                    if (auditManager) {
                        auditManager.refreshStatistics()
                    }
                }
            }
        }

        // Statistics cards
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
                        text: "Total Events"
                        color: "#cccccc"
                        font.pixelSize: 14
                        Layout.fillWidth: true
                    }

                    Text {
                        text: auditManager && auditManager.statistics ? (auditManager.statistics.total_events || "0") : "0"
                        color: "#00ffff"
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
                        text: "Critical Events"
                        color: "#cccccc"
                        font.pixelSize: 14
                        Layout.fillWidth: true
                    }

                    Text {
                        text: auditManager && auditManager.statistics ? (auditManager.statistics.critical_events || "0") : "0"
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
                        text: "Error Events"
                        color: "#cccccc"
                        font.pixelSize: 14
                        Layout.fillWidth: true
                    }

                    Text {
                        text: auditManager && auditManager.statistics ? (auditManager.statistics.error_events || "0") : "0"
                        color: "#ff8800"
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
                        text: "Warning Events"
                        color: "#cccccc"
                        font.pixelSize: 14
                        Layout.fillWidth: true
                    }

                    Text {
                        text: auditManager && auditManager.statistics ? (auditManager.statistics.warning_events || "0") : "0"
                        color: "#ffff00"
                        font.pixelSize: 32
                        font.bold: true
                        Layout.fillWidth: true
                    }
                }
            }
        }

        // Distribution chart
        Card {
            Layout.fillWidth: true
            Layout.fillHeight: true
            variant: "dashboard-card"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                Text {
                    text: "Event Distribution"
                    color: "#ffffff"
                    font.pixelSize: 16
                    font.bold: true
                    Layout.fillWidth: true
                }

                // Data visualization component
                DataVisualization {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    variant: "bar-chart"
                    data: auditManager && auditManager.statistics ? auditManager.statistics.event_distribution : []
                }
            }
        }
    }

    Component.onCompleted: {
        if (auditManager) {
            auditManager.refreshStatistics()
        }
    }
}

