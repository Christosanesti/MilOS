import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: mainWindow
    width: 1400
    height: 900
    visible: true
    title: "MilOS Secure Development Environment"
    
    color: "#0a0a0a"
    
    TabView {
        anchors.fill: parent
        anchors.margins: 20
        
        Tab {
            title: "Code Analysis"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                Text {
                    text: "Security Code Analysis"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffffff"
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    
                    TextField {
                        id: filePathField
                        Layout.fillWidth: true
                        placeholderText: "File or directory path..."
                        color: "#ffffff"
                        background: Rectangle {
                            color: "#2a2a2a"
                            border.color: "#444444"
                            border.width: 1
                            radius: 4
                        }
                    }
                    
                    Button {
                        text: "Analyze"
                        onClicked: {
                            // Analyze code
                        }
                    }
                }
                
                Text {
                    text: "Security Issues: 0"
                    font.pixelSize: 14
                    color: "#44ff44"
                }
            }
        }
        
        Tab {
            title: "Supply Chain"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                Text {
                    text: "Supply Chain Security"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffffff"
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    
                    TextField {
                        id: projectPathField
                        Layout.fillWidth: true
                        placeholderText: "Project path..."
                        color: "#ffffff"
                        background: Rectangle {
                            color: "#2a2a2a"
                            border.color: "#444444"
                            border.width: 1
                            radius: 4
                        }
                    }
                    
                    Button {
                        text: "Scan Dependencies"
                        onClicked: {
                            // Scan dependencies
                        }
                    }
                }
                
                Text {
                    text: "Vulnerable Dependencies: 0"
                    font.pixelSize: 14
                    color: "#ff4444"
                }
            }
        }
        
        Tab {
            title: "Security Statistics"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                Text {
                    text: "Security Statistics"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffffff"
                }
                
                Text {
                    text: "Total Issues: 0"
                    font.pixelSize: 14
                    color: "#ffffff"
                }
                
                Text {
                    text: "Critical: 0"
                    font.pixelSize: 14
                    color: "#ff4444"
                }
                
                Text {
                    text: "High: 0"
                    font.pixelSize: 14
                    color: "#ff8844"
                }
            }
        }
    }
}





