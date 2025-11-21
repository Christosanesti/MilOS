import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: mainWindow
    width: 1200
    height: 800
    visible: true
    title: "MilOS Military Radio Integration"
    
    color: "#0a0a0a"
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20
        
        Text {
            text: "Military Radio Communication"
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
                    text: "Radio Protocol"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#ffffff"
                }
                
                ComboBox {
                    Layout.fillWidth: true
                    model: ["SINCGARS", "HAVE_QUICK", "Link16", "VHF", "UHF", "HF"]
                    currentIndex: 0
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    
                    TextField {
                        id: frequencyField
                        Layout.fillWidth: true
                        placeholderText: "Frequency (MHz)..."
                        color: "#ffffff"
                        background: Rectangle {
                            color: "#2a2a2a"
                            border.color: "#444444"
                            border.width: 1
                            radius: 4
                        }
                    }
                    
                    Button {
                        text: "Initialize"
                        onClicked: {
                            // Initialize protocol
                        }
                    }
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
                
                TextField {
                    id: recipientField
                    Layout.fillWidth: true
                    placeholderText: "Recipient..."
                    color: "#ffffff"
                    background: Rectangle {
                        color: "#2a2a2a"
                        border.color: "#444444"
                        border.width: 1
                        radius: 4
                    }
                }
                
                TextArea {
                    id: messageField
                    Layout.fillWidth: true
                    Layout.preferredHeight: 80
                    placeholderText: "Message..."
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
                    text: "Send Encrypted Message"
                    onClicked: {
                        // Send message
                    }
                }
            }
        }
    }
}

