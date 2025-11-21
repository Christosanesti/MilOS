import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: mainWindow
    width: 1400
    height: 900
    visible: true
    title: "MilOS AI-Powered Continuous Security Monitoring"
    
    color: "#0a0a0a"
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20
        
        // Status Bar
        RowLayout {
            Layout.fillWidth: true
            spacing: 20
            
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 80
                color: "#1a1a1a"
                border.color: threatDetector.isMonitoring ? "#44ff44" : "#ff4444"
                border.width: 2
                radius: 8
                
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 15
                    spacing: 15
                    
                    Text {
                        text: "Monitoring Status:"
                        font.pixelSize: 16
                        color: "#ffffff"
                    }
                    
                    Text {
                        text: threatDetector.isMonitoring ? "ACTIVE" : "INACTIVE"
                        font.pixelSize: 20
                        font.bold: true
                        color: threatDetector.isMonitoring ? "#44ff44" : "#ff4444"
                    }
                }
            }
            
            Button {
                text: threatDetector.isMonitoring ? "Stop Monitoring" : "Start Monitoring"
                onClicked: {
                    if (threatDetector.isMonitoring) {
                        aiMonitoringSystem.stop()
                    } else {
                        aiMonitoringSystem.start()
                    }
                }
            }
        }
        
        // Main Content
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 20
            
            // Threats Panel
            ColumnLayout {
                Layout.preferredWidth: 500
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
                            text: "Detected Threats"
                            font.pixelSize: 18
                            font.bold: true
                            color: "#ffffff"
                        }
                        
                        ScrollView {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            
                            ListView {
                                id: threatsList
                                model: []
                                delegate: Rectangle {
                                    width: ListView.view.width
                                    height: 100
                                    color: "#2a2a2a"
                                    radius: 4
                                    
                                    ColumnLayout {
                                        anchors.fill: parent
                                        anchors.margins: 10
                                        spacing: 5
                                        
                                        Text {
                                            text: modelData.description || "Unknown Threat"
                                            font.pixelSize: 14
                                            font.bold: true
                                            color: "#ffffff"
                                            wrapMode: Text.WordWrap
                                        }
                                        
                                        RowLayout {
                                            Text {
                                                text: "Severity: " + (modelData.severity || "Low")
                                                font.pixelSize: 12
                                                color: "#aaaaaa"
                                            }
                                            
                                            Button {
                                                text: "Contain"
                                                onClicked: {
                                                    // Contain threat
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
            
            // System Metrics Panel
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 15
                
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
                            text: "System Metrics"
                            font.pixelSize: 18
                            font.bold: true
                            color: "#ffffff"
                        }
                        
                        GridLayout {
                            Layout.fillWidth: true
                            columns: 2
                            rowSpacing: 10
                            columnSpacing: 20
                            
                            Text {
                                text: "CPU Usage:"
                                color: "#aaaaaa"
                            }
                            Text {
                                text: (systemMonitor.systemMetrics.cpu_usage || 0) + "%"
                                color: "#ffffff"
                            }
                            
                            Text {
                                text: "Memory Usage:"
                                color: "#aaaaaa"
                            }
                            Text {
                                text: (systemMonitor.systemMetrics.memory_percent || 0) + "%"
                                color: "#ffffff"
                            }
                            
                            Text {
                                text: "Disk Usage:"
                                color: "#aaaaaa"
                            }
                            Text {
                                text: (systemMonitor.systemMetrics.disk_percent || 0) + "%"
                                color: "#ffffff"
                            }
                        }
                    }
                }
                
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
                            text: "Processes & Network"
                            font.pixelSize: 18
                            font.bold: true
                            color: "#ffffff"
                        }
                        
                        TabView {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            
                            Tab {
                                title: "Processes"
                                // Process list would go here
                            }
                            
                            Tab {
                                title: "Network"
                                // Network connections would go here
                            }
                        }
                    }
                }
            }
        }
    }
}

