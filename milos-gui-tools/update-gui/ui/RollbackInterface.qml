import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: rollbackInterface
    color: "transparent"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // Header
        Text {
            text: "Rollback Updates"
            color: "#ffffff"
            font.pixelSize: 20
            font.bold: true
            Layout.fillWidth: true
        }

        // Warning message
        Card {
            id: warningCard
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            variant: "alert-card"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                StatusIndicator {
                    variant: "status-icon"
                    state: "warning"
                    Layout.preferredWidth: 40
                    Layout.preferredHeight: 40
                }

                Text {
                    text: "Warning: Rolling back an update will restore previous package versions. This action cannot be undone."
                    color: "#ffaa00"
                    font.pixelSize: 14
                    Layout.fillWidth: true
                    wrapMode: Text.Wrap
                }
            }
        }

        // Rollback selection
        Card {
            id: rollbackCard
            Layout.fillWidth: true
            Layout.fillHeight: true
            variant: "dashboard-card"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                Text {
                    text: "Select Update to Rollback"
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
                        id: rollbackList
                        model: updateManager ? updateManager.updateHistory : []
                        spacing: 12

                        delegate: Card {
                            width: rollbackList.width
                            height: 140
                            variant: "interactive-card"

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 16
                                spacing: 8

                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 12

                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 4

                                        Text {
                                            text: "Update ID: " + (modelData.update_id || "Unknown")
                                            color: "#ffffff"
                                            font.pixelSize: 14
                                            font.bold: true
                                            Layout.fillWidth: true
                                        }

                                        Text {
                                            text: "Date: " + (modelData.timestamp || "Unknown")
                                            color: "#b0b0b0"
                                            font.pixelSize: 12
                                            Layout.fillWidth: true
                                        }

                                        Text {
                                            text: "Status: " + (modelData.status || "Unknown")
                                            color: "#b0b0b0"
                                            font.pixelSize: 12
                                            Layout.fillWidth: true
                                        }
                                    }

                                    StatusIndicator {
                                        variant: "status-badge"
                                        state: getStatusState(modelData.status)
                                        text: modelData.status || "Unknown"
                                    }
                                }

                                // Package list
                                Text {
                                    text: "Packages: " + (modelData.package_list ? modelData.package_list.join(", ") : "None")
                                    color: "#e0e0e0"
                                    font.pixelSize: 11
                                    Layout.fillWidth: true
                                    wrapMode: Text.Wrap
                                }

                                // Rollback button
                                Button {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 36
                                    text: "Rollback This Update"
                                    variant: "secondary"
                                    enabled: modelData.status === "COMPLETED" && 
                                             dbusInterface && dbusInterface.serviceAvailable

                                    onClicked: {
                                        if (updateManager && modelData.update_id) {
                                            // Show confirmation dialog
                                            rollbackConfirmationDialog.updateId = modelData.update_id
                                            rollbackConfirmationDialog.visible = true
                                        }
                                    }
                                }
                            }
                        }

                        // Empty state
                        Text {
                            anchors.centerIn: parent
                            text: "No updates available for rollback"
                            color: "#b0b0b0"
                            font.pixelSize: 14
                            visible: rollbackList.count === 0
                        }
                    }
                }
            }
        }
    }

    function getStatusState(status) {
        switch(status) {
            case "COMPLETED": return "active"
            case "FAILED": return "critical"
            case "IN_PROGRESS": return "processing"
            case "ROLLED_BACK": return "warning"
            default: return "inactive"
        }
    }

    // Confirmation dialog
    ModalDialog {
        id: rollbackConfirmationDialog
        property string updateId: ""
        variant: "warning"
        title: "Confirm Rollback"
        message: "Are you sure you want to rollback update " + updateId + "? This action cannot be undone."
        visible: false

        onAccepted: {
            if (updateManager && updateId) {
                updateManager.performRollback(updateId)
            }
            visible = false
        }

        onRejected: {
            visible = false
        }
    }

    Component.onCompleted: {
        if (updateManager) {
            updateManager.refreshHistory()
        }
    }

    Connections {
        target: updateManager
        function onRollbackCompleted(updateId, success) {
            if (success) {
                // Refresh history after successful rollback
                updateManager.refreshHistory()
            }
        }
        function onRollbackFailed(updateId, error) {
            // Show error notification
            console.log("Rollback failed:", updateId, error)
        }
    }
}


