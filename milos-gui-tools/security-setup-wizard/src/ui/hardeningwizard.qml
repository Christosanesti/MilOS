import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: hardeningWizard
    
    color: "#0a0a0a"
    
    property string selectedLevel: "standard"
    property bool applying: false
    
    signal nextClicked()
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 32
        spacing: 24
        
        // Title
        Text {
            Layout.fillWidth: true
            text: "Security Hardening Level"
            font.pixelSize: 28
            font.weight: Font.Bold
            color: "#ffffff"
        }
        
        Text {
            Layout.fillWidth: true
            text: "Select a security hardening level for your system"
            font.pixelSize: 16
            color: "#cccccc"
        }
        
        // Hardening level cards
        GridLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            columns: 2
            columnSpacing: 16
            rowSpacing: 16
            
            // Basic Level
            Card {
                Layout.fillWidth: true
                Layout.preferredHeight: 200
                variant: hardeningWizard.selectedLevel === "basic" ? "interactive" : "dashboard"
                state: hardeningWizard.selectedLevel === "basic" ? "active" : "default"
                title: "Basic"
                subtitle: "Standard Arch Linux security"
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 24
                    spacing: 8
                    
                    Text {
                        Layout.fillWidth: true
                        text: "• Standard security hardening\n• Basic firewall rules\n• Standard audit logging"
                        font.pixelSize: 14
                        color: "#cccccc"
                        wrapMode: Text.WordWrap
                    }
                }
                
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        hardeningWizard.selectedLevel = "basic"
                    }
                }
            }
            
            // Standard Level
            Card {
                Layout.fillWidth: true
                Layout.preferredHeight: 200
                variant: hardeningWizard.selectedLevel === "standard" ? "interactive" : "dashboard"
                state: hardeningWizard.selectedLevel === "standard" ? "active" : "default"
                title: "Standard"
                subtitle: "Enhanced security (Recommended)"
                
                StatusIndicator {
                    variant: "badge"
                    status: "success"
                    text: "Recommended"
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.margins: 12
                }
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 24
                    spacing: 8
                    
                    Text {
                        Layout.fillWidth: true
                        text: "• Enhanced security hardening\n• Advanced firewall rules\n• Enhanced audit logging\n• Additional security policies"
                        font.pixelSize: 14
                        color: "#cccccc"
                        wrapMode: Text.WordWrap
                    }
                }
                
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        hardeningWizard.selectedLevel = "standard"
                    }
                }
            }
            
            // High Level
            Card {
                Layout.fillWidth: true
                Layout.preferredHeight: 200
                variant: hardeningWizard.selectedLevel === "high" ? "interactive" : "dashboard"
                state: hardeningWizard.selectedLevel === "high" ? "active" : "default"
                title: "High"
                subtitle: "Maximum security with strict policies"
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 24
                    spacing: 8
                    
                    Text {
                        Layout.fillWidth: true
                        text: "• Maximum security hardening\n• Strict firewall rules\n• Comprehensive audit logging\n• Strict security policies\n• Enhanced access controls"
                        font.pixelSize: 14
                        color: "#cccccc"
                        wrapMode: Text.WordWrap
                    }
                }
                
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        hardeningWizard.selectedLevel = "high"
                    }
                }
            }
            
            // Maximum Level
            Card {
                Layout.fillWidth: true
                Layout.preferredHeight: 200
                variant: hardeningWizard.selectedLevel === "maximum" ? "interactive" : "dashboard"
                state: hardeningWizard.selectedLevel === "maximum" ? "active" : "default"
                title: "Maximum"
                subtitle: "Military-grade security"
                
                StatusIndicator {
                    variant: "badge"
                    status: "warning"
                    text: "Advanced"
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.margins: 12
                }
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 24
                    spacing: 8
                    
                    Text {
                        Layout.fillWidth: true
                        text: "• Military-grade security\n• All hardening enabled\n• Strictest policies\n• Maximum audit logging\n• Enhanced access controls\n• May impact usability"
                        font.pixelSize: 14
                        color: "#cccccc"
                        wrapMode: Text.WordWrap
                    }
                }
                
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        hardeningWizard.selectedLevel = "maximum"
                    }
                }
            }
        }
        
        // Progress indicator (when applying)
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 12
            visible: applying
            
            ProgressBar {
                Layout.fillWidth: true
                Layout.preferredHeight: 32
                variant: "linear"
                value: 75
                status: "processing"
            }
            
            Text {
                Layout.fillWidth: true
                text: "Applying security hardening..."
                font.pixelSize: 14
                color: "#00d4ff"
                horizontalAlignment: Text.AlignHCenter
            }
        }
        
        // Navigation buttons
        RowLayout {
            Layout.fillWidth: true
            spacing: 16
            
            Item {
                Layout.fillWidth: true
            }
            
            Button {
                Layout.preferredWidth: 120
                Layout.preferredHeight: 48
                variant: "primary"
                text: "Next"
                enabled: !applying
                onClicked: {
                    if (!applying) {
                        applying = true
                        // Simulate hardening application
                        Qt.callLater(function() {
                            applying = false
                            hardeningWizard.nextClicked()
                        })
                    }
                }
            }
        }
    }
    
    // Accessibility
    Accessible.role: Accessible.Pane
    Accessible.name: "Security Hardening Wizard"
    Accessible.description: "Select security hardening level for your system"
}

