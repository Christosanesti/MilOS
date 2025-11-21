import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: mainWindow
    width: 1024
    height: 768
    visible: true
    title: "MilOS Air-Gap Detection & Enforcement"
    
    color: "#0a0a0a"
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20
        
        // Status Card
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 150
            color: "#1a1a1a"
            border.color: airGapDetector.isAirGapped ? "#ff4444" : "#44ff44"
            border.width: 2
            radius: 8
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 10
                
                Text {
                    text: "Air-Gap Status"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#ffffff"
                }
                
                Text {
                    text: airGapDetector.isAirGapped ? "AIR-GAPPED" : "CONNECTED"
                    font.pixelSize: 24
                    font.bold: true
                    color: airGapDetector.isAirGapped ? "#ff4444" : "#44ff44"
                }
                
                Text {
                    text: "Last Check: " + (airGapDetector.lastCheckTime ? 
                          airGapDetector.lastCheckTime.toLocaleString(Qt.locale(), "hh:mm:ss") : "Never")
                    font.pixelSize: 12
                    color: "#aaaaaa"
                }
            }
        }
        
        // Policy Level Card
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
                    text: "Security Policy Level"
                    font.pixelSize: 16
                    font.bold: true
                    color: "#ffffff"
                }
                
                Text {
                    property string level: {
                        switch(airGapEnforcement.currentPolicyLevel) {
                            case 0: return "Normal"
                            case 1: return "Enhanced"
                            case 2: return "Maximum"
                            default: return "Unknown"
                        }
                    }
                    text: level
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffff44"
                }
                
                Text {
                    text: airGapEnforcement.dataLeakagePreventionEnabled ? 
                          "Data Leakage Prevention: ENABLED" : 
                          "Data Leakage Prevention: DISABLED"
                    font.pixelSize: 12
                    color: airGapEnforcement.dataLeakagePreventionEnabled ? "#ff4444" : "#44ff44"
                }
            }
        }
        
        // Blocked Operations
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
                    text: "Blocked Operations"
                    font.pixelSize: 16
                    font.bold: true
                    color: "#ffffff"
                }
                
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    
                    ListView {
                        model: {
                            var ops = airGapEnforcement.getBlockedOperations()
                            return ops
                        }
                        delegate: Rectangle {
                            width: ListView.view.width
                            height: 30
                            color: "#2a2a2a"
                            
                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.leftMargin: 10
                                text: modelData
                                color: "#ff4444"
                                font.pixelSize: 12
                            }
                        }
                    }
                }
            }
        }
        
        // Manual Check Button
        Button {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            text: "Check Now"
            onClicked: {
                airGapDetector.checkNow()
            }
        }
    }
}

