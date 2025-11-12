import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents

/**
 * Clipboard History Component
 * 
 * Displays clipboard history with management options.
 */
Rectangle {
    id: clipboardHistory
    property var clipboardService: null

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
                text: "Clipboard History"
                color: "#00ffff"
                font.pixelSize: 12
                font.bold: true
            }

            Item {
                Layout.fillWidth: true
            }

            PlasmaComponents.Button {
                text: "Clear All"
                onClicked: {
                    // Clear all history
                    // This would call clipboardService.clearHistory()
                }
            }
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ListView {
                id: historyList
                model: clipboardService ? parseHistory(clipboardService.getClipboardHistory()) : []
                spacing: 5

                delegate: Rectangle {
                    width: historyList.width
                    height: 50
                    color: "#1a1a1a"
                    radius: 4
                    border.color: "#00ffff"
                    border.width: 1

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
                            // Restore clipboard entry
                            if (clipboardService) {
                                clipboardService.copyToClipboard(modelData.preview || "");
                            }
                        }
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 10

                        Text {
                            text: modelData.data_type || "text"
                            color: "#00ffff"
                            font.pixelSize: 11
                            Layout.preferredWidth: 60
                        }

                        Text {
                            text: modelData.preview || ""
                            color: "#ffffff"
                            font.pixelSize: 11
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }

                        Text {
                            text: formatTimestamp(modelData.timestamp || 0)
                            color: "#888888"
                            font.pixelSize: 10
                            Layout.preferredWidth: 100
                        }
                    }
                }
            }
        }
    }

    function parseHistory(jsonString) {
        try {
            var doc = JSON.parse(jsonString);
            if (Array.isArray(doc)) {
                return doc.map(function(entry) {
                    return {
                        id: entry.id,
                        data_type: entry.data_type,
                        timestamp: entry.timestamp,
                        preview: ""  // Would be decrypted in production
                    };
                });
            }
        } catch (e) {
            console.log("Error parsing history:", e);
        }
        return [];
    }

    function formatTimestamp(timestamp) {
        if (!timestamp) return "";
        var date = new Date(timestamp * 1000);
        return date.toLocaleTimeString();
    }
}

