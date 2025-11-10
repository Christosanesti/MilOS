import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: networkConfigurationScreen
    
    color: "#0a0a0a"
    
    property bool scanning: false
    property var networkInterfaces: []
    property string selectedInterface: ""
    property bool useDHCP: true
    property string ipAddress: ""
    property string subnetMask: ""
    property string gateway: ""
    property string dnsServer: ""
    property string connectionStatus: "disconnected" // disconnected, connecting, connected, error
    property bool canSkip: true
    property var networkActivityData: [] // For data visualization
    property bool showTestConnectionDialog: false
    
    signal backClicked()
    signal nextClicked()
    signal skipClicked()
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 32
        spacing: 24
        
        // Title
        Text {
            Layout.fillWidth: true
            text: "Network Configuration"
            font.pixelSize: 28
            font.weight: Font.Bold
            color: "#ffffff"
        }
        
        Text {
            Layout.fillWidth: true
            text: "Configure network connectivity for package installation"
            font.pixelSize: 16
            color: "#cccccc"
        }
        
        // Network interface detection
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            variant: "dashboard"
            title: "Network Interfaces"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 16
                
                // Scanning indicator
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12
                    visible: networkConfigurationScreen.scanning
                    
                    ProgressBar {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 8
                        variant: "linear"
                        value: 0
                        status: "processing"
                        indeterminate: true
                    }
                    
                    Text {
                        text: "Scanning..."
                        font.pixelSize: 14
                        color: "#00d4ff"
                    }
                }
                
                // Network interfaces list
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    visible: !networkConfigurationScreen.scanning && networkConfigurationScreen.networkInterfaces.length > 0
                    
                    ColumnLayout {
                        width: networkConfigurationScreen.width - 88
                        spacing: 12
                        
                        Repeater {
                            model: networkConfigurationScreen.networkInterfaces
                            
                            Card {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 80
                                variant: networkConfigurationScreen.selectedInterface === modelData.name ? "interactive" : "dashboard"
                                state: networkConfigurationScreen.selectedInterface === modelData.name ? "active" : "default"
                                
                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 16
                                    spacing: 16
                                    
                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 4
                                        
                                        Text {
                                            Layout.fillWidth: true
                                            text: modelData.name
                                            font.pixelSize: 16
                                            font.weight: Font.Medium
                                            color: "#ffffff"
                                        }
                                        
                                        Text {
                                            Layout.fillWidth: true
                                            text: modelData.type + " • " + modelData.macAddress
                                            font.pixelSize: 12
                                            color: "#b0b0b0"
                                        }
                                    }
                                    
                                    StatusIndicator {
                                        variant: "badge"
                                        status: modelData.status === "connected" ? "success" : (modelData.status === "error" ? "error" : "warning")
                                        text: modelData.status
                                    }
                                }
                                
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        networkConfigurationScreen.selectedInterface = modelData.name
                                        networkConfigurationScreen.connectionStatus = modelData.status
                                    }
                                }
                            }
                        }
                    }
                }
                
                // No interfaces found
                Text {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    text: "No network interfaces detected"
                    font.pixelSize: 14
                    color: "#ffaa00"
                    horizontalAlignment: Text.AlignHCenter
                    visible: !networkConfigurationScreen.scanning && networkConfigurationScreen.networkInterfaces.length === 0
                }
            }
        }
        
        // Network configuration options
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 300
            variant: "dashboard"
            title: "Configuration"
            visible: networkConfigurationScreen.selectedInterface.length > 0
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 16
                
                // DHCP/Manual toggle
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 16
                    
                    Button {
                        Layout.preferredWidth: 150
                        Layout.preferredHeight: 48
                        variant: networkConfigurationScreen.useDHCP ? "primary" : "secondary"
                        text: "Use DHCP"
                        onClicked: {
                            networkConfigurationScreen.useDHCP = true
                        }
                    }
                    
                    Button {
                        Layout.preferredWidth: 150
                        Layout.preferredHeight: 48
                        variant: !networkConfigurationScreen.useDHCP ? "primary" : "secondary"
                        text: "Manual IP"
                        onClicked: {
                            networkConfigurationScreen.useDHCP = false
                        }
                    }
                }
                
                // Manual IP configuration form
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 12
                    visible: !networkConfigurationScreen.useDHCP
                    
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 8
                            
                            Text {
                                Layout.fillWidth: true
                                text: "IP Address"
                                font.pixelSize: 14
                                color: "#e0e0e0"
                            }
                            
                            InputField {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 48
                                variant: "text"
                                placeholderText: "192.168.1.100"
                                text: networkConfigurationScreen.ipAddress
                                onTextChanged: {
                                    networkConfigurationScreen.ipAddress = text
                                }
                            }
                        }
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 8
                            
                            Text {
                                Layout.fillWidth: true
                                text: "Subnet Mask"
                                font.pixelSize: 14
                                color: "#e0e0e0"
                            }
                            
                            InputField {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 48
                                variant: "text"
                                placeholderText: "255.255.255.0"
                                text: networkConfigurationScreen.subnetMask
                                onTextChanged: {
                                    networkConfigurationScreen.subnetMask = text
                                }
                            }
                        }
                    }
                    
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 8
                            
                            Text {
                                Layout.fillWidth: true
                                text: "Gateway"
                                font.pixelSize: 14
                                color: "#e0e0e0"
                            }
                            
                            InputField {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 48
                                variant: "text"
                                placeholderText: "192.168.1.1"
                                text: networkConfigurationScreen.gateway
                                onTextChanged: {
                                    networkConfigurationScreen.gateway = text
                                }
                            }
                        }
                        
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 8
                            
                            Text {
                                Layout.fillWidth: true
                                text: "DNS Server"
                                font.pixelSize: 14
                                color: "#e0e0e0"
                            }
                            
                            InputField {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 48
                                variant: "text"
                                placeholderText: "8.8.8.8"
                                text: networkConfigurationScreen.dnsServer
                                onTextChanged: {
                                    networkConfigurationScreen.dnsServer = text
                                }
                            }
                        }
                    }
                }
                
                // Connection status
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12
                    
                    StatusIndicator {
                        variant: "badge"
                        status: {
                            if (networkConfigurationScreen.connectionStatus === "connected") return "success"
                            if (networkConfigurationScreen.connectionStatus === "error") return "error"
                            return "warning"
                        }
                        text: {
                            if (networkConfigurationScreen.connectionStatus === "connected") return "Connected"
                            if (networkConfigurationScreen.connectionStatus === "connecting") return "Connecting..."
                            if (networkConfigurationScreen.connectionStatus === "error") return "Connection Error"
                            return "Disconnected"
                        }
                    }
                    
                    Button {
                        Layout.preferredWidth: 120
                        Layout.preferredHeight: 40
                        variant: "secondary"
                        text: "Test Connection"
                        onClicked: {
                            networkConfigurationScreen.showTestConnectionDialog = true
                        }
                    }
                }
            }
        }
        
        // Network activity visualization
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 250
            variant: "dashboard"
            title: "Network Activity"
            visible: networkConfigurationScreen.connectionStatus === "connected"
            
            DataVisualization {
                anchors.fill: parent
                anchors.margins: 24
                variant: "line"
                state: networkConfigurationScreen.connectionStatus === "connected" ? "animated" : "default"
                data: networkConfigurationScreen.networkActivityData
            }
        }
        
        // Error/warning notification
        NotificationAlert {
            Layout.fillWidth: true
            variant: "warning"
            title: "Network Configuration"
            message: "Network configuration is optional. You can skip this step and continue with offline installation."
            visible: networkConfigurationScreen.connectionStatus === "error" || networkConfigurationScreen.networkInterfaces.length === 0
        }
        
        // Navigation buttons
        RowLayout {
            Layout.fillWidth: true
            spacing: 16
            
            Button {
                Layout.preferredWidth: 120
                Layout.preferredHeight: 48
                variant: "secondary"
                text: "Back"
                onClicked: {
                    networkConfigurationScreen.backClicked()
                }
            }
            
            Item {
                Layout.fillWidth: true
            }
            
            Button {
                Layout.preferredWidth: 120
                Layout.preferredHeight: 48
                variant: "secondary"
                text: "Skip"
                onClicked: {
                    networkConfigurationScreen.skipClicked()
                }
            }
            
            Button {
                Layout.preferredWidth: 120
                Layout.preferredHeight: 48
                variant: "primary"
                text: "Next"
                enabled: networkConfigurationScreen.connectionStatus === "connected" || networkConfigurationScreen.canSkip
                onClicked: {
                    networkConfigurationScreen.nextClicked()
                }
            }
        }
    }
    
    // Modal Dialog for connection test confirmation
    ModalDialog {
        id: testConnectionDialog
        anchors.fill: parent
        variant: "confirmation"
        title: "Test Network Connection"
        message: "This will test the network connection using the configured settings. Continue?"
        critical: false
        visible: networkConfigurationScreen.showTestConnectionDialog
        
        onConfirmed: {
            networkConfigurationScreen.showTestConnectionDialog = false
            networkConfigurationScreen.connectionStatus = "connecting"
            // TODO: Test network connection
            Qt.callLater(function() {
                networkConfigurationScreen.connectionStatus = "connected"
                // Generate sample network activity data
                var data = []
                for (var i = 0; i < 20; i++) {
                    data.push({x: i, y: Math.random() * 100})
                }
                networkConfigurationScreen.networkActivityData = data
            })
        }
        
        onCancelled: {
            networkConfigurationScreen.showTestConnectionDialog = false
        }
    }
    
    // Start scanning on load
    Component.onCompleted: {
        networkConfigurationScreen.scanning = true
        // TODO: Detect network interfaces
        Qt.callLater(function() {
            networkConfigurationScreen.networkInterfaces = [
                {name: "eth0", type: "Ethernet", macAddress: "00:11:22:33:44:55", status: "disconnected"},
                {name: "wlan0", type: "Wireless", macAddress: "AA:BB:CC:DD:EE:FF", status: "disconnected"}
            ]
            networkConfigurationScreen.scanning = false
        })
        
        // Initialize empty network activity data
        networkConfigurationScreen.networkActivityData = []
    }
    
    // Accessibility
    Accessible.role: Accessible.Pane
    Accessible.name: "Network Configuration"
    Accessible.description: "Configure network connectivity for package installation"
}

