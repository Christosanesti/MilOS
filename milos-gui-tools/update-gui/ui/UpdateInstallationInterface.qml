import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: updateInstallationInterface
    color: "transparent"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // Header
        Text {
            text: "Install Updates"
            color: "#ffffff"
            font.pixelSize: 20
            font.bold: true
            Layout.fillWidth: true
        }

        // Update selection
        Card {
            id: selectionCard
            Layout.fillWidth: true
            Layout.preferredHeight: 300
            variant: "dashboard-card"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                Text {
                    text: "Select Updates to Install"
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
                        id: packageList
                        model: updateManager ? updateManager.availableUpdates : []
                        spacing: 8

                        delegate: Card {
                            width: packageList.width
                            height: 60
                            variant: "interactive-card"

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 12
                                spacing: 12

                                Rectangle {
                                    id: checkboxRect
                                    width: 20
                                    height: 20
                                    radius: 4
                                    border.width: 2
                                    border.color: "#00d4ff"
                                    color: checkboxRect.checked ? "#00d4ff" : "transparent"
                                    
                                    property bool checked: true
                                    
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: checkboxRect.checked = !checkboxRect.checked
                                    }
                                    
                                    Text {
                                        anchors.centerIn: parent
                                        text: "✓"
                                        color: "#0a0a0a"
                                        font.pixelSize: 12
                                        visible: checkboxRect.checked
                                    }
                                }

                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 4

                                    Text {
                                        text: modelData.name || modelData || "Unknown Package"
                                        color: "#ffffff"
                                        font.pixelSize: 14
                                        Layout.fillWidth: true
                                    }

                                    Text {
                                        text: modelData.version ? "Version: " + modelData.version : ""
                                        color: "#b0b0b0"
                                        font.pixelSize: 12
                                        Layout.fillWidth: true
                                        visible: modelData.version
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // Progress indicator
        Card {
            id: progressCard
            Layout.fillWidth: true
            Layout.preferredHeight: 150
            variant: "status-card"
            visible: updateManager && updateManager.applyingUpdates

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                Text {
                    text: "Installing Updates"
                    color: "#ffffff"
                    font.pixelSize: 16
                    font.bold: true
                    Layout.fillWidth: true
                }

                ProgressBar {
                    id: updateProgressBar
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    variant: "linear-progress"
                    value: updateManager ? updateManager.updateProgress : 0
                    maximumValue: 100
                    state: "in-progress"
                }

                Text {
                    text: "Progress: " + (updateManager ? updateManager.updateProgress : 0) + "%"
                    color: "#b0b0b0"
                    font.pixelSize: 12
                    Layout.fillWidth: true
                }
            }
        }

        // Action buttons
        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Button {
                id: installButton
                Layout.fillWidth: true
                Layout.preferredHeight: 48
                text: "Install Selected Updates"
                variant: "primary"
                enabled: !(updateManager && updateManager.applyingUpdates) && 
                         dbusInterface && dbusInterface.serviceAvailable &&
                         packageList.count > 0

                onClicked: {
                    if (updateManager) {
                        var selectedPackages = []
                        // Collect selected packages (simplified - in real implementation, track checkbox states)
                        for (var i = 0; i < packageList.count; i++) {
                            var item = packageList.model[i]
                            selectedPackages.push(item.name || item)
                        }
                        updateManager.applySelectedUpdates(selectedPackages)
                    }
                }
            }

            Button {
                id: cancelButton
                Layout.preferredWidth: 150
                Layout.preferredHeight: 48
                text: "Cancel"
                variant: "secondary"
                enabled: updateManager && updateManager.applyingUpdates

                onClicked: {
                    if (updateManager) {
                        updateManager.cancelUpdate()
                    }
                }
            }
        }
    }

    Connections {
        target: updateManager
        function onUpdateCompleted(updateId, success) {
            if (success) {
                // Show success notification
                console.log("Update completed successfully:", updateId)
            }
        }
        function onUpdateFailed(updateId, error) {
            // Show error notification
            console.log("Update failed:", updateId, error)
        }
    }
}

