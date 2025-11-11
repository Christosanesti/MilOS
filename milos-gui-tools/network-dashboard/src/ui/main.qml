import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents

ApplicationWindow {
    id: mainWindow
    width: 1400
    height: 900
    visible: true
    title: "MilOS Network Defense Dashboard"

    color: "#0a0a0a"

    // Network Monitor backend
    property var networkMonitor: null

    // Main layout
    RowLayout {
        anchors.fill: parent
        spacing: 10

        // Left panel: Statistics and controls
        ColumnLayout {
            Layout.preferredWidth: 300
            Layout.fillHeight: true
            spacing: 10

            // Network Statistics
            NetworkStatistics {
                id: networkStats
                Layout.fillWidth: true
                Layout.preferredHeight: 200
                networkMonitor: mainWindow.networkMonitor
            }

            // Bandwidth Monitor
            BandwidthMonitor {
                id: bandwidthMonitor
                Layout.fillWidth: true
                Layout.preferredHeight: 200
                networkMonitor: mainWindow.networkMonitor
            }

            // Controls
            PlasmaComponents.GroupBox {
                Layout.fillWidth: true
                title: "Controls"

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 10

                    PlasmaComponents.Button {
                        text: networkMonitor && networkMonitor.running ? "Stop Capture" : "Start Capture"
                        Layout.fillWidth: true
                        onClicked: {
                            if (networkMonitor) {
                                if (networkMonitor.running) {
                                    // Stop capture via D-Bus
                                } else {
                                    // Start capture via D-Bus
                                }
                            }
                        }
                    }
                }
            }
        }

        // Center panel: Traffic visualization
        TrafficVisualization {
            id: trafficViz
            Layout.fillWidth: true
            Layout.fillHeight: true
            networkMonitor: mainWindow.networkMonitor
        }

        // Right panel: Connections and alerts
        ColumnLayout {
            Layout.preferredWidth: 400
            Layout.fillHeight: true
            spacing: 10

            // Connection Tracker
            ConnectionTracker {
                id: connectionTracker
                Layout.fillWidth: true
                Layout.preferredHeight: 400
                networkMonitor: mainWindow.networkMonitor
            }

            // Network Alerts
            NetworkAlerts {
                id: networkAlerts
                Layout.fillWidth: true
                Layout.fillHeight: true
                networkMonitor: mainWindow.networkMonitor
            }
        }
    }
}

