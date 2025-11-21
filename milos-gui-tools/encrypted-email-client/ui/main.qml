import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: mainWindow
    width: 1200
    height: 800
    visible: true
    title: "MilOS Encrypted Email Client"
    
    color: "#0a0a0a"
    
    RowLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20
        
        // Email List
        ColumnLayout {
            Layout.preferredWidth: 400
            Layout.fillHeight: true
            spacing: 15
            
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#1a1a1a"
                border.color: "#444444"
                border.width: 1
                radius: 8
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 15
                    spacing: 10
                    
                    Text {
                        text: "Inbox"
                        font.pixelSize: 18
                        font.bold: true
                        color: "#ffffff"
                    }
                    
                    ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        
                        ListView {
                            id: emailsList
                            model: []
                            delegate: Rectangle {
                                width: ListView.view.width
                                height: 80
                                color: "#2a2a2a"
                                radius: 4
                                
                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 10
                                    
                                    Text {
                                        text: modelData.subject || "No Subject"
                                        font.pixelSize: 14
                                        font.bold: true
                                        color: "#ffffff"
                                    }
                                    
                                    Text {
                                        text: modelData.from || "Unknown"
                                        font.pixelSize: 12
                                        color: "#aaaaaa"
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // Email View/Compose
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 15
            
            // Compose Button
            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 50
                text: "Compose Email"
                onClicked: {
                    // Open compose dialog
                }
            }
            
            // Email Content
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#1a1a1a"
                border.color: "#444444"
                border.width: 1
                radius: 8
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 15
                    spacing: 10
                    
                    Text {
                        text: "Email Content"
                        font.pixelSize: 16
                        font.bold: true
                        color: "#ffffff"
                    }
                    
                    Text {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        text: "Select an email to view"
                        color: "#aaaaaa"
                        wrapMode: Text.WordWrap
                    }
                }
            }
        }
    }
}

