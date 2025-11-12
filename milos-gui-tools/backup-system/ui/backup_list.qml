import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents

/**
 * Backup List Component
 * 
 * Displays list of backups with management options.
 */
Rectangle {
    id: backupList
    property var backupService: null

    color: "#0a0a0a"
    border.color: "#00ffff"
    border.width: 1
    radius: 4

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        RowLayout {
            Layout.fillWidth: true

            Text {
                text: "Backups"
                color: "#00ffff"
                font.pixelSize: 12
                font.bold: true
            }

            Item {
                Layout.fillWidth: true
            }

            PlasmaComponents.Button {
                text: "Create Backup"
                onClicked: {
                    // Open backup wizard
                }
            }
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ListView {
                id: backupsListView
                model: backupService ? parseBackups(backupService.listBackups()) : []
                spacing: 5

                delegate: Rectangle {
                    width: backupsListView.width
                    height: 80
                    color: "#1a1a1a"
                    radius: 4
                    border.color: "#00ffff"
                    border.width: 1

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 10

                        Column {
                            Layout.fillWidth: true
                            spacing: 5

                            Text {
                                text: modelData.name || "Unnamed Backup"
                                color: "#00ffff"
                                font.pixelSize: 12
                                font.bold: true
                            }

                            Text {
                                text: "Source: " + (modelData.source_path || "")
                                color: "#ffffff"
                                font.pixelSize: 10
                            }

                            Text {
                                text: "Timestamp: " + formatTimestamp(modelData.timestamp || 0)
                                color: "#888888"
                                font.pixelSize: 10
                            }
                        }

                        PlasmaComponents.Button {
                            text: "Restore"
                            onClicked: {
                                // Restore backup
                            }
                        }

                        PlasmaComponents.Button {
                            text: "Delete"
                            onClicked: {
                                // Delete backup
                            }
                        }
                    }
                }
            }
        }
    }

    function parseBackups(jsonString) {
        try {
            var doc = JSON.parse(jsonString);
            if (Array.isArray(doc)) {
                return doc;
            }
        } catch (e) {
            console.log("Error parsing backups:", e);
        }
        return [];
    }

    function formatTimestamp(timestamp) {
        if (!timestamp) return "";
        var date = new Date(timestamp * 1000);
        return date.toLocaleString();
    }
}

