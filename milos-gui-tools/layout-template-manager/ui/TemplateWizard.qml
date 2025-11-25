import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.core 2.1 as PlasmaCore
import MilosComponents 1.0

/**
 * Template Application Wizard
 * 
 * Guides users through applying layout templates with xenon-accented progress indicators
 */
Dialog {
    id: wizard
    
    title: "Apply Layout Template"
    width: 600
    height: 500
    modal: true
    
    property var templateManager: null
    property string selectedTemplateId: ""
    property int currentStep: 0
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 16
        
        // Progress indicator
        ProgressBar {
            id: progressBar
            Layout.fillWidth: true
            from: 0
            to: 3
            value: currentStep
            
            // Xenon-accented progress bar
            background: Rectangle {
                color: "#1a1a1a"
                border.color: "#00d4ff"
                border.width: 1
                radius: 4
            }
            
            contentItem: Item {
                Rectangle {
                    width: progressBar.visualPosition * parent.width
                    height: parent.height
                    color: "#00d4ff"
                    radius: 4
                    
                    // Xenon pulse animation
                    SequentialAnimation on opacity {
                        running: true
                        loops: Animation.Infinite
                        NumberAnimation {
                            from: 0.6
                            to: 1.0
                            duration: 2000
                            easing.type: Easing.InOutSine
                        }
                        NumberAnimation {
                            from: 1.0
                            to: 0.6
                            duration: 2000
                            easing.type: Easing.InOutSine
                        }
                    }
                }
            }
        }
        
        // Step content
        StackLayout {
            id: stepStack
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: currentStep
            
            // Step 1: Template Selection
            ColumnLayout {
                spacing: 16
                
                Text {
                    text: "Select Template"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#ffffff"
                }
                
                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: templateManager ? templateManager.templates : []
                    spacing: 8
                    
                    delegate: Rectangle {
                        width: ListView.view.width
                        height: 80
                        color: mouseArea.containsMouse ? "#2a2a2a" : "#1a1a1a"
                        border.color: selectedTemplateId === modelData.id ? "#00d4ff" : "#2a2a2a"
                        border.width: selectedTemplateId === modelData.id ? 2 : 1
                        radius: 4
                        
                        // Xenon hover glow
                        Rectangle {
                            anchors.fill: parent
                            anchors.margins: -2
                            radius: parent.radius + 2
                            color: "transparent"
                            border.color: "#00d4ff"
                            border.width: 2
                            opacity: mouseArea.containsMouse ? 0.4 : 0
                            
                            Behavior on opacity {
                                NumberAnimation { duration: 200 }
                            }
                        }
                        
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 12
                            spacing: 12
                            
                            Column {
                                Layout.fillWidth: true
                                spacing: 4
                                
                                Text {
                                    text: modelData.name || "Unnamed Template"
                                    color: "#ffffff"
                                    font.pixelSize: 14
                                    font.bold: true
                                }
                                
                                Text {
                                    text: modelData.description || ""
                                    color: "#b0b0b0"
                                    font.pixelSize: 12
                                    elide: Text.ElideRight
                                    width: parent.width
                                }
                                
                                Text {
                                    text: "Monitors: " + (modelData.monitors || modelData.monitor_count || 1)
                                    color: "#808080"
                                    font.pixelSize: 11
                                }
                            }
                        }
                        
                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                selectedTemplateId = modelData.id
                            }
                        }
                    }
                }
            }
            
            // Step 2: Validation
            ColumnLayout {
                spacing: 16
                
                Text {
                    text: "Validating Template"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#ffffff"
                }
                
                Text {
                    id: validationText
                    text: "Checking monitor configuration..."
                    color: "#b0b0b0"
                    Layout.fillWidth: true
                }
                
                Column {
                    id: errorList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 8
                    visible: false
                }
            }
            
            // Step 3: Applying
            ColumnLayout {
                spacing: 16
                
                Text {
                    text: "Applying Template"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#ffffff"
                }
                
                Text {
                    id: applyText
                    text: "Positioning windows..."
                    color: "#b0b0b0"
                    Layout.fillWidth: true
                }
            }
            
            // Step 4: Complete
            ColumnLayout {
                spacing: 16
                
                Text {
                    text: "Template Applied"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#00cc66"
                }
                
                Text {
                    text: "Layout template has been successfully applied."
                    color: "#b0b0b0"
                    Layout.fillWidth: true
                }
            }
        }
        
        // Navigation buttons
        RowLayout {
            Layout.fillWidth: true
            
            Button {
                text: "Cancel"
                onClicked: wizard.close()
                visible: currentStep < 3
            }
            
            Item {
                Layout.fillWidth: true
            }
            
            Button {
                text: currentStep === 0 ? "Next" : (currentStep === 3 ? "Close" : "Next")
                onClicked: {
                    if (currentStep === 0) {
                        if (selectedTemplateId === "") {
                            return; // Cannot proceed without selection
                        }
                        // Validate template
                        currentStep = 1
                        validateTemplate()
                    } else if (currentStep === 1) {
                        // Apply template
                        currentStep = 2
                        applyTemplate()
                    } else if (currentStep === 2) {
                        // Complete
                        currentStep = 3
                    } else {
                        wizard.close()
                    }
                }
            }
        }
    }
    
    function validateTemplate() {
        if (!templateManager) {
            return
        }
        
        // Get monitor configuration (simplified - would use actual KDE API)
        var monitorConfig = {
            "count": 1, // Would detect actual monitor count
            "monitors": []
        }
        
        // Validate using template manager
        var template = templateManager.getTemplate(selectedTemplateId)
        if (template.isEmpty()) {
            validationText.text = "Template not found"
            validationText.color = "#ff4444"
            return
        }
        
        var requiredMonitors = template.monitors || template.monitor_count || 1
        if (monitorConfig.count < requiredMonitors) {
            validationText.text = "Template requires " + requiredMonitors + " monitor(s), but only " + monitorConfig.count + " available"
            validationText.color = "#ff4444"
        } else {
            validationText.text = "Template is compatible with your monitor configuration"
            validationText.color = "#00cc66"
        }
    }
    
    function applyTemplate() {
        if (!templateManager) {
            return
        }
        
        applyText.text = "Applying template..."
        templateManager.applyTemplate(selectedTemplateId)
        
        // Simulate progress
        applyTimer.start()
    }
    
    Timer {
        id: applyTimer
        interval: 1500
        onTriggered: {
            applyText.text = "Template applied successfully"
            applyText.color = "#00cc66"
            currentStep = 3
        }
    }
    
    function open() {
        visible = true
        currentStep = 0
        selectedTemplateId = ""
        if (templateManager) {
            templateManager.loadTemplates()
        }
    }
    
    function close() {
        visible = false
    }
}


