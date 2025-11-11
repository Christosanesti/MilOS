import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.core 2.0 as PlasmaCore

/**
 * Network Statistics Component
 * 
 * Displays network statistics including packet counts, byte counts, and protocol distribution.
 */
Item {
    id: networkStats
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
                text: "Network Statistics"
                color: "#00ffff"
                font.pixelSize: 14
                font.bold: true
            }

            // Total statistics
            RowLayout {
                Layout.fillWidth: true
                spacing: 20

                Column {
                    Text {
                        text: "Total Packets"
                        color: "#888888"
                        font.pixelSize: 11
                    }
                    Text {
                        text: networkMonitor ? networkMonitor.totalPackets : 0
                        color: "#00ffff"
                        font.pixelSize: 18
                        font.bold: true
                    }
                }

                Column {
                    Text {
                        text: "Total Bytes"
                        color: "#888888"
                        font.pixelSize: 11
                    }
                    Text {
                        text: networkMonitor ? formatBytes(networkMonitor.totalBytes) : "0 B"
                        color: "#00ffff"
                        font.pixelSize: 18
                        font.bold: true
                    }
                }
            }

            // Protocol distribution
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true

                ColumnLayout {
                    width: networkStats.width - 20
                    spacing: 5

                    Text {
                        text: "Protocol Distribution"
                        color: "#888888"
                        font.pixelSize: 11
                        Layout.fillWidth: true
                    }

                    Repeater {
                        model: networkMonitor ? Object.keys(networkMonitor.protocolStats) : []
                        
                        Rectangle {
                            Layout.fillWidth: true
                            height: 40
                            color: "#1a1a1a"
                            radius: 2

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 5
                                spacing: 10

                                Text {
                                    text: modelData
                                    color: "#ffffff"
                                    font.pixelSize: 12
                                    Layout.preferredWidth: 80
                                }

                                // Progress bar
                                Rectangle {
                                    Layout.fillWidth: true
                                    height: 20
                                    color: "#2a2a2a"
                                    radius: 2

                                    Rectangle {
                                        width: parent.width * (networkMonitor.protocolStats[modelData].percentage_packets / 100)
                                        height: parent.height
                                        color: "#00ffff"
                                        radius: 2
                                        
                                        // Xenon glow effect
                                        Rectangle {
                                            anchors.fill: parent
                                            color: "transparent"
                                            border.color: "#00ffff"
                                            border.width: 1
                                            opacity: 0.5
                                            radius: 2
                                        }
                                    }
                                }

                                Text {
                                    text: networkMonitor.protocolStats[modelData].percentage_packets.toFixed(1) + "%"
                                    color: "#00ffff"
                                    font.pixelSize: 11
                                    Layout.preferredWidth: 50
                                }
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

