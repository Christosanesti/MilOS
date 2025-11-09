import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: firewallConfigurationScreen
    
    color: "#0a0a0a"
    
    property var recommendedRules: [
        {name: "Block All Incoming", type: "deny", description: "Block all incoming connections by default", enabled: true},
        {name: "Allow SSH", type: "allow", description: "Allow SSH connections on port 22", enabled: false},
        {name: "Allow HTTP/HTTPS", type: "allow", description: "Allow web traffic on ports 80 and 443", enabled: false},
        {name: "Block ICMP", type: "deny", description: "Block ICMP ping requests", enabled: false}
    ]
    property bool firewallActive: false
    property bool activating: false
    
    signal backClicked()
    signal nextClicked()
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 32
        spacing: 24
        
        // Title
        Text {
            Layout.fillWidth: true
            text: "Firewall Configuration"
            font.pixelSize: 28
            font.weight: Font.Bold
            color: "#ffffff"
        }
        
        Text {
            Layout.fillWidth: true
            text: "Configure firewall rules to protect your system"
            font.pixelSize: 16
            color: "#cccccc"
        }
        
        // Recommended rules
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 300
            variant: "dashboard"
            title: "Recommended Rules"
            
            ScrollView {
                anchors.fill: parent
                anchors.margins: 24
                clip: true
                
                ColumnLayout {
                    width: firewallConfigurationScreen.width - 88
                    spacing: 12
                    
                    Repeater {
                        model: firewallConfigurationScreen.recommendedRules
                        
                        Card {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 100
                            variant: modelData.enabled ? "interactive" : "dashboard"
                            state: modelData.enabled ? "active" : "default"
                            
                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 16
                                spacing: 16
                                
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 4
                                    
                                    Text {
                                        Layout.fillWidth: true
                                        text: modelData.name
                                        font.pixelSize: 16
                                        font.weight: Font.Medium
                                        color: "#ffffff"
                                    }
                                    
                                    Text {
                                        Layout.fillWidth: true
                                        text: modelData.description
                                        font.pixelSize: 12
                                        color: "#b0b0b0"
                                    }
                                }
                                
                                StatusIndicator {
                                    variant: "badge"
                                    status: modelData.enabled ? "success" : "default"
                                    text: modelData.enabled ? "Enabled" : "Disabled"
                                }
                                
                                // Toggle button
                                Rectangle {
                                    width: 48
                                    height: 24
                                    radius: 12
                                    color: modelData.enabled ? "#00cc66" : "#2a2a2a"
                                    border.color: "#00cc66"
                                    border.width: 1
                                    
                                    Rectangle {
                                        anchors.verticalCenter: parent.verticalCenter
                                        x: modelData.enabled ? parent.width - width - 2 : 2
                                        width: 20
                                        height: 20
                                        radius: 10
                                        color: "#ffffff"
                                        
                                        Behavior on x {
                                            NumberAnimation { duration: 200 }
                                        }
                                    }
                                    
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            firewallConfigurationScreen.recommendedRules[index].enabled = !firewallConfigurationScreen.recommendedRules[index].enabled
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // Firewall activation status
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 120
            variant: "dashboard"
            title: "Firewall Status"
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 16
                
                StatusIndicator {
                    variant: "badge"
                    status: firewallConfigurationScreen.firewallActive ? "success" : "error"
                    text: firewallConfigurationScreen.firewallActive ? "Active" : "Inactive"
                }
                
                Text {
                    Layout.fillWidth: true
                    text: firewallConfigurationScreen.firewallActive ? 
                          "Firewall is protecting your system" : 
                          "Firewall is not active. Click 'Activate Firewall' to enable protection."
                    font.pixelSize: 14
                    color: "#cccccc"
                }
                
                Button {
                    Layout.preferredWidth: 150
                    Layout.preferredHeight: 48
                    variant: "primary"
                    text: firewallConfigurationScreen.firewallActive ? "Deactivate" : "Activate Firewall"
                    enabled: !firewallConfigurationScreen.activating
                    onClicked: {
                        if (!firewallConfigurationScreen.firewallActive) {
                            firewallConfigurationScreen.activating = true
                            // Simulate activation
                            Qt.callLater(function() {
                                firewallConfigurationScreen.firewallActive = true
                                firewallConfigurationScreen.activating = false
                            })
                        } else {
                            firewallConfigurationScreen.firewallActive = false
                        }
                    }
                }
            }
        }
        
        // Activation progress
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 12
            visible: firewallConfigurationScreen.activating
            
            ProgressBar {
                Layout.fillWidth: true
                Layout.preferredHeight: 32
                variant: "linear"
                value: 0
                status: "processing"
                indeterminate: true
            }
            
            Text {
                Layout.fillWidth: true
                text: "Activating firewall..."
                font.pixelSize: 14
                color: "#00d4ff"
                horizontalAlignment: Text.AlignHCenter
            }
        }
        
        // Navigation buttons
        RowLayout {
            Layout.fillWidth: true
            spacing: 16
            
            Button {
                Layout.preferredWidth: 120
                Layout.preferredHeight: 48
                variant: "secondary"
                text: "Back"
                onClicked: {
                    firewallConfigurationScreen.backClicked()
                }
            }
            
            Item {
                Layout.fillWidth: true
            }
            
            Button {
                Layout.preferredWidth: 120
                Layout.preferredHeight: 48
                variant: "primary"
                text: "Next"
                onClicked: {
                    firewallConfigurationScreen.nextClicked()
                }
            }
        }
    }
    
    // Accessibility
    Accessible.role: Accessible.Pane
    Accessible.name: "Firewall Configuration"
    Accessible.description: "Configure firewall rules to protect your system"
}

