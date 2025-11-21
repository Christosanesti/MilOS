import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: mainWindow
    width: 1000
    height: 700
    visible: true
    title: "MilOS Biometric Authentication"
    
    color: "#0a0a0a"
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20
        
        Text {
            text: "Biometric Authentication"
            font.pixelSize: 24
            font.bold: true
            color: "#ffffff"
        }
        
        // Enrollment Section
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
                    text: "Enroll User"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#ffffff"
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10
                    
                    TextField {
                        id: enrollUserIdField
                        Layout.fillWidth: true
                        placeholderText: "User ID..."
                        color: "#ffffff"
                        background: Rectangle {
                            color: "#2a2a2a"
                            border.color: "#444444"
                            border.width: 1
                            radius: 4
                        }
                    }
                    
                    ComboBox {
                        id: biometricTypeCombo
                        Layout.preferredWidth: 200
                        model: ["Fingerprint", "Iris", "Voice", "Face"]
                        currentIndex: 0
                    }
                    
                    Button {
                        text: "Enroll"
                        onClicked: {
                            // Enroll user
                        }
                    }
                }
            }
        }
        
        // Authentication Section
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
                    text: "Authenticate User"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#ffffff"
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10
                    
                    TextField {
                        id: authUserIdField
                        Layout.fillWidth: true
                        placeholderText: "User ID..."
                        color: "#ffffff"
                        background: Rectangle {
                            color: "#2a2a2a"
                            border.color: "#444444"
                            border.width: 1
                            radius: 4
                        }
                    }
                    
                    Button {
                        text: "Authenticate"
                        onClicked: {
                            // Authenticate user
                        }
                    }
                }
            }
        }
        
        // MFA Section
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
                    text: "Multi-Factor Authentication"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#ffffff"
                }
                
                Button {
                    Layout.fillWidth: true
                    text: "Authenticate with MFA"
                    onClicked: {
                        // MFA authentication
                    }
                }
            }
        }
    }
}

