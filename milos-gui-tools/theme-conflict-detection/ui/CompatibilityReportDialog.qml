import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.milos.Components 1.0

Dialog {
    id: dialog
    title: "Theme Compatibility Report"
    width: 700
    height: 600
    modal: true
    
    property string themeName: ""
    property var compatibilityDB: null
    property var conflictDetector: null
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 20
        
        // Theme Info
        GroupBox {
            title: "Theme Information"
            Layout.fillWidth: true
            
            GridLayout {
                columns: 2
                anchors.fill: parent
                
                Text { text: "Theme Name:" }
                Text { 
                    text: dialog.themeName
                    font.bold: true
                }
                
                Text { text: "Compatibility Status:" }
                Text {
                    text: {
                        if (!compatibilityDB) return "Unknown"
                        var status = compatibilityDB.getThemeStatus(dialog.themeName)
                        if (status === 0) return "✓ Compatible"
                        if (status === 1) return "✗ Incompatible"
                        return "? Unknown"
                    }
                    font.bold: true
                }
            }
        }
        
        // Conflict Analysis
        GroupBox {
            title: "Conflict Analysis"
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                
                ColumnLayout {
                    width: parent.width
                    spacing: 10
                    
                    Repeater {
                        id: conflictsRepeater
                        model: []
                        
                        Card {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 80
                            
                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 10
                                spacing: 5
                                
                                Text {
                                    text: modelData.type || "Unknown Conflict"
                                    font.bold: true
                                    font.pixelSize: 14
                                }
                                
                                Text {
                                    text: modelData.description || ""
                                    wrapMode: Text.WordWrap
                                    Layout.fillWidth: true
                                }
                                
                                Text {
                                    text: "Impact: " + (modelData.impact || "")
                                    font.pixelSize: 12
                                    color: "#888"
                                }
                            }
                        }
                    }
                    
                    Text {
                        text: conflictsRepeater.count === 0 ? "No conflicts detected" : ""
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignHCenter
                        color: "#888"
                    }
                }
            }
        }
        
        // Actions
        RowLayout {
            Layout.fillWidth: true
            
            Button {
                text: "Validate Theme"
                onClicked: {
                    if (conflictDetector) {
                        validateTheme()
                    }
                }
            }
            
            Button {
                text: "Report Compatibility"
                onClicked: {
                    reportDialog.open()
                }
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: "Close"
                onClicked: dialog.close()
            }
        }
    }
    
    function validateTheme() {
        if (!conflictDetector || !themeName) return
        
        // Trigger validation via D-Bus service
        var validationResult = conflictDetector.ValidateTheme(themeName)
        var conflicts = validationResult.conflicts || []
        
        // Update conflicts list
        var conflictsList = []
        for (var i = 0; i < conflicts.length; i++) {
            var conflict = conflicts[i]
            conflictsList.push({
                type: conflict.type || "Unknown",
                description: conflict.description || "",
                impact: conflict.impact || "",
                severity: conflict.severity || "Medium"
            })
        }
        conflictsRepeater.model = conflictsList
    }
    
    // Report Compatibility Dialog
    Dialog {
        id: reportDialog
        title: "Report Theme Compatibility"
        width: 400
        modal: true
        
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 20
            
            Text {
                text: "Report compatibility status for: " + dialog.themeName
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
            
            ComboBox {
                id: statusComboBox
                Layout.fillWidth: true
                model: ["Compatible", "Incompatible", "Unknown"]
            }
            
            TextArea {
                id: detailsTextArea
                placeholderText: "Additional details (optional)"
                Layout.fillWidth: true
                Layout.preferredHeight: 100
            }
            
            RowLayout {
                Layout.fillWidth: true
                
                Item { Layout.fillWidth: true }
                
                Button {
                    text: "Cancel"
                    onClicked: reportDialog.close()
                }
                
                Button {
                    text: "Submit"
                    onClicked: {
                        if (compatibilityDB) {
                            var status = statusComboBox.currentIndex
                            compatibilityDB.reportThemeCompatibility(
                                dialog.themeName,
                                status,
                                detailsTextArea.text
                            )
                        }
                        reportDialog.close()
                    }
                }
            }
        }
    }
    
    Component.onCompleted: {
        if (themeName) {
            validateTheme()
        }
    }
}

