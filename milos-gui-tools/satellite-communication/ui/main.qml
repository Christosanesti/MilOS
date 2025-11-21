import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: mainWindow
    width: 1200
    height: 800
    visible: true
    title: "MilOS Satellite Communication"
    
    color: "#0a0a0a"
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20
        
        Text {
            text: "Satellite Communication"
            font.pixelSize: 24
            font.bold: true
            color: "#ffffff"
        }
        
        // Protocol Selection
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 150
            color: "#1a1a1a"
            border.color: "#444444"
            border.width: 1
            radius: 8
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 10
                
                Text {
                    text: "Satellite Protocol"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#ffffff"
                }
                
                ComboBox {
                    Layout.fillWidth: true
                    model: ["MILSTAR", "AEHF", "WGS", "Iridium", "Inmarsat", "VSAT"]
                    currentIndex: 0
                }
                
                TextField {
                    id: satelliteIdField
                    Layout.fillWidth: true
                    placeholderText: "Satellite ID..."
                    color: "#ffffff"
                    background: Rectangle {
                        color: "#2a2a2a"
                        border.color: "#444444"
                        border.width: 1
                        radius: 4
                    }
                }
                
                Button {
                    text: "Initialize Protocol"
                    onClicked: {
                        // Initialize protocol
                    }
                }
            }
        }
        
        // Bandwidth Optimization
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 100
            color: "#1a1a1a"
            border.color: "#444444"
            border.width: 1
            radius: 8
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 10
                
                Text {
                    text: "Bandwidth Optimization"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#ffffff"
                }
                
                Text {
                    text: "Available Bandwidth: 10.0 Mbps"
                    font.pixelSize: 14
                    color: "#44ff44"
                }
            }
        }
        
        // Message Sending
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            color: "#1a1a1a"
            border.color: "#444444"
            border.width: 1
            radius: 8
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 10
                
                Text {
                    text: "Send Message"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#ffffff"
                }
                
                TextArea {
                    id: messageField
                    Layout.fillWidth: true
                    Layout.preferredHeight: 100
                    placeholderText: "Message (will be optimized for bandwidth)..."
                    color: "#ffffff"
                    background: Rectangle {
                        color: "#2a2a2a"
                        border.color: "#444444"
                        border.width: 1
                        radius: 4
                    }
                }
                
                Button {
                    Layout.fillWidth: true
                    text: "Send Optimized Message"
                    onClicked: {
                        // Send optimized message
                    }
                }
            }
        }
    }
}

