import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.core 2.0 as PlasmaCore

/**
 * Network Alerts Component
 * 
 * Displays network security alerts with xenon-accented indicators.
 */
Item {
    id: networkAlerts
    property var networkMonitor: null
    property var alerts: []

    Rectangle {
        anchors.fill: parent
        color: "#0a0a0a"
        border.color: "#ff0000"
        border.width: 1
        radius: 4

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 10

            // Title
            RowLayout {
                Layout.fillWidth: true

                Text {
                    text: "Network Alerts"
                    color: "#ff0000"
                    font.pixelSize: 14
                    font.bold: true
                }

                // Alert count badge
                Rectangle {
                    width: 30
                    height: 20
                    color: "#ff0000"
                    radius: 10
                    visible: alerts.length > 0

                    Text {
                        anchors.centerIn: parent
                        text: alerts.length
                        color: "#ffffff"
                        font.pixelSize: 11
                        font.bold: true
                    }
                }
            }

            // Alert list
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true

                ColumnLayout {
                    width: networkAlerts.width - 20
                    spacing: 5

                    Repeater {
                        model: alerts

                        Rectangle {
                            Layout.fillWidth: true
                            height: 50
                            color: "#2a0000"
                            radius: 4
                            border.color: "#ff0000"
                            border.width: 1

                            // Xenon glow animation
                            SequentialAnimation on border.width {
                                running: true
                                loops: Animation.Infinite
                                NumberAnimation {
                                    from: 1
                                    to: 3
                                    duration: 1000
                                }
                                NumberAnimation {
                                    from: 3
                                    to: 1
                                    duration: 1000
                                }
                            }

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 10
                                spacing: 10

                                // Alert icon
                                Rectangle {
                                    width: 10
                                    height: 10
                                    radius: 5
                                    color: "#ff0000"
                                }

                                Column {
                                    Layout.fillWidth: true
                                    spacing: 3

                                    Text {
                                        text: modelData.title || "Network Alert"
                                        color: "#ffffff"
                                        font.pixelSize: 12
                                        font.bold: true
                                    }

                                    Text {
                                        text: modelData.message || "Alert detected"
                                        color: "#888888"
                                        font.pixelSize: 10
                                    }
                                }

                                Text {
                                    text: modelData.severity || "HIGH"
                                    color: "#ff0000"
                                    font.pixelSize: 10
                                    font.bold: true
                                }
                            }
                        }
                    }

                    Text {
                        text: alerts.length === 0 ? "No alerts" : ""
                        color: "#888888"
                        font.pixelSize: 12
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }
    }

    Connections {
        target: networkMonitor
        function onThreatDetected(threat) {
            alerts.push(threat);
            if (alerts.length > 50) {
                alerts.shift();  // Keep only last 50 alerts
            }
        }
    }
}

