import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.milos.Components 1.0
import org.milos.NavigationReset 1.0

Dialog {
    id: resetDialog
    title: "Reset Navigation Settings"
    width: 700
    height: 600
    modal: true
    
    property var resetUI: NavigationResetUI {
        id: resetManager
    }
    
    property string selectedResetType: "Full"
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 20
        
        // Reset Type Selection
        GroupBox {
            title: "Reset Type"
            Layout.fillWidth: true
            
            ColumnLayout {
                anchors.fill: parent
                spacing: 10
                
                ComboBox {
                    id: resetTypeComboBox
                    Layout.fillWidth: true
                    model: resetManager.resetTypes
                    currentIndex: 0
                    onCurrentTextChanged: {
                        selectedResetType = currentText
                        resetManager.previewReset(currentText)
                    }
                }
                
                // Reset Preview
                ScrollView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 150
                    visible: resetManager.resetPreview && Object.keys(resetManager.resetPreview).length > 0
                    
                    Text {
                        text: {
                            var preview = resetManager.resetPreview
                            if (!preview || Object.keys(preview).length === 0) return ""
                            
                            var text = "The following will be reset:\n\n"
                            if (preview.files) {
                                text += "Files: " + preview.files.length + "\n"
                            }
                            if (preview.settings) {
                                text += "Settings: " + preview.settings.length + "\n"
                            }
                            if (preview.warnings) {
                                text += "\nWarnings:\n"
                                var warnings = preview.warnings
                                for (var i = 0; i < warnings.length; i++) {
                                    text += "• " + warnings[i] + "\n"
                                }
                            }
                            return text
                        }
                        wrapMode: Text.WordWrap
                    }
                }
            }
        }
        
        // Progress Indicator
        GroupBox {
            title: "Reset Progress"
            Layout.fillWidth: true
            visible: resetManager.resetInProgress
            
            ColumnLayout {
                anchors.fill: parent
                spacing: 10
                
                ProgressBar {
                    id: progressBar
                    Layout.fillWidth: true
                    value: resetManager.resetProgress / 100.0
                    
                    // Xenon-accented progress bar
                    background: Rectangle {
                        color: "#1a1a2e"
                        border.color: "#0f3460"
                        border.width: 2
                    }
                    
                    contentItem: Item {
                        Rectangle {
                            width: progressBar.visualPosition * parent.width
                            height: parent.height
                            color: "#16213e"
                            border.color: "#0ea5e9"
                            border.width: 2
                            
                            // Xenon glow effect
                            Rectangle {
                                anchors.fill: parent
                                color: "#0ea5e9"
                                opacity: 0.3
                                radius: 2
                            }
                        }
                    }
                }
                
                Text {
                    text: Math.round(resetManager.resetProgress) + "%"
                    horizontalAlignment: Text.AlignHCenter
                    Layout.fillWidth: true
                }
            }
        }
        
        // Backup Management
        GroupBox {
            title: "Backup Management"
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ColumnLayout {
                anchors.fill: parent
                spacing: 10
                
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    
                    ListView {
                        id: backupList
                        model: resetManager.backups
                        
                        delegate: Card {
                            width: backupList.width
                            height: 80
                            
                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 10
                                spacing: 10
                                
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 5
                                    
                                    Text {
                                        text: modelData.backup_id || "Unknown Backup"
                                        font.bold: true
                                    }
                                    
                                    Text {
                                        text: "Created: " + (modelData.backup_timestamp || "Unknown")
                                        font.pixelSize: 12
                                        color: "#888"
                                    }
                                    
                                    Text {
                                        text: "Type: " + (modelData.backup_type || "Unknown")
                                        font.pixelSize: 12
                                        color: "#888"
                                    }
                                }
                                
                                Button {
                                    text: "Restore"
                                    onClicked: {
                                        restoreBackupDialog.backupPath = modelData.backup_path
                                        restoreBackupDialog.open()
                                    }
                                }
                                
                                Button {
                                    text: "Delete"
                                    onClicked: {
                                        deleteBackupDialog.backupPath = modelData.backup_path
                                        deleteBackupDialog.open()
                                    }
                                }
                            }
                        }
                    }
                }
                
                Button {
                    text: "Refresh Backups"
                    onClicked: resetManager.loadBackups()
                }
            }
        }
        
        // Buttons
        RowLayout {
            Layout.fillWidth: true
            
            Button {
                text: "Cancel"
                enabled: !resetManager.resetInProgress
                onClicked: {
                    if (resetManager.resetInProgress) {
                        resetManager.cancelReset()
                    }
                    resetDialog.close()
                }
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: "Reset"
                enabled: !resetManager.resetInProgress && selectedResetType
                onClicked: {
                    confirmationDialog.open()
                }
            }
        }
    }
    
    // Confirmation Dialog with Xenon Warning Border
    Dialog {
        id: confirmationDialog
        title: "Confirm Reset"
        width: 500
        modal: true
        
        // Xenon warning border
        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: "#ff6b6b"
            border.width: 3
            radius: 5
            
            // Xenon glow
            Rectangle {
                anchors.fill: parent
                color: "#ff6b6b"
                opacity: 0.2
                radius: 5
            }
        }
        
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 20
            
            Text {
                text: "Are you sure you want to reset " + selectedResetType + " navigation settings?\n\nThis action cannot be undone. A backup will be created automatically."
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
            
            RowLayout {
                Layout.fillWidth: true
                
                Item { Layout.fillWidth: true }
                
                Button {
                    text: "Cancel"
                    onClicked: confirmationDialog.close()
                }
                
                Button {
                    text: "Confirm Reset"
                    onClicked: {
                        resetManager.executeReset(selectedResetType)
                        confirmationDialog.close()
                    }
                }
            }
        }
    }
    
    // Restore Backup Confirmation
    Dialog {
        id: restoreBackupDialog
        title: "Restore Backup"
        width: 400
        modal: true
        
        property string backupPath: ""
        
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 20
            
            Text {
                text: "Are you sure you want to restore this backup?\n\n" + backupPath
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
            
            RowLayout {
                Layout.fillWidth: true
                
                Item { Layout.fillWidth: true }
                
                Button {
                    text: "Cancel"
                    onClicked: restoreBackupDialog.close()
                }
                
                Button {
                    text: "Restore"
                    onClicked: {
                        resetManager.restoreBackup(restoreBackupDialog.backupPath)
                        restoreBackupDialog.close()
                    }
                }
            }
        }
    }
    
    // Delete Backup Confirmation
    Dialog {
        id: deleteBackupDialog
        title: "Delete Backup"
        width: 400
        modal: true
        
        property string backupPath: ""
        
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 20
            
            Text {
                text: "Are you sure you want to delete this backup?\n\n" + backupPath
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
            
            RowLayout {
                Layout.fillWidth: true
                
                Item { Layout.fillWidth: true }
                
                Button {
                    text: "Cancel"
                    onClicked: deleteBackupDialog.close()
                }
                
                Button {
                    text: "Delete"
                    onClicked: {
                        resetManager.deleteBackup(deleteBackupDialog.backupPath)
                        deleteBackupDialog.close()
                    }
                }
            }
        }
    }
    
    // Completion Notification
    Connections {
        target: resetManager
        
        function onResetCompleted(status, backupPath) {
            completionNotification.show(status, backupPath)
        }
    }
    
    Notification {
        id: completionNotification
        
        function show(status, backupPath) {
            var message = "Reset completed: " + status
            if (backupPath) {
                message += "\n\nBackup created: " + backupPath
                message += "\n\nWould you like to restore this backup?"
            }
            // Show notification with restore option
        }
    }
}

