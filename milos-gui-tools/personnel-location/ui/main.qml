import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtLocation 5.15
import QtPositioning 5.15

ApplicationWindow {
    id: mainWindow
    width: 1024
    height: 768
    visible: true
    title: "MilOS Personnel Location Mapping"
    
    color: "#0a0a0a"
    
    RowLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20
        
        // Control Panel
        ColumnLayout {
            Layout.preferredWidth: 300
            spacing: 15
            
            // Tracking Control
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
                        text: "Tracking Status"
                        font.pixelSize: 16
                        font.bold: true
                        color: "#ffffff"
                    }
                    
                    Text {
                        text: locationTracker.trackingEnabled ? "ENABLED" : "DISABLED"
                        font.pixelSize: 18
                        font.bold: true
                        color: locationTracker.trackingEnabled ? "#44ff44" : "#ff4444"
                    }
                }
            }
            
            // Personnel List
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
                        text: "Tracked Personnel"
                        font.pixelSize: 16
                        font.bold: true
                        color: "#ffffff"
                    }
                    
                    ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        
                        ListView {
                            id: personnelList
                            model: []
                            delegate: Rectangle {
                                width: ListView.view.width
                                height: 50
                                color: "#2a2a2a"
                                radius: 4
                                
                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 10
                                    
                                    Text {
                                        text: modelData.id || "Unknown"
                                        Layout.fillWidth: true
                                        color: "#ffffff"
                                    }
                                    
                                    Button {
                                        text: "Track"
                                        onClicked: {
                                            locationTracker.startTracking(modelData.id)
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // Map View
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#1a1a1a"
            border.color: "#444444"
            border.width: 1
            radius: 8
            
            Text {
                anchors.centerIn: parent
                text: "Map View\n(Integration with QtLocation required)"
                color: "#aaaaaa"
                font.pixelSize: 16
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}

