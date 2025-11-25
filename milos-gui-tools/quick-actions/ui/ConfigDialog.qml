import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.core 2.1 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents

/**
 * Quick Actions Configuration Dialog
 * 
 * Allows users to configure floating action buttons:
 * - Button position
 * - Auto-hide behavior
 * - Button size
 * - Per-tool enable/disable
 * - Action priority settings
 */
Dialog {
    id: configDialog
    
    title: "Quick Actions Configuration"
    width: 600
    height: 500
    modal: true
    
    property var config: ({})
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 16
        
        // General Settings
        GroupBox {
            title: "General Settings"
            Layout.fillWidth: true
            
            ColumnLayout {
                anchors.fill: parent
                spacing: 12
                
                // Enable/Disable
                RowLayout {
                    Layout.fillWidth: true
                    
                    Text {
                        text: "Enable Quick Actions:"
                        Layout.fillWidth: true
                    }
                    
                    Switch {
                        id: enableSwitch
                        checked: config.enabled !== false
                        onCheckedChanged: {
                            config.enabled = checked
                        }
                    }
                }
                
                // Button Position
                RowLayout {
                    Layout.fillWidth: true
                    
                    Text {
                        text: "Button Position:"
                        Layout.fillWidth: true
                    }
                    
                    ComboBox {
                        id: positionCombo
                        model: ["bottom-right", "bottom-left", "top-right", "top-left"]
                        currentIndex: model.indexOf(config.position || "bottom-right")
                        onCurrentIndexChanged: {
                            config.position = model[currentIndex]
                        }
                    }
                }
                
                // Auto-Hide
                RowLayout {
                    Layout.fillWidth: true
                    
                    Text {
                        text: "Auto-Hide When Inactive:"
                        Layout.fillWidth: true
                    }
                    
                    Switch {
                        id: autoHideSwitch
                        checked: config.autoHide || false
                        onCheckedChanged: {
                            config.autoHide = checked
                        }
                    }
                }
                
                // Button Size
                RowLayout {
                    Layout.fillWidth: true
                    
                    Text {
                        text: "Button Size:"
                        Layout.fillWidth: true
                    }
                    
                    SpinBox {
                        id: sizeSpinBox
                        from: 40
                        to: 80
                        value: config.buttonSize || 56
                        onValueChanged: {
                            config.buttonSize = value
                        }
                    }
                }
            }
        }
        
        // Tool-Specific Settings
        GroupBox {
            title: "Tool-Specific Settings"
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                
                ColumnLayout {
                    width: parent.width
                    spacing: 8
                    
                    Repeater {
                        model: ["encryption_manager", "network_dashboard", "anonymity_controller", 
                               "backup_system", "clipboard_manager"]
                        
                        RowLayout {
                            Layout.fillWidth: true
                            
                            Text {
                                text: modelData.replace("_", " ").replace(/\b\w/g, l => l.toUpperCase())
                                Layout.fillWidth: true
                            }
                            
                            Switch {
                                checked: (config.toolEnabled && config.toolEnabled[modelData]) !== false
                                onCheckedChanged: {
                                    if (!config.toolEnabled) {
                                        config.toolEnabled = {}
                                    }
                                    config.toolEnabled[modelData] = checked
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
            
            Item {
                Layout.fillWidth: true
            }
            
            PlasmaComponents.Button {
                text: "Cancel"
                onClicked: configDialog.reject()
            }
            
            PlasmaComponents.Button {
                text: "Apply"
                onClicked: {
                    saveConfig()
                    configDialog.accept()
                }
            }
        }
    }
    
    function saveConfig() {
        // Save configuration to file
        // This would typically call a C++ method to save to YAML
        console.log("Saving config:", JSON.stringify(config))
    }
    
    function loadConfig() {
        // Load configuration from file
        // This would typically call a C++ method to load from YAML
        config = {
            enabled: true,
            position: "bottom-right",
            autoHide: false,
            buttonSize: 56,
            toolEnabled: {}
        }
    }
    
    Component.onCompleted: {
        loadConfig()
    }
}

