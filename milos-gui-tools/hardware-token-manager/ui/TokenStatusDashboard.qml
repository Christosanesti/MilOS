import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

/**
 * Token Status Dashboard
 * 
 * Displays all enrolled tokens with visual status indicators
 */
Rectangle {
    id: dashboard
    
    property var tokenManager: null
    color: "#1a1a1a"
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16
        
        Text {
            text: "Hardware Tokens"
            font.pixelSize: 20
            font.bold: true
            color: "#ffffff"
        }
        
        ListView {
            id: tokenList
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: tokenManager ? tokenManager.tokens : []
            spacing: 8
            
            delegate: Rectangle {
                width: tokenList.width
                height: 120
                color: "#2a2a2a"
                border.color: getStatusColor(modelData.connection_status || "disconnected")
                border.width: 2
                radius: 4
                
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 12
                    
                    // Status indicator
                    Rectangle {
                        width: 12
                        height: 12
                        radius: 6
                        color: getStatusColor(modelData.connection_status || "disconnected")
                        
                        SequentialAnimation on opacity {
                            running: modelData.connection_status === "connected"
                            loops: Animation.Infinite
                            NumberAnimation {
                                from: 0.5
                                to: 1.0
                                duration: 1000
                            }
                            NumberAnimation {
                                from: 1.0
                                to: 0.5
                                duration: 1000
                            }
                        }
                    }
                    
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 4
                        
                        Text {
                            text: modelData.name || "Unnamed Token"
                            color: "#ffffff"
                            font.pixelSize: 14
                            font.bold: true
                        }
                        
                        Text {
                            text: "Type: " + (modelData.type || "Unknown")
                            color: "#b0b0b0"
                            font.pixelSize: 12
                        }
                        
                        Text {
                            text: "Status: " + (modelData.connection_status || "disconnected")
                            color: "#b0b0b0"
                            font.pixelSize: 12
                        }
                        
                        // Capabilities
                        Row {
                            spacing: 8
                            Repeater {
                                model: modelData.capabilities || []
                                Text {
                                    text: modelData
                                    color: "#00d4ff"
                                    font.pixelSize: 11
                                    padding: 4
                                    background: Rectangle {
                                        color: "#1a1a1a"
                                        border.color: "#00d4ff"
                                        border.width: 1
                                        radius: 2
                                    }
                                }
                            }
                        }
                    }
                    
                    Button {
                        text: "Configure"
                        onClicked: {
                            // Open configuration dialog
                        }
                    }
                    
                    Button {
                        text: "Remove"
                        onClicked: {
                            // Open removal dialog
                        }
                    }
                }
            }
        }
        
        Button {
            text: "Enroll New Token"
            Layout.fillWidth: true
            onClicked: {
                // Open enrollment wizard
            }
        }
    }
    
    function getStatusColor(status) {
        switch (status) {
            case "connected": return "#00cc66"
            case "disconnected": return "#808080"
            case "error": return "#ff4444"
            case "enrollment_required": return "#ffaa00"
            default: return "#808080"
        }
    }
}

