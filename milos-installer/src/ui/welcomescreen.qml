import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: welcomeScreen
    
    color: "#0a0a0a"
    
    signal installClicked()
    signal liveModeClicked()
    
    ColumnLayout {
        anchors.centerIn: parent
        spacing: 32
        width: Math.min(parent.width * 0.8, 600)
        
        // MilOS Logo with pulsing animation
        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            width: 200
            height: 200
            radius: 100
            color: "transparent"
            border.color: "#00d4ff"
            border.width: 3
            
            // Pulsing animation (Animation 1: Xenon Glow Pulse)
            SequentialAnimation on opacity {
                running: true
                loops: Animation.Infinite
                NumberAnimation {
                    from: 0.6
                    to: 1.0
                    duration: 1000
                    easing.type: Easing.InOutSine
                }
                NumberAnimation {
                    from: 1.0
                    to: 0.6
                    duration: 1000
                    easing.type: Easing.InOutSine
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
        
        // Welcome text
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
            text: "A hardened Linux distribution for security professionals"
            font.pixelSize: 16
            color: "#cccccc"
            horizontalAlignment: Text.AlignHCenter
        }
        
        // Language selection
        DropdownMenu {
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            variant: "standard"
            placeholder: "Select Language"
            items: [
                {text: "English", value: "en"},
                {text: "Español", value: "es"},
                {text: "Français", value: "fr"},
                {text: "Deutsch", value: "de"}
            ]
        }
        
        // Buttons
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 16
            
            // Install MilOS button (Primary)
            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 56
                variant: "primary"
                text: "Install MilOS"
                onClicked: {
                    welcomeScreen.installClicked()
                }
            }
            
            // Try Live Mode button (Secondary)
            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 48
                variant: "secondary"
                text: "Try Live Mode"
                onClicked: {
                    welcomeScreen.liveModeClicked()
                }
            }
        }
    }
    
    // Accessibility
    Accessible.role: Accessible.Pane
    Accessible.name: "Welcome Screen"
    Accessible.description: "Welcome to MilOS installation wizard"
}

