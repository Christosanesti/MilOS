import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import org.milos.Components 1.0
import org.milos.AnonymityController 1.0

ApplicationWindow {
    id: window
    title: "Anonymity Layer Controller"
    width: 900
    height: 700
    visible: true
    
    property var anonymityManager: AnonymityManager {
        id: manager
        Component.onCompleted: {
            startMonitoring()
        }
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20
        
        // Header
        Text {
            text: "Anonymity Layer Controller"
            font.pixelSize: 24
            font.bold: true
            Layout.fillWidth: true
        }
        
        // Toggle Buttons Section
        Card {
            title: "Network Toggles"
            Layout.fillWidth: true
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 30
                
                // Tor Toggle
                ColumnLayout {
                    spacing: 10
                    
                    Button {
                        id: torToggle
                        text: manager.torEnabled ? "Disable Tor" : "Enable Tor"
                        Layout.preferredWidth: 200
                        Layout.preferredHeight: 60
                        
                        // Xenon glow based on status
                        property double glowIntensity: {
                            if (manager.torStatus === "connected") return 1.0
                            if (manager.torStatus === "connecting") return 0.5
                            return 0.2
                        }
                        
                        // Xenon glow effect
                        Rectangle {
                            anchors.fill: parent
                            color: "transparent"
                            border.color: {
                                if (manager.torStatus === "connected") return "#00ff00"
                                if (manager.torStatus === "connecting") return "#00aaff"
                                return "#ff0000"
                            }
                            border.width: 3
                            radius: 5
                            opacity: parent.glowIntensity
                            
                            // Glow effect
                            Rectangle {
                                anchors.fill: parent
                                color: parent.border.color
                                opacity: 0.3 * parent.opacity
                                radius: parent.radius
                            }
                        }
                        
                        onClicked: manager.toggleTor()
                    }
                    
                    StatusIndicator {
                        status: {
                            if (manager.torStatus === "connected") return "success"
                            if (manager.torStatus === "connecting") return "warning"
                            return "error"
                        }
                        text: manager.torStatus
                        Layout.alignment: Qt.AlignHCenter
                    }
                }
                
                // I2P Toggle
                ColumnLayout {
                    spacing: 10
                    
                    Button {
                        id: i2pToggle
                        text: manager.i2pEnabled ? "Disable I2P" : "Enable I2P"
                        Layout.preferredWidth: 200
                        Layout.preferredHeight: 60
                        
                        // Xenon glow based on status
                        property double glowIntensity: {
                            if (manager.i2pStatus === "connected") return 1.0
                            if (manager.i2pStatus === "connecting") return 0.5
                            return 0.2
                        }
                        
                        // Xenon glow effect
                        Rectangle {
                            anchors.fill: parent
                            color: "transparent"
                            border.color: {
                                if (manager.i2pStatus === "connected") return "#00ff00"
                                if (manager.i2pStatus === "connecting") return "#00aaff"
                                return "#ff0000"
                            }
                            border.width: 3
                            radius: 5
                            opacity: parent.glowIntensity
                            
                            // Glow effect
                            Rectangle {
                                anchors.fill: parent
                                color: parent.border.color
                                opacity: 0.3 * parent.opacity
                                radius: parent.radius
                            }
                        }
                        
                        onClicked: manager.toggleI2P()
                    }
                    
                    StatusIndicator {
                        status: {
                            if (manager.i2pStatus === "connected") return "success"
                            if (manager.i2pStatus === "connecting") return "warning"
                            return "error"
                        }
                        text: manager.i2pStatus
                        Layout.alignment: Qt.AlignHCenter
                    }
                }
            }
        }
        
        // Bandwidth Monitoring
        Card {
            title: "Bandwidth Monitoring"
            Layout.fillWidth: true
            Layout.preferredHeight: 300
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 20
                
                // Tor Bandwidth
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 10
                    
                    Text {
                        text: "Tor Bandwidth"
                        font.bold: true
                        Layout.fillWidth: true
                    }
                    
                    // Bandwidth visualization placeholder (would use Data Visualization component)
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: "#1a1a2e"
                        border.color: "#0ea5e9"
                        border.width: 2
                        radius: 5
                        
                        ColumnLayout {
                            anchors.centerIn: parent
                            spacing: 10
                            
                            Text {
                                text: "Upload: " + (manager.torBandwidth.uploadRate || 0) + " KB/s"
                                color: "#00aaff"
                                font.pixelSize: 16
                            }
                            
                            Text {
                                text: "Download: " + (manager.torBandwidth.downloadRate || 0) + " KB/s"
                                color: "#00ff00"
                                font.pixelSize: 16
                            }
                        }
                    }
                    
                    GridLayout {
                        columns: 2
                        Layout.fillWidth: true
                        
                        Text { text: "Upload:" }
                        Text { 
                            text: (manager.torBandwidth.uploadRate || 0) + " KB/s"
                            font.bold: true
                        }
                        
                        Text { text: "Download:" }
                        Text { 
                            text: (manager.torBandwidth.downloadRate || 0) + " KB/s"
                            font.bold: true
                        }
                    }
                }
                
                // I2P Bandwidth
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 10
                    
                    Text {
                        text: "I2P Bandwidth"
                        font.bold: true
                        Layout.fillWidth: true
                    }
                    
                    // Bandwidth visualization placeholder (would use Data Visualization component)
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: "#1a1a2e"
                        border.color: "#0ea5e9"
                        border.width: 2
                        radius: 5
                        
                        ColumnLayout {
                            anchors.centerIn: parent
                            spacing: 10
                            
                            Text {
                                text: "Upload: " + (manager.i2pBandwidth.uploadRate || 0) + " KB/s"
                                color: "#00aaff"
                                font.pixelSize: 16
                            }
                            
                            Text {
                                text: "Download: " + (manager.i2pBandwidth.downloadRate || 0) + " KB/s"
                                color: "#00ff00"
                                font.pixelSize: 16
                            }
                        }
                    }
                    
                    GridLayout {
                        columns: 2
                        Layout.fillWidth: true
                        
                        Text { text: "Upload:" }
                        Text { 
                            text: (manager.i2pBandwidth.uploadRate || 0) + " KB/s"
                            font.bold: true
                        }
                        
                        Text { text: "Download:" }
                        Text { 
                            text: (manager.i2pBandwidth.downloadRate || 0) + " KB/s"
                            font.bold: true
                        }
                    }
                }
            }
        }
        
        // Network Routing Information
        Card {
            title: "Network Routing Information"
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            
            ScrollView {
                anchors.fill: parent
                anchors.margins: 20
                
                ColumnLayout {
                    width: parent.width
                    spacing: 15
                    
                    // Tor Routing
                    GroupBox {
                        title: "Tor Configuration"
                        Layout.fillWidth: true
                        
                        GridLayout {
                            columns: 2
                            anchors.fill: parent
                            
                            Text { text: "Status:" }
                            Text { 
                                text: manager.torStatus
                                font.bold: true
                            }
                            
                            Text { text: "SOCKS Port:" }
                            Text { text: "9050" }
                            
                            Text { text: "Control Port:" }
                            Text { text: "9051" }
                        }
                    }
                    
                    // I2P Routing
                    GroupBox {
                        title: "I2P Configuration"
                        Layout.fillWidth: true
                        
                        GridLayout {
                            columns: 2
                            anchors.fill: parent
                            
                            Text { text: "Status:" }
                            Text { 
                                text: manager.i2pStatus
                                font.bold: true
                            }
                            
                            Text { text: "HTTP Proxy:" }
                            Text { text: "127.0.0.1:4444" }
                            
                            Text { text: "Router Port:" }
                            Text { text: "7657" }
                        }
                    }
                }
            }
        }
    }
}

