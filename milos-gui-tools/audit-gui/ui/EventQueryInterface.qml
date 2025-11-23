import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: eventQueryInterface
    color: "transparent"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // Header
        Text {
            text: "Query Audit Events"
            color: "#ffffff"
            font.pixelSize: 20
            font.bold: true
            Layout.fillWidth: true
        }

        // Filter panel
        Card {
            id: filterCard
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            variant: "dashboard-card"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                Text {
                    text: "Filters"
                    color: "#ffffff"
                    font.pixelSize: 16
                    font.bold: true
                    Layout.fillWidth: true
                }

                GridLayout {
                    Layout.fillWidth: true
                    columns: 2
                    columnSpacing: 16
                    rowSpacing: 12

                    Text {
                        text: "Event Type:"
                        color: "#cccccc"
                        Layout.preferredWidth: 120
                    }
                    ComboBox {
                        id: eventTypeFilter
                        Layout.fillWidth: true
                        model: ["All", "ENCRYPTION", "NETWORK", "AUTHENTICATION", "CONFIGURATION", "SYSTEM"]
                    }

                    Text {
                        text: "Severity:"
                        color: "#cccccc"
                        Layout.preferredWidth: 120
                    }
                    ComboBox {
                        id: severityFilter
                        Layout.fillWidth: true
                        model: ["All", "INFO", "WARNING", "ERROR", "CRITICAL"]
                    }

                    Text {
                        text: "Source:"
                        color: "#cccccc"
                        Layout.preferredWidth: 120
                    }
                    TextField {
                        id: sourceFilter
                        Layout.fillWidth: true
                        placeholderText: "Component name"
                    }

                    Text {
                        text: "Date From:"
                        color: "#cccccc"
                        Layout.preferredWidth: 120
                    }
                    TextField {
                        id: dateFromFilter
                        Layout.fillWidth: true
                        placeholderText: "YYYY-MM-DD HH:MM:SS"
                    }

                    Text {
                        text: "Date To:"
                        color: "#cccccc"
                        Layout.preferredWidth: 120
                    }
                    TextField {
                        id: dateToFilter
                        Layout.fillWidth: true
                        placeholderText: "YYYY-MM-DD HH:MM:SS"
                    }
                }

                // Query button
                Button {
                    id: queryButton
                    Layout.fillWidth: true
                    Layout.preferredHeight: 48
                    text: auditManager && auditManager.queryingEvents ? "Querying..." : "Query Events"
                    variant: "primary"
                    state: auditManager && auditManager.queryingEvents ? "loading" : "default"
                    enabled: !(auditManager && auditManager.queryingEvents) && dbusInterface && dbusInterface.serviceAvailable

                    onClicked: {
                        if (auditManager) {
                            var filters = {}
                            if (eventTypeFilter.currentText !== "All") {
                                filters["event_type"] = eventTypeFilter.currentText
                            }
                            if (severityFilter.currentText !== "All") {
                                filters["event_severity"] = severityFilter.currentText
                            }
                            if (sourceFilter.text !== "") {
                                filters["source_component"] = sourceFilter.text
                            }
                            if (dateFromFilter.text !== "") {
                                filters["timestamp_from"] = dateFromFilter.text
                            }
                            if (dateToFilter.text !== "") {
                                filters["timestamp_to"] = dateToFilter.text
                            }
                            auditManager.queryEvents(filters)
                        }
                    }
                }
            }
        }

        // Results list
        Card {
            id: resultsCard
            Layout.fillWidth: true
            Layout.fillHeight: true
            variant: "dashboard-card"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                Text {
                    text: "Query Results"
                    color: "#ffffff"
                    font.pixelSize: 16
                    font.bold: true
                    Layout.fillWidth: true
                }

                // Events list
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    ListView {
                        id: eventsList
                        model: auditManager ? auditManager.eventList : []
                        spacing: 8

                        delegate: Card {
                            width: eventsList.width
                            height: 80
                            variant: "status-card"

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 12
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
                                        text: modelData.event_type || "Unknown"
                                        color: "#ffffff"
                                        font.pixelSize: 14
                                        font.bold: true
                                        Layout.fillWidth: true
                                    }

                                    Text {
                                        text: (modelData.source_component || "Unknown") + " - " + (modelData.timestamp || "")
                                        color: "#cccccc"
                                        font.pixelSize: 12
                                        Layout.fillWidth: true
                                    }

                                    Text {
                                        text: modelData.event_severity || "INFO"
                                        color: "#00ffff"
                                        font.pixelSize: 11
                                        Layout.fillWidth: true
                                    }
                                }

                                Button {
                                    text: "Details"
                                    variant: "secondary"
                                    Layout.preferredWidth: 80
                                    onClicked: {
                                        // Navigate to event viewer with this event
                                        // This would be handled by the main window
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

