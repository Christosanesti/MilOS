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
        
        // Success icon with xenon green celebration animation
        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            width: 150
            height: 150
            radius: 75
            color: "#0a0a0a"
            border.color: "#00cc66"
            border.width: 4
            
            // Xenon green celebration animation (Animation 5: Xenon Success Celebration)
            SequentialAnimation on scale {
                running: true
                loops: 3
                NumberAnimation {
                    from: 1.0
                    to: 1.2
                    duration: 200
                    easing.type: Easing.OutQuad
                }
                NumberAnimation {
                    from: 1.2
                    to: 1.0
                    duration: 200
                    easing.type: Easing.InQuad
                }
            }
            
            SequentialAnimation on opacity {
                running: true
                loops: 3
                NumberAnimation {
                    from: 0.6
                    to: 1.0
                    duration: 200
                    easing.type: Easing.OutQuad
                }
                NumberAnimation {
                    from: 1.0
                    to: 0.6
                    duration: 200
                    easing.type: Easing.InQuad
                }
            }
            
            Text {
                anchors.centerIn: parent
                text: "✓"
                font.pixelSize: 80
                font.weight: Font.Bold
                color: "#00cc66"
            }
        }
        
        // Success message
        Text {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            text: "Security Setup Complete!"
            font.pixelSize: 32
            font.weight: Font.Bold
            color: "#ffffff"
            horizontalAlignment: Text.AlignHCenter
        }
        
        Text {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            text: "Your MilOS system has been configured with the security settings you selected. Your system is now protected and ready to use."
            font.pixelSize: 16
            color: "#cccccc"
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
        }
        
        // Setup summary
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            variant: "dashboard"
            title: "Setup Summary"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 12
                
                Text {
                    Layout.fillWidth: true
                    text: "✓ Firewall configured"
                    font.pixelSize: 14
                    color: "#00cc66"
                }
                
                Text {
                    Layout.fillWidth: true
                    text: "✓ System updates checked"
                    font.pixelSize: 14
                    color: "#00cc66"
                }
                
                Text {
                    Layout.fillWidth: true
                    text: "✓ Security hardening applied"
                    font.pixelSize: 14
                    color: "#00cc66"
                }
                
                Text {
                    Layout.fillWidth: true
                    text: "✓ Security preferences configured"
                    font.pixelSize: 14
                    color: "#00cc66"
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
    Accessible.name: "Security Setup Complete"
    Accessible.description: "Security setup has been completed successfully"
}

