import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: mainWindow
    width: 1400
    height: 900
    visible: true
    title: "MilOS Drone/UAV Control"
    
    color: "#0a0a0a"
    
    TabView {
        anchors.fill: parent
        anchors.margins: 20
        
        Tab {
            title: "Drone Control"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                Text {
                    text: "Drone Control"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffffff"
                }
                
                Button {
                    text: "Connect Drone"
                    onClicked: {
                        // Connect to drone
                    }
                }
                
                Button {
                    text: "Takeoff"
                    onClicked: {
                        // Takeoff command
                    }
                }
                
                Button {
                    text: "Land"
                    onClicked: {
                        // Land command
                    }
                }
            }
        }
        
        Tab {
            title: "Mission Planning"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                Text {
                    text: "Mission Planning"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffffff"
                }
                
                TextField {
                    id: missionNameField
                    Layout.fillWidth: true
                    placeholderText: "Mission Name..."
                    color: "#ffffff"
                    background: Rectangle {
                        color: "#2a2a2a"
                        border.color: "#444444"
                        border.width: 1
                        radius: 4
                    }
                }
                
                Button {
                    text: "Create Mission"
                    onClicked: {
                        // Create mission
                    }
                }
            }
        }
        
        Tab {
            title: "Telemetry"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                Text {
                    text: "Drone Telemetry"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffffff"
                }
            }
        }
    }
}

