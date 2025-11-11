import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.core 2.0 as PlasmaCore

/**
 * Connection Tracker Component
 * 
 * Displays active network connections with details.
 */
Item {
    id: connectionTracker
    property var networkMonitor: null

    Rectangle {
        anchors.fill: parent
        color: "#0a0a0a"
        border.color: "#00ffff"
        border.width: 1
        radius: 4

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 10

            // Title
            Text {
                text: "Active Connections"
                color: "#00ffff"
                font.pixelSize: 14
                font.bold: true
            }

            // Connection list
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true

                ListView {
                    id: connectionList
                    model: networkMonitor ? networkMonitor.activeConnections : []
                    spacing: 5

                    delegate: Rectangle {
                        width: connectionList.width
                        height: 60
                        color: "#1a1a1a"
                        radius: 4
                        border.color: "#00ffff"
                        border.width: 1

                        // Xenon glow on hover
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: {
                                parent.border.width = 2
                                parent.color = "#2a2a2a"
                            }
                            onExited: {
                                parent.border.width = 1
                                parent.color = "#1a1a1a"
                            }
                            onClicked: {
                                // Show connection details
                                var details = networkMonitor.getConnectionDetails(modelData.id);
                                console.log("Connection details:", JSON.stringify(details));
                            }
                        }

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 10
                            spacing: 10

                            Column {
                                Layout.fillWidth: true
                                spacing: 5

                                Text {
                                    text: modelData.source + ":" + modelData.source_port + " → " + 
                                          modelData.destination + ":" + modelData.destination_port
                                    color: "#ffffff"
                                    font.pixelSize: 12
                                }

                                Text {
                                    text: modelData.protocol + " • " + 
                                          formatBytes(modelData.byte_count) + " • " + 
                                          modelData.packet_count + " packets"
                                    color: "#888888"
                                    font.pixelSize: 10
                                }
                            }

                            Text {
                                text: modelData.protocol
                                color: "#00ffff"
                                font.pixelSize: 12
                                font.bold: true
                            }
                        }
                    }
                }
            }
        }
    }

    function formatBytes(bytes) {
        if (bytes < 1024) return bytes + " B";
        if (bytes < 1024 * 1024) return (bytes / 1024).toFixed(2) + " KB";
        if (bytes < 1024 * 1024 * 1024) return (bytes / (1024 * 1024)).toFixed(2) + " MB";
        return (bytes / (1024 * 1024 * 1024)).toFixed(2) + " GB";
    }
}

