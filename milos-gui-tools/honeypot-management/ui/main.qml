import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: mainWindow
    width: 1400
    height: 900
    visible: true
    title: "MilOS Honeypot Management"
    
    color: "#0a0a0a"
    
    TabView {
        anchors.fill: parent
        anchors.margins: 20
        
        Tab {
            title: "Deploy Honeypot"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                Text {
                    text: "Deploy New Honeypot"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffffff"
                }
                
                TextField {
                    id: honeypotNameField
                    Layout.fillWidth: true
                    placeholderText: "Honeypot Name..."
                    color: "#ffffff"
                    background: Rectangle {
                        color: "#2a2a2a"
                        border.color: "#444444"
                        border.width: 1
                        radius: 4
                    }
                }
                
                ComboBox {
                    id: honeypotTypeCombo
                    Layout.fillWidth: true
                    model: ["Web Server", "SSH", "FTP", "Database", "Email"]
                    currentIndex: 0
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    
                    TextField {
                        id: hostField
                        Layout.fillWidth: true
                        placeholderText: "Host..."
                        color: "#ffffff"
                        background: Rectangle {
                            color: "#2a2a2a"
                            border.color: "#444444"
                            border.width: 1
                            radius: 4
                        }
                    }
                    
                    TextField {
                        id: portField
                        Layout.preferredWidth: 100
                        placeholderText: "Port..."
                        color: "#ffffff"
                        background: Rectangle {
                            color: "#2a2a2a"
                            border.color: "#444444"
                            border.width: 1
                            radius: 4
                        }
                    }
                }
                
                Button {
                    Layout.fillWidth: true
                    text: "Deploy Honeypot"
                    onClicked: {
                        // Deploy honeypot
                    }
                }
            }
        }
        
        Tab {
            title: "Active Honeypots"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                Text {
                    text: "Active Honeypots"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffffff"
                }
                
                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: []  // Would be populated from honeypot manager
                    delegate: Rectangle {
                        width: parent.width
                        height: 60
                        color: "#1a1a1a"
                        border.color: "#444444"
                        border.width: 1
                        radius: 4
                        
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 10
                            
                            Text {
                                text: "Honeypot " + index
                                color: "#ffffff"
                            }
                            
                            Item { Layout.fillWidth: true }
                            
                            Button {
                                text: "Start"
                                onClicked: {
                                    // Start honeypot
                                }
                            }
                            
                            Button {
                                text: "Stop"
                                onClicked: {
                                    // Stop honeypot
                                }
                            }
                        }
                    }
                }
            }
        }
        
        Tab {
            title: "Threat Intelligence"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                Text {
                    text: "Threat Intelligence"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffffff"
                }
                
                Text {
                    text: "Total Threats Detected: 0"
                    font.pixelSize: 14
                    color: "#44ff44"
                }
            }
        }
    }
}






