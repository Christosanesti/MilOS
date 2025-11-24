import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: updateCheckInterface
    color: "transparent"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // Header
        Text {
            text: "Check for Updates"
            color: "#ffffff"
            font.pixelSize: 20
            font.bold: true
            Layout.fillWidth: true
        }

        // Check button
        Button {
            id: checkButton
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            text: updateManager && updateManager.checkingUpdates ? "Checking..." : "Check for Updates"
            variant: "primary"
            state: updateManager && updateManager.checkingUpdates ? "loading" : "default"
            enabled: !(updateManager && updateManager.checkingUpdates) && dbusInterface && dbusInterface.serviceAvailable

            onClicked: {
                if (updateManager) {
                    updateManager.checkForUpdates()
                }
            }
        }

        // Available updates list
        Card {
            id: updatesCard
            Layout.fillWidth: true
            Layout.fillHeight: true
            variant: "dashboard-card"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                Text {
                    text: "Available Updates"
                    color: "#ffffff"
                    font.pixelSize: 16
                    font.bold: true
                    Layout.fillWidth: true
                }

                // Updates list
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    ListView {
                        id: updatesList
                        model: updateManager ? updateManager.availableUpdates : []
                        spacing: 8

                        delegate: Card {
                            width: updatesList.width
                            height: 60
                            variant: "status-card"

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 12
                                spacing: 12

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

                                StatusIndicator {
                                    variant: "status-badge"
                                    state: "active"
                                    text: "Available"
                                }
                            }
                        }

                        // Empty state
                        Text {
                            anchors.centerIn: parent
                            text: updateManager && updateManager.checkingUpdates ? "Checking for updates..." : "No updates available"
                            color: "#b0b0b0"
                            font.pixelSize: 14
                            visible: updatesList.count === 0
                        }
                    }
                }
            }
        }
    }

    Connections {
        target: updateManager
        function onUpdateCheckCompleted(updates) {
            // Updates list will automatically update via model binding
        }
        function onUpdateCheckFailed(error) {
            // Show error notification
            console.log("Update check failed:", error)
        }
    }
}


