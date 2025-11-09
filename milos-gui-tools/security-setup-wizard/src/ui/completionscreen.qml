import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: completionScreen
    
    color: "#0a0a0a"
    
    signal finishClicked()
    
    ColumnLayout {
        anchors.centerIn: parent
        spacing: 32
        width: Math.min(parent.width * 0.8, 600)
        
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
            text: "Security Setup Complete!"
            font.pixelSize: 32
            font.weight: Font.Bold
            color: "#00cc66"
            horizontalAlignment: Text.AlignHCenter
        }
        
        Text {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            text: "Your system has been configured with the selected security settings."
            font.pixelSize: 16
            color: "#cccccc"
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
        }
        
        // Setup summary
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 150
            variant: "dashboard"
            title: "Setup Summary"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 8
                
                Text {
                    Layout.fillWidth: true
                    text: "• Security hardening level configured"
                    font.pixelSize: 14
                    color: "#cccccc"
                }
                
                Text {
                    Layout.fillWidth: true
                    text: "• Security preferences saved"
                    font.pixelSize: 14
                    color: "#cccccc"
                }
                
                Text {
                    Layout.fillWidth: true
                    text: "• Security features tour completed"
                    font.pixelSize: 14
                    color: "#cccccc"
                }
            }
        }
        
        // Finish button
        Button {
            Layout.fillWidth: true
            Layout.preferredHeight: 56
            Layout.alignment: Qt.AlignHCenter
            variant: "primary"
            text: "Finish"
            onClicked: {
                completionScreen.finishClicked()
            }
        }
    }
    
    // Accessibility
    Accessible.role: Accessible.Pane
    Accessible.name: "Setup Complete"
    Accessible.description: "Security setup has been completed successfully"
}

