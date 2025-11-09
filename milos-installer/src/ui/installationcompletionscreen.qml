import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: installationCompletionScreen
    
    color: "#0a0a0a"
    
    signal rebootClicked()
    
    ColumnLayout {
        anchors.centerIn: parent
        spacing: 32
        width: Math.min(parent.width * 0.8, 700)
        
        // Success icon with celebration animation
        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            width: 120
            height: 120
            radius: 60
            color: "#00cc66"
            
            // Success celebration pulse (Animation 5: Xenon Success Celebration)
            SequentialAnimation on scale {
                running: true
                NumberAnimation {
                    from: 1.0
                    to: 1.2
                    duration: 200
                    easing.type: Easing.EaseOut
                }
                NumberAnimation {
                    from: 1.2
                    to: 1.0
                    duration: 200
                    easing.type: Easing.EaseIn
                }
                NumberAnimation {
                    from: 1.0
                    to: 1.15
                    duration: 200
                    easing.type: Easing.EaseOut
                }
                NumberAnimation {
                    from: 1.15
                    to: 1.0
                    duration: 200
                    easing.type: Easing.EaseIn
                }
                NumberAnimation {
                    from: 1.0
                    to: 1.1
                    duration: 200
                    easing.type: Easing.EaseOut
                }
                NumberAnimation {
                    from: 1.1
                    to: 1.0
                    duration: 300
                    easing.type: Easing.EaseIn
                }
            }
            
            SequentialAnimation on opacity {
                running: true
                NumberAnimation {
                    from: 0.8
                    to: 1.0
                    duration: 200
                    easing.type: Easing.EaseOut
                }
                NumberAnimation {
                    from: 1.0
                    to: 0.9
                    duration: 200
                    easing.type: Easing.EaseIn
                }
                NumberAnimation {
                    from: 0.9
                    to: 1.0
                    duration: 200
                    easing.type: Easing.EaseOut
                }
                NumberAnimation {
                    from: 1.0
                    to: 0.95
                    duration: 200
                    easing.type: Easing.EaseIn
                }
                NumberAnimation {
                    from: 0.95
                    to: 1.0
                    duration: 200
                    easing.type: Easing.EaseOut
                }
                NumberAnimation {
                    from: 1.0
                    to: 0.95
                    duration: 300
                    easing.type: Easing.EaseIn
                }
            }
            
            Text {
                anchors.centerIn: parent
                text: "✓"
                font.pixelSize: 64
                font.weight: Font.Bold
                color: "#0a0a0a"
            }
        }
        
        // Success message
        Text {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            text: "Installation Complete!"
            font.pixelSize: 32
            font.weight: Font.Bold
            color: "#00cc66"
            horizontalAlignment: Text.AlignHCenter
        }
        
        Text {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            text: "MilOS has been successfully installed on your system."
            font.pixelSize: 16
            color: "#cccccc"
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
        }
        
        // Installation summary
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 300
            variant: "dashboard"
            title: "Installation Summary"
            
            ScrollView {
                anchors.fill: parent
                anchors.margins: 24
                clip: true
                
                ColumnLayout {
                    width: installationCompletionScreen.width * 0.8 - 48
                    spacing: 16
                    
                    Repeater {
                        model: [
                            {section: "Partitioning", details: "Disk partitioned successfully"},
                            {section: "Encryption", details: "Full disk encryption configured"},
                            {section: "User Account", details: "User account created"},
                            {section: "Network", details: "Network configured"},
                            {section: "Hardware", details: "Hardware detected and drivers installed"},
                            {section: "Packages", details: "All packages installed successfully"}
                        ]
                        
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 12
                            
                            StatusIndicator {
                                variant: "badge"
                                status: "success"
                                text: "✓"
                            }
                            
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 4
                                
                                Text {
                                    Layout.fillWidth: true
                                    text: modelData.section
                                    font.pixelSize: 14
                                    font.weight: Font.Medium
                                    color: "#ffffff"
                                }
                                
                                Text {
                                    Layout.fillWidth: true
                                    text: modelData.details
                                    font.pixelSize: 12
                                    color: "#b0b0b0"
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // Reboot button
        Button {
            Layout.fillWidth: true
            Layout.preferredHeight: 56
            Layout.alignment: Qt.AlignHCenter
            variant: "primary"
            text: "Reboot System"
            onClicked: {
                installationCompletionScreen.rebootClicked()
            }
        }
    }
    
    // Accessibility
    Accessible.role: Accessible.Pane
    Accessible.name: "Installation Complete"
    Accessible.description: "MilOS installation has been completed successfully"
}

