import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: logExportInterface
    color: "transparent"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // Header
        Text {
            text: "Export Audit Logs"
            color: "#ffffff"
            font.pixelSize: 20
            font.bold: true
            Layout.fillWidth: true
        }

        // Export configuration
        Card {
            id: exportConfigCard
            Layout.fillWidth: true
            Layout.preferredHeight: 300
            variant: "dashboard-card"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                Text {
                    text: "Export Configuration"
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
                        text: "Format:"
                        color: "#cccccc"
                        Layout.preferredWidth: 120
                    }
                    ComboBox {
                        id: formatComboBox
                        Layout.fillWidth: true
                        model: ["JSON", "CSV", "XML"]
                        currentIndex: 0
                    }

                    Text {
                        text: "Date From:"
                        color: "#cccccc"
                        Layout.preferredWidth: 120
                    }
                    TextField {
                        id: exportDateFrom
                        Layout.fillWidth: true
                        placeholderText: "YYYY-MM-DD HH:MM:SS"
                    }

                    Text {
                        text: "Date To:"
                        color: "#cccccc"
                        Layout.preferredWidth: 120
                    }
                    TextField {
                        id: exportDateTo
                        Layout.fillWidth: true
                        placeholderText: "YYYY-MM-DD HH:MM:SS"
                    }
                }

                // Export button
                Button {
                    id: exportButton
                    Layout.fillWidth: true
                    Layout.preferredHeight: 48
                    text: auditManager && auditManager.exportingLogs ? "Exporting..." : "Export Logs"
                    variant: "primary"
                    state: auditManager && auditManager.exportingLogs ? "loading" : "default"
                    enabled: !(auditManager && auditManager.exportingLogs) && dbusInterface && dbusInterface.serviceAvailable

                    onClicked: {
                        if (auditManager) {
                            var timeRange = {}
                            if (exportDateFrom.text !== "") {
                                timeRange["from"] = exportDateFrom.text
                            }
                            if (exportDateTo.text !== "") {
                                timeRange["to"] = exportDateTo.text
                            }
                            var format = formatComboBox.currentText.toLowerCase()
                            auditManager.exportLogs(timeRange, format)
                        }
                    }
                }
            }
        }

        // Export status
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 150
            variant: "dashboard-card"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                Text {
                    text: "Export Status"
                    color: "#ffffff"
                    font.pixelSize: 16
                    font.bold: true
                    Layout.fillWidth: true
                }

                StatusIndicator {
                    Layout.preferredWidth: 200
                    Layout.preferredHeight: 40
                    variant: "status-light"
                    state: auditManager && auditManager.exportingLogs ? "loading" : "active"
                    text: auditManager && auditManager.exportingLogs ? "Exporting..." : "Ready"
                }

                Text {
                    id: exportStatusText
                    text: "No export in progress"
                    color: "#cccccc"
                    font.pixelSize: 12
                    Layout.fillWidth: true
                }
            }
        }
    }

    Connections {
        target: auditManager
        function onExportCompleted(filePath, format) {
            exportStatusText.text = "Export completed: " + filePath
        }
        function onExportFailed(error) {
            exportStatusText.text = "Export failed: " + error
        }
    }
}

