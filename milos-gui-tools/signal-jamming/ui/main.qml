import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtCharts 2.15

ApplicationWindow {
    id: mainWindow
    width: 1280
    height: 800
    visible: true
    title: "MilOS Signal Jamming Interface"
    
    color: "#0a0a0a"
    
    RowLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20
        
        // Control Panel
        ColumnLayout {
            Layout.preferredWidth: 350
            spacing: 15
            
            // Device Status
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
                        text: "Jamming Devices"
                        font.pixelSize: 16
                        font.bold: true
                        color: "#ffffff"
                    }
                    
                    ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        
                        ListView {
                            id: devicesList
                            model: []
                            delegate: Rectangle {
                                width: ListView.view.width
                                height: 60
                                color: "#2a2a2a"
                                radius: 4
                                
                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 10
                                    
                                    Text {
                                        text: modelData.name || "Unknown Device"
                                        font.pixelSize: 12
                                        color: "#ffffff"
                                    }
                                    
                                    Text {
                                        text: modelData.connected ? "Connected" : "Disconnected"
                                        font.pixelSize: 10
                                        color: modelData.connected ? "#44ff44" : "#ff4444"
                                    }
                                }
                            }
                        }
                    }
                }
            }
            
            // Pattern Configuration
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 250
                color: "#1a1a1a"
                border.color: "#444444"
                border.width: 1
                radius: 8
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 15
                    spacing: 10
                    
                    Text {
                        text: "Jamming Pattern"
                        font.pixelSize: 16
                        font.bold: true
                        color: "#ffffff"
                    }
                    
                    TextField {
                        id: patternNameField
                        Layout.fillWidth: true
                        placeholderText: "Pattern name..."
                        color: "#ffffff"
                        background: Rectangle {
                            color: "#2a2a2a"
                            border.color: "#444444"
                            border.width: 1
                            radius: 4
                        }
                    }
                    
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 10
                        
                        TextField {
                            id: freqStartField
                            Layout.fillWidth: true
                            placeholderText: "Start (MHz)"
                            color: "#ffffff"
                            background: Rectangle {
                                color: "#2a2a2a"
                                border.color: "#444444"
                                border.width: 1
                                radius: 4
                            }
                        }
                        
                        Text {
                            text: "-"
                            color: "#ffffff"
                        }
                        
                        TextField {
                            id: freqEndField
                            Layout.fillWidth: true
                            placeholderText: "End (MHz)"
                            color: "#ffffff"
                            background: Rectangle {
                                color: "#2a2a2a"
                                border.color: "#444444"
                                border.width: 1
                                radius: 4
                            }
                        }
                    }
                    
                    TextField {
                        id: powerField
                        Layout.fillWidth: true
                        placeholderText: "Power (dBm)"
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
                        text: "Create Pattern"
                        onClicked: {
                            // Create pattern
                        }
                    }
                }
            }
            
            // Control Buttons
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 10
                
                Button {
                    Layout.fillWidth: true
                    text: "Start Jamming"
                    onClicked: {
                        // Start jamming
                    }
                }
                
                Button {
                    Layout.fillWidth: true
                    text: "Stop Jamming"
                    onClicked: {
                        // Stop jamming
                    }
                }
            }
        }
        
        // Spectrum View
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
                    text: "Spectrum Analysis"
                    font.pixelSize: 16
                    font.bold: true
                    color: "#ffffff"
                }
                
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#0a0a0a"
                    
                    Text {
                        anchors.centerIn: parent
                        text: "Spectrum Chart\n(Integration with QtCharts required)"
                        color: "#aaaaaa"
                        font.pixelSize: 14
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }
    }
}

