import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.core 2.1 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents

/**
 * Alert Zone Configuration Dialog
 * 
 * Allows users to configure alert filtering and widget settings.
 */
Dialog {
    id: configDialog
    
    title: "Alert Zone Configuration"
    width: 500
    height: 400
    modal: true
    
    property var service: null
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 16
        
        // Alert Filtering
        GroupBox {
            title: "Alert Categories"
            Layout.fillWidth: true
            
            ColumnLayout {
                anchors.fill: parent
                spacing: 12
                
                Repeater {
                    model: ["encryption_failures", "network_breaches", "blocked_transmissions", "hardening_violations"]
                    
                    RowLayout {
                        Layout.fillWidth: true
                        
                        Text {
                            text: modelData.replace("_", " ").replace(/\b\w/g, l => l.toUpperCase())
                            Layout.fillWidth: true
                        }
                        
                        Switch {
                            checked: service && service.enabledCategories.indexOf(modelData) >= 0
                            onCheckedChanged: {
                                if (service) {
                                    var categories = service.enabledCategories
                                    if (checked && categories.indexOf(modelData) < 0) {
                                        categories.push(modelData)
                                    } else if (!checked) {
                                        var index = categories.indexOf(modelData)
                                        if (index >= 0) {
                                            categories.splice(index, 1)
                                        }
                                    }
                                    service.setEnabledCategories(categories)
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // Escalation Settings
        GroupBox {
            title: "Escalation Settings"
            Layout.fillWidth: true
            
            ColumnLayout {
                anchors.fill: parent
                spacing: 12
                
                RowLayout {
                    Layout.fillWidth: true
                    
                    Text {
                        text: "Escalation Timeout (seconds):"
                        Layout.fillWidth: true
                    }
                    
                    SpinBox {
                        id: escalationSpinBox
                        from: 5
                        to: 300
                        value: service ? service.escalationTimeout / 1000 : 30
                        onValueChanged: {
                            if (service) {
                                service.setEscalationTimeout(value * 1000)
                            }
                        }
                    }
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    
                    Text {
                        text: "Acknowledgment Timeout (minutes):"
                        Layout.fillWidth: true
                    }
                    
                    SpinBox {
                        id: acknowledgmentSpinBox
                        from: 1
                        to: 60
                        value: service ? service.acknowledgmentTimeout / 60000 : 5
                        onValueChanged: {
                            if (service) {
                                service.setAcknowledgmentTimeout(value * 60000)
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
                text: "Close"
                onClicked: configDialog.close()
            }
        }
    }
    
    function open() {
        visible = true
    }
    
    function close() {
        visible = false
    }
}

