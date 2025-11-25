import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.milos.Components 1.0

Dialog {
    id: dialog
    title: "Performance Mode Settings"
    width: 600
    height: 500
    modal: true
    
    property var controlManager
    property var indicatorsManager
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 20
        
        // Current Mode Selection
        GroupBox {
            title: "Performance Mode"
            Layout.fillWidth: true
            
            ColumnLayout {
                anchors.fill: parent
                spacing: 10
                
                ComboBox {
                    id: modeComboBox
                    Layout.fillWidth: true
                    model: controlManager ? controlManager.availableModes : []
                    currentIndex: {
                        if (!controlManager) return 0
                        var modes = controlManager.availableModes
                        var current = controlManager.currentMode
                        for (var i = 0; i < modes.length; i++) {
                            if (modes[i] === current) return i
                        }
                        return 0
                    }
                    onCurrentTextChanged: {
                        if (controlManager) {
                            controlManager.setCurrentMode(currentText)
                        }
                    }
                }
                
                Text {
                    text: {
                        if (!controlManager) return ""
                        var mode = controlManager.currentMode
                        if (mode === "standard") return "Full xenon effects, all animations, particle effects"
                        if (mode === "performance") return "Reduced xenon effects, simplified animations, no particles"
                        if (mode === "low-power") return "Minimal xenon effects, minimal animations, no particles"
                        return ""
                    }
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }
        }
        
        // Override Settings
        GroupBox {
            title: "Override Settings"
            Layout.fillWidth: true
            
            ColumnLayout {
                anchors.fill: parent
                spacing: 10
                
                CheckBox {
                    id: overrideCheckBox
                    text: "Enable Performance Mode Override"
                    checked: controlManager ? controlManager.overrideEnabled : false
                    onCheckedChanged: {
                        if (controlManager) {
                            controlManager.setOverrideEnabled(checked)
                        }
                    }
                }
                
                ComboBox {
                    id: overrideComboBox
                    enabled: overrideCheckBox.checked
                    Layout.fillWidth: true
                    model: controlManager ? controlManager.availableModes : []
                    currentIndex: {
                        if (!controlManager || !overrideCheckBox.checked) return 0
                        var modes = controlManager.availableModes
                        var current = controlManager.overrideMode
                        for (var i = 0; i < modes.length; i++) {
                            if (modes[i] === current) return i
                        }
                        return 0
                    }
                    onCurrentTextChanged: {
                        if (controlManager && overrideCheckBox.checked) {
                            controlManager.setOverrideMode(currentText)
                        }
                    }
                }
            }
        }
        
        // Performance Status
        GroupBox {
            title: "Performance Status"
            Layout.fillWidth: true
            visible: indicatorsManager ? indicatorsManager.indicatorVisible : false
            
            ColumnLayout {
                anchors.fill: parent
                spacing: 10
                
                GridLayout {
                    columns: 2
                    Layout.fillWidth: true
                    
                    Text { text: "Current Mode:" }
                    Text { 
                        text: indicatorsManager ? indicatorsManager.currentMode : ""
                        font.bold: true
                    }
                    
                    Text { text: "Frame Rate:" }
                    Text { 
                        text: indicatorsManager ? indicatorsManager.frameRate.toFixed(1) + " fps" : ""
                    }
                    
                    Text { text: "CPU Usage:" }
                    Text { 
                        text: indicatorsManager ? indicatorsManager.cpuUsage.toFixed(1) + "%" : ""
                    }
                    
                    Text { text: "GPU Usage:" }
                    Text { 
                        text: indicatorsManager ? indicatorsManager.gpuUsage.toFixed(1) + "%" : ""
                    }
                    
                    Text { text: "Suggested Mode:" }
                    Text { 
                        text: indicatorsManager ? indicatorsManager.suggestedMode : ""
                        font.bold: true
                        color: indicatorsManager && indicatorsManager.suggestedMode !== indicatorsManager.currentMode ? "orange" : "green"
                    }
                }
                
                Button {
                    text: "Apply Suggested Mode"
                    enabled: indicatorsManager && indicatorsManager.suggestedMode !== indicatorsManager.currentMode
                    onClicked: {
                        if (controlManager && indicatorsManager) {
                            controlManager.setCurrentMode(indicatorsManager.suggestedMode)
                        }
                    }
                }
            }
        }
        
        // Application Overrides
        GroupBox {
            title: "Application Overrides"
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                
                ListView {
                    id: overrideList
                    model: controlManager ? Object.keys(controlManager.applicationOverrides) : []
                    
                    delegate: RowLayout {
                        width: overrideList.width
                        spacing: 10
                        
                        Text {
                            text: modelData
                            Layout.fillWidth: true
                        }
                        
                        ComboBox {
                            model: controlManager ? controlManager.availableModes : []
                            currentIndex: {
                                if (!controlManager) return 0
                                var modes = controlManager.availableModes
                                var appMode = controlManager.getApplicationMode(modelData)
                                for (var i = 0; i < modes.length; i++) {
                                    if (modes[i] === appMode) return i
                                }
                                return 0
                            }
                            onCurrentTextChanged: {
                                if (controlManager) {
                                    controlManager.setApplicationMode(modelData, currentText)
                                }
                            }
                        }
                        
                        Button {
                            text: "Remove"
                            onClicked: {
                                if (controlManager) {
                                    controlManager.removeApplicationOverride(modelData)
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // Buttons
        RowLayout {
            Layout.fillWidth: true
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: "Close"
                onClicked: dialog.close()
            }
        }
    }
}

