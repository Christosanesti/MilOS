import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: mainWindow
    width: 1280
    height: 800
    visible: true
    title: "MilOS Threat Intelligence Feed Integration"
    
    color: "#0a0a0a"
    
    RowLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20
        
        // Feed Panel
        ColumnLayout {
            Layout.preferredWidth: 350
            spacing: 15
            
            // Feed Status
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 120
                color: "#1a1a1a"
                border.color: "#444444"
                border.width: 1
                radius: 8
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 15
                    spacing: 10
                    
                    Text {
                        text: "Threat Feeds"
                        font.pixelSize: 16
                        font.bold: true
                        color: "#ffffff"
                    }
                    
                    Text {
                        text: "Active Feeds: 2"
                        font.pixelSize: 14
                        color: "#44ff44"
                    }
                }
            }
            
            // Feed List
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
                        text: "Feed Sources"
                        font.pixelSize: 14
                        font.bold: true
                        color: "#ffffff"
                    }
                    
                    ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        
                        ListView {
                            id: feedsList
                            model: [
                                {"name": "Military Threat Feed", "enabled": true, "indicators": 45},
                                {"name": "Public Threat Feed", "enabled": true, "indicators": 128}
                            ]
                            delegate: Rectangle {
                                width: ListView.view.width
                                height: 70
                                color: "#2a2a2a"
                                radius: 4
                                
                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 10
                                    
                                    Text {
                                        text: modelData.name || "Unknown Feed"
                                        font.pixelSize: 12
                                        font.bold: true
                                        color: "#ffffff"
                                    }
                                    
                                    RowLayout {
                                        Text {
                                            text: modelData.enabled ? "Enabled" : "Disabled"
                                            font.pixelSize: 10
                                            color: modelData.enabled ? "#44ff44" : "#ff4444"
                                        }
                                        
                                        Text {
                                            text: "• " + modelData.indicators + " indicators"
                                            font.pixelSize: 10
                                            color: "#aaaaaa"
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            
            // Auto-Blocking Control
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 80
                color: "#1a1a1a"
                border.color: "#444444"
                border.width: 1
                radius: 8
                
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 15
                    
                    Text {
                        text: "Auto-Blocking:"
                        font.pixelSize: 14
                        color: "#ffffff"
                    }
                    
                    Switch {
                        checked: threatBlocker.autoBlockingEnabled
                        onToggled: threatBlocker.autoBlockingEnabled = checked
                    }
                }
            }
        }
        
        // Threat Indicators Panel
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
                    text: "Threat Indicators"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#ffffff"
                }
                
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    
                    ListView {
                        id: indicatorsList
                        model: []
                        delegate: Rectangle {
                            width: ListView.view.width
                            height: 80
                            color: "#2a2a2a"
                            radius: 4
                            
                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 10
                                spacing: 15
                                
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 5
                                    
                                    Text {
                                        text: modelData.value || "Unknown"
                                        font.pixelSize: 14
                                        font.bold: true
                                        color: "#ffffff"
                                    }
                                    
                                    Text {
                                        text: modelData.description || "No description"
                                        font.pixelSize: 12
                                        color: "#aaaaaa"
                                        wrapMode: Text.WordWrap
                                    }
                                }
                                
                                Rectangle {
                                    Layout.preferredWidth: 80
                                    Layout.preferredHeight: 30
                                    color: {
                                        if (modelData.severity === "critical") return "#ff4444"
                                        if (modelData.severity === "high") return "#ff8844"
                                        if (modelData.severity === "medium") return "#ffff44"
                                        return "#44ff44"
                                    }
                                    radius: 4
                                    
                                    Text {
                                        anchors.centerIn: parent
                                        text: modelData.severity || "low"
                                        font.pixelSize: 10
                                        font.bold: true
                                        color: "#000000"
                                    }
                                }
                                
                                Button {
                                    text: "Block"
                                    onClicked: {
                                        // Block indicator
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

