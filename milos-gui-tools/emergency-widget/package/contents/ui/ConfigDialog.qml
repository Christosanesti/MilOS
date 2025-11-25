import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.core 2.1 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents
import EmergencyWidget 1.0

Dialog {
    id: configDialog
    title: "Emergency Widget Configuration"
    width: 500
    height: 600
    modal: true
    
    property WidgetConfig config: widgetConfig
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 20
        
        // Button Visibility Settings
        GroupBox {
            title: "Button Visibility"
            Layout.fillWidth: true
            
            ColumnLayout {
                anchors.fill: parent
                spacing: 10
                
                CheckBox {
                    text: "Show Network Kill Switch Button"
                    checked: config.showNetworkButton
                    onToggled: config.showNetworkButton = checked
                }
                
                CheckBox {
                    text: "Show Screen Lock Button"
                    checked: config.showLockButton
                    onToggled: config.showLockButton = checked
                }
                
                CheckBox {
                    text: "Show Secure Mode Button"
                    checked: config.showSecureModeButton
                    onToggled: config.showSecureModeButton = checked
                }
                
                CheckBox {
                    text: "Show Emergency Shutdown Button"
                    checked: config.showShutdownButton
                    onToggled: config.showShutdownButton = checked
                }
            }
        }
        
        // Confirmation Settings
        GroupBox {
            title: "Confirmation Settings"
            Layout.fillWidth: true
            
            CheckBox {
                text: "Require Confirmation Dialogs"
                checked: config.requireConfirmations
                onToggled: config.requireConfirmations = checked
            }
        }
        
        // Keyboard Shortcut Settings
        GroupBox {
            title: "Keyboard Shortcut"
            Layout.fillWidth: true
            
            RowLayout {
                anchors.fill: parent
                
                TextField {
                    id: shortcutField
                    text: config.keyboardShortcut
                    placeholderText: "Alt+E"
                    Layout.fillWidth: true
                    onTextChanged: config.keyboardShortcut = text
                }
                
                Button {
                    text: "Register"
                    onClicked: {
                        keyboardShortcut.setShortcut(shortcutField.text)
                        keyboardShortcut.registerShortcut()
                    }
                }
            }
        }
        
        // Status Indicators
        GroupBox {
            title: "Status Indicators"
            Layout.fillWidth: true
            
            ColumnLayout {
                anchors.fill: parent
                spacing: 10
                
                RowLayout {
                    Text {
                        text: "Network Status:"
                        Layout.fillWidth: true
                    }
                    Text {
                        text: statusIndicators.networkStatus
                        color: statusIndicators.networkActive ? "#00cc66" : "#ff4444"
                    }
                }
                
                RowLayout {
                    Text {
                        text: "Secure Mode Status:"
                        Layout.fillWidth: true
                    }
                    Text {
                        text: statusIndicators.secureModeStatus
                        color: statusIndicators.secureModeActive ? "#00cc66" : "#ff4444"
                    }
                }
                
                Button {
                    text: "Refresh Status"
                    onClicked: statusIndicators.updateAll()
                }
            }
        }
        
        // Buttons
        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight
            
            Button {
                text: "Cancel"
                onClicked: configDialog.close()
            }
            
            Button {
                text: "Save"
                onClicked: {
                    config.saveConfig()
                    configDialog.close()
                }
            }
        }
    }
}

