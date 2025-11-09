import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: welcomeScreen
    
    color: "#0a0a0a"
    
    signal getStartedClicked()
    signal skipClicked()
    
    ColumnLayout {
        anchors.centerIn: parent
        spacing: 32
        width: Math.min(parent.width * 0.8, 600)
        
        // MilOS Logo with xenon pulsing animation
        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            width: 200
            height: 200
            radius: 100
            color: "#0a0a0a"
            border.color: "#00d4ff"
            border.width: 4
            
            // Xenon pulsing animation (Animation 1: Xenon Glow Pulse)
            SequentialAnimation on opacity {
                running: true
                loops: Animation.Infinite
                NumberAnimation {
                    from: 0.4
                    to: 1.0
                    duration: 2000
                    easing.type: Easing.InOutQuad
                }
                NumberAnimation {
                    from: 1.0
                    to: 0.4
                    duration: 2000
                    easing.type: Easing.InOutQuad
                }
            }
            
            SequentialAnimation on scale {
                running: true
                loops: Animation.Infinite
                NumberAnimation {
                    from: 1.0
                    to: 1.05
                    duration: 2000
                    easing.type: Easing.InOutQuad
                }
                NumberAnimation {
                    from: 1.05
                    to: 1.0
                    duration: 2000
                    easing.type: Easing.InOutQuad
                }
            }
            
            Text {
                anchors.centerIn: parent
                text: "MilOS"
                font.pixelSize: 48
                font.weight: Font.Bold
                color: "#00d4ff"
            }
        }
        
        // Welcome message
        Text {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            text: "Welcome to MilOS"
            font.pixelSize: 32
            font.weight: Font.Bold
            color: "#ffffff"
            horizontalAlignment: Text.AlignHCenter
        }
        
        Text {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            text: "Let's set up your security configuration to keep your system protected."
            font.pixelSize: 16
            color: "#cccccc"
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
        }
        
        // Get Started button
        Button {
            Layout.fillWidth: true
            Layout.preferredHeight: 56
            Layout.alignment: Qt.AlignHCenter
            variant: "primary"
            text: "Get Started"
            onClicked: {
                welcomeScreen.getStartedClicked()
            }
        }
        
        // Skip setup option
        ColumnLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            spacing: 12
            
            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 48
                Layout.alignment: Qt.AlignHCenter
                variant: "secondary"
                text: "Skip Setup"
                onClicked: {
                    welcomeScreen.skipClicked()
                }
            }
            
            NotificationAlert {
                Layout.fillWidth: true
                variant: "warning"
                title: "Not Recommended"
                message: "Skipping security setup may leave your system vulnerable. It is recommended to complete the setup."
                visible: false // Show on skip button hover or click
            }
        }
    }
    
    // Accessibility
    Accessible.role: Accessible.Pane
    Accessible.name: "Welcome to MilOS"
    Accessible.description: "Welcome screen for MilOS security setup"
}

