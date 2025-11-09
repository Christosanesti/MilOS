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
    property var steps: ["Hardening", "Preferences", "Tour", "Complete"]
    
    // Stack view for wizard screens
    StackView {
        id: stackView
        anchors.fill: parent
        
        initialItem: hardeningWizard
        
        HardeningWizard {
            id: hardeningWizard
            onNextClicked: {
                stackView.push(securityPreferences)
                mainWindow.currentStep = 1
            }
        }
        
        SecurityPreferences {
            id: securityPreferences
            onBackClicked: {
                stackView.pop()
                mainWindow.currentStep = 0
            }
            onNextClicked: {
                stackView.push(securityTour)
                mainWindow.currentStep = 2
            }
        }
        
        SecurityTour {
            id: securityTour
            onBackClicked: {
                stackView.pop()
                mainWindow.currentStep = 1
            }
            onNextClicked: {
                stackView.push(completionScreen)
                mainWindow.currentStep = 3
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

