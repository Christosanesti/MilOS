import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.milos.Components 1.0

Dialog {
    id: dialog
    title: "Validate Theme"
    width: 600
    height: 500
    modal: true
    
    property string themeName: ""
    property var conflictDetector: null
    property var compatibilityDB: null
    
    property bool validating: false
    property var validationResults: null
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 20
        
        // Theme Selection
        GroupBox {
            title: "Select Theme to Validate"
            Layout.fillWidth: true
            
            ComboBox {
                id: themeComboBox
                width: parent.width
                model: compatibilityDB ? compatibilityDB.themes : []
                textRole: "name"
                onCurrentTextChanged: {
                    dialog.themeName = currentText
                }
            }
        }
        
        // Validation Progress
        GroupBox {
            title: "Validation Progress"
            Layout.fillWidth: true
            visible: validating
            
            ColumnLayout {
                anchors.fill: parent
                spacing: 10
                
                ProgressBar {
                    id: progressBar
                    Layout.fillWidth: true
                    indeterminate: true
                }
                
                Text {
                    text: "Validating theme compatibility..."
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
        
        // Validation Results
        GroupBox {
            title: "Validation Results"
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: validationResults !== null && !validating
            
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                
                ColumnLayout {
                    width: parent.width
                    spacing: 10
                    
                    Text {
                        text: validationResults && validationResults.compatible ? 
                              "✓ Theme is compatible" : 
                              "✗ Theme has compatibility issues"
                        font.bold: true
                        font.pixelSize: 16
                        color: validationResults && validationResults.compatible ? "green" : "red"
                        Layout.fillWidth: true
                    }
                    
                    Repeater {
                        model: validationResults ? validationResults.conflicts : []
                        
                        Card {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 100
                            
                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 10
                                spacing: 5
                                
                                Text {
                                    text: modelData.type || "Unknown"
                                    font.bold: true
                                }
                                
                                Text {
                                    text: modelData.description || ""
                                    wrapMode: Text.WordWrap
                                    Layout.fillWidth: true
                                }
                                
                                Text {
                                    text: "Recommendation: " + (modelData.recommendation || "None")
                                    font.pixelSize: 12
                                    color: "#888"
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // Actions
        RowLayout {
            Layout.fillWidth: true
            
            Button {
                text: "Validate"
                enabled: !validating && themeComboBox.currentText
                onClicked: {
                    startValidation()
                }
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: "Close"
                onClicked: dialog.close()
            }
        }
    }
    
    function startValidation() {
        if (!conflictDetector || !themeName) return
        
        validating = true
        validationResults = null
        
        // Get theme info
        var themeInfo = compatibilityDB ? compatibilityDB.getThemeInfo(themeName) : {}
        
        // Detect conflicts via D-Bus service
        if (conflictDetector) {
            var validationResult = conflictDetector.ValidateTheme(themeName)
            var conflicts = validationResult.conflicts || []
            
            var results = {
                compatible: validationResult.compatible || conflicts.length === 0,
                conflicts: []
            }
            
            for (var i = 0; i < conflicts.length; i++) {
                var conflict = conflicts[i]
                results.conflicts.push({
                    type: conflict.type || "Unknown",
                    description: conflict.description || "",
                    recommendation: getRecommendation(conflict.type || "Unknown")
                })
            }
            
            validationResults = results
            validating = false
            
            // Update compatibility database
            if (compatibilityDB) {
                var status = results.compatible ? 0 : 1 // Compatible or Incompatible
                compatibilityDB.updateThemeStatus(themeName, status, results)
            }
        }
    }
    
    function getRecommendation(conflictType) {
        if (conflictType === "XenonRendering") {
            return "Switch to a compatible theme or update graphics drivers"
        } else if (conflictType === "ColorScheme") {
            return "Adjust theme color scheme or use fallback styling"
        } else if (conflictType === "ComponentStyle") {
            return "Theme component styles may conflict with MilOS components"
        }
        return "Review theme configuration"
    }
}

