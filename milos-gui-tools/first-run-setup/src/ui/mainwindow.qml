import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

ApplicationWindow {
    id: mainWindow
    width: 900
    height: 700
    visible: true
    title: "MilOS Security Setup"
    
    color: "#0a0a0a"
    
    // Wizard state
    property int currentStep: 0
    property var steps: ["Welcome", "Security Overview", "Firewall", "Updates", "Hardening", "Preferences", "Tour", "Complete"]
    
    // Skip warning dialog
    Dialog {
        id: skipWarningDialog
        title: "Skip Security Setup?"
        width: 500
        height: 300
        modal: true
        
        Column {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 20
            
            Text {
                width: parent.width
                wrapMode: Text.WordWrap
                text: "Are you sure you want to skip the security setup?\n\n" +
                      "Skipping the setup means:\n" +
                      "• Firewall may not be configured\n" +
                      "• System updates may not be enabled\n" +
                      "• Security hardening may not be applied\n" +
                      "• Your system may be less secure\n\n" +
                      "You can run the setup later from the system settings."
                color: "#ffffff"
                font.pixelSize: 14
            }
            
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 15
                
                Button {
                    text: "Cancel"
                    onClicked: skipWarningDialog.close()
                }
                
                Button {
                    text: "Skip Anyway"
                    onClicked: {
                        skipWarningDialog.close()
                        Qt.quit()
                    }
                }
            }
        }
    }
    
    // Stack view for wizard screens
    StackView {
        id: stackView
        anchors.fill: parent
        
        initialItem: welcomeScreen
        
        WelcomeScreen {
            id: welcomeScreen
            onGetStartedClicked: {
                stackView.push(securityOverview)
                mainWindow.currentStep = 1
            }
            onSkipClicked: {
                skipWarningDialog.open()
            }
        }
        
        SecurityOverview {
            id: securityOverview
            onBackClicked: {
                stackView.pop()
                mainWindow.currentStep = 0
            }
            onNextClicked: {
                stackView.push(firewallConfigurationScreen)
                mainWindow.currentStep = 2
            }
        }
        
        FirewallConfigurationScreen {
            id: firewallConfigurationScreen
            onBackClicked: {
                stackView.pop()
                mainWindow.currentStep = 1
            }
            onNextClicked: {
                stackView.push(updateCheckScreen)
                mainWindow.currentStep = 3
            }
        }
        
        UpdateCheckScreen {
            id: updateCheckScreen
            onBackClicked: {
                stackView.pop()
                mainWindow.currentStep = 2
            }
            onNextClicked: {
                stackView.push(hardeningWizardScreen)
                mainWindow.currentStep = 4
            }
            onSkipClicked: {
                stackView.push(hardeningWizardScreen)
                mainWindow.currentStep = 4
            }
        }
        
        HardeningWizardScreen {
            id: hardeningWizardScreen
            onBackClicked: {
                stackView.pop()
                mainWindow.currentStep = 3
            }
            onNextClicked: {
                stackView.push(securityPreferencesScreen)
                mainWindow.currentStep = 5
            }
        }
        
        SecurityPreferencesScreen {
            id: securityPreferencesScreen
            onBackClicked: {
                stackView.pop()
                mainWindow.currentStep = 4
            }
            onNextClicked: {
                stackView.push(securityTourScreen)
                mainWindow.currentStep = 6
            }
        }
        
        SecurityTourScreen {
            id: securityTourScreen
            onBackClicked: {
                stackView.pop()
                mainWindow.currentStep = 5
            }
            onNextClicked: {
                stackView.push(completionScreen)
                mainWindow.currentStep = 7
            }
        }
        
        CompletionScreen {
            id: completionScreen
            onFinishClicked: {
                Qt.quit()
            }
        }
    }
    
    // Step indicator (bottom of window)
    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 60
        color: "#1a1a1a"
        border.color: "#00d4ff"
        border.width: 1
        
        RowLayout {
            anchors.centerIn: parent
            spacing: 24
            
            Repeater {
                model: mainWindow.steps.length
                
                Row {
                    spacing: 8
                    
                    Rectangle {
                        width: 32
                        height: 32
                        radius: 16
                        color: index <= mainWindow.currentStep ? "#00d4ff" : "#2a2a2a"
                        border.color: "#00d4ff"
                        border.width: 2
                        
                        Text {
                            anchors.centerIn: parent
                            text: index + 1
                            font.pixelSize: 14
                            font.weight: Font.Bold
                            color: index <= mainWindow.currentStep ? "#0a0a0a" : "#ffffff"
                        }
                    }
                    
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: mainWindow.steps[index]
                        font.pixelSize: 12
                        color: index <= mainWindow.currentStep ? "#00d4ff" : "#666666"
                    }
                }
            }
        }
    }
}

