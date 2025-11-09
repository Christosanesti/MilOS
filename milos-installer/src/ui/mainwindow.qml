import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

ApplicationWindow {
    id: mainWindow
    width: 1024
    height: 768
    visible: true
    title: "MilOS Installer"
    
    color: "#0a0a0a"
    
    // Wizard navigation
    property int currentStep: 0
    property int totalSteps: 8
    property var steps: ["Welcome", "Partitioning", "Encryption", "User Account", "Network", "Hardware", "Packages", "Complete"]
    
    // Stack view for wizard screens
    StackView {
        id: stackView
        anchors.fill: parent
        
        initialItem: welcomeScreen
        
        WelcomeScreen {
            id: welcomeScreen
            onInstallClicked: {
                stackView.push(partitioningScreen)
                currentStep = 1
            }
            onLiveModeClicked: {
                // TODO: Launch live mode
                console.log("Live mode clicked")
            }
        }
        
        PartitioningScreen {
            id: partitioningScreen
            onBackClicked: {
                stackView.pop()
                currentStep = 0
            }
            onNextClicked: {
                stackView.push(encryptionSetupScreen)
                currentStep = 2
            }
        }
        
        EncryptionSetupScreen {
            id: encryptionSetupScreen
            onBackClicked: {
                stackView.pop()
                currentStep = 1
            }
            onNextClicked: {
                stackView.push(userAccountScreen)
                currentStep = 3
            }
        }
        
        UserAccountScreen {
            id: userAccountScreen
            onBackClicked: {
                stackView.pop()
                currentStep = 2
            }
            onNextClicked: {
                stackView.push(networkConfigurationScreen)
                currentStep = 4
            }
        }
        
        NetworkConfigurationScreen {
            id: networkConfigurationScreen
            onBackClicked: {
                stackView.pop()
                currentStep = 3
            }
            onNextClicked: {
                stackView.push(hardwareDetectionScreen)
                currentStep = 5
            }
            onSkipClicked: {
                stackView.push(hardwareDetectionScreen)
                currentStep = 5
            }
        }
        
        HardwareDetectionScreen {
            id: hardwareDetectionScreen
            onBackClicked: {
                stackView.pop()
                currentStep = 4
            }
            onNextClicked: {
                stackView.push(packageInstallationScreen)
                currentStep = 6
            }
        }
        
        PackageInstallationScreen {
            id: packageInstallationScreen
            onBackClicked: {
                stackView.pop()
                currentStep = 5
            }
            onNextClicked: {
                stackView.push(installationCompletionScreen)
                currentStep = 7
            }
            onErrorOccurred: function(error) {
                console.log("Installation error:", error)
                // TODO: Show error dialog
            }
        }
        
        InstallationCompletionScreen {
            id: installationCompletionScreen
            onRebootClicked: {
                // TODO: Reboot system
                console.log("Rebooting system...")
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
            
            // Step indicators
            Repeater {
                model: mainWindow.totalSteps
                
                Row {
                    spacing: 8
                    
                    // Step number circle
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
                    
                    // Step label
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

