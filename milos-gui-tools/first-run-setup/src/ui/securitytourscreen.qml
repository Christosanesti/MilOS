import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: securityTourScreen
    
    color: "#0a0a0a"
    
    property int currentStep: 0
    property var tourSteps: [
        {
            title: "Welcome to MilOS Security",
            description: "MilOS provides comprehensive security features to protect your system and data.",
            feature: "Security Dashboard"
        },
        {
            title: "Encryption Manager",
            description: "Encrypt and decrypt files with quantum-resistant algorithms. Supports batch operations for multiple files.",
            feature: "File Encryption"
        },
        {
            title: "Network Dashboard",
            description: "Monitor network traffic, manage firewall rules, and detect security threats in real-time.",
            feature: "Network Security"
        },
        {
            title: "Emergency Actions",
            description: "Quick access to emergency security actions like network kill switch and screen lock.",
            feature: "Emergency Controls"
        }
    ]
    
    signal backClicked()
    signal nextClicked()
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 32
        spacing: 24
        
        // Title
        Text {
            Layout.fillWidth: true
            text: "Security Features Tour"
            font.pixelSize: 28
            font.weight: Font.Bold
            color: "#ffffff"
        }
        
        // Tour step card
        Card {
            Layout.fillWidth: true
            Layout.fillHeight: true
            variant: "dashboard"
            title: securityTourScreen.tourSteps[securityTourScreen.currentStep].title
            subtitle: securityTourScreen.tourSteps[securityTourScreen.currentStep].feature
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 16
                
                Text {
                    Layout.fillWidth: true
                    text: securityTourScreen.tourSteps[securityTourScreen.currentStep].description
                    font.pixelSize: 16
                    color: "#cccccc"
                    wrapMode: Text.WordWrap
                }
                
                // Step indicator
                RowLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 8
                    
                    Repeater {
                        model: securityTourScreen.tourSteps.length
                        
                        Rectangle {
                            width: 12
                            height: 12
                            radius: 6
                            color: index === securityTourScreen.currentStep ? "#00d4ff" : "#2a2a2a"
                            border.color: "#00d4ff"
                            border.width: 1
                        }
                    }
                }
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
                    if (securityTourScreen.currentStep > 0) {
                        securityTourScreen.currentStep--
                    } else {
                        securityTourScreen.backClicked()
                    }
                }
            }
            
            Item {
                Layout.fillWidth: true
            }
            
            Button {
                Layout.preferredWidth: 120
                Layout.preferredHeight: 48
                variant: "primary"
                text: securityTourScreen.currentStep < securityTourScreen.tourSteps.length - 1 ? "Next" : "Continue"
                onClicked: {
                    if (securityTourScreen.currentStep < securityTourScreen.tourSteps.length - 1) {
                        securityTourScreen.currentStep++
                    } else {
                        securityTourScreen.nextClicked()
                    }
                }
            }
        }
    }
    
    // Accessibility
    Accessible.role: Accessible.Pane
    Accessible.name: "Security Features Tour"
    Accessible.description: "Learn about MilOS security features"
}

