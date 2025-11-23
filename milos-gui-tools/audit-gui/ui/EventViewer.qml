import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: eventViewer
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
                text: "Event Viewer"
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
                        auditManager.queryEvents({})
                    }
                }
            }
        }

        // Event list
        Card {
            Layout.fillWidth: true
            Layout.fillHeight: true
            variant: "dashboard-card"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                Text {
                    text: "Recent Events"
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
                        id: eventsListView
                        model: auditManager ? auditManager.eventList : []
                        spacing: 8

                        delegate: Card {
                            width: eventsListView.width
                            height: 120
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
                                        state: {
                                            var severity = modelData.event_severity || ""
                                            if (severity === "CRITICAL") return "critical"
                                            if (severity === "ERROR") return "error"
                                            if (severity === "WARNING") return "warning"
                                            return "active"
                                        }
                                    }

                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 4

                                        Text {
                                            text: modelData.event_type || "Unknown Event"
                                            color: "#ffffff"
                                            font.pixelSize: 16
                                            font.bold: true
                                            Layout.fillWidth: true
                                        }

                                        Text {
                                            text: "Source: " + (modelData.source_component || "Unknown")
                                            color: "#cccccc"
                                            font.pixelSize: 12
                                            Layout.fillWidth: true
                                        }

                                        Text {
                                            text: "Timestamp: " + (modelData.timestamp || "Unknown")
                                            color: "#cccccc"
                                            font.pixelSize: 12
                                            Layout.fillWidth: true
                                        }
                                    }
                                }

                                Text {
                                    text: "Severity: " + (modelData.event_severity || "INFO")
                                    color: {
                                        var severity = modelData.event_severity || ""
                                        if (severity === "CRITICAL") return "#ff0000"
                                        if (severity === "ERROR") return "#ff8800"
                                        if (severity === "WARNING") return "#ffff00"
                                        return "#00ffff"
                                    }
                                    font.pixelSize: 12
                                    font.bold: true
                                    Layout.fillWidth: true
                                }

                                Text {
                                    text: "Event ID: " + (modelData.event_id || "Unknown")
                                    color: "#888888"
                                    font.pixelSize: 11
                                    Layout.fillWidth: true
                                }

                                // Event data (if available)
                                Text {
                                    visible: modelData.event_data !== undefined
                                    text: "Data: " + JSON.stringify(modelData.event_data || {})
                                    color: "#888888"
                                    font.pixelSize: 10
                                    wrapMode: Text.Wrap
                                    Layout.fillWidth: true
                                    maximumLineCount: 2
                                    elide: Text.ElideRight
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        if (auditManager) {
            auditManager.queryEvents({})
        }
    }
}

