import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: hardwareDetectionScreen
    
    color: "#0a0a0a"
    
    property bool scanning: false
    property double scanProgress: 0.0
    property var detectedHardware: []
    property bool installingDrivers: false
    property double driverProgress: 0.0
    property string currentDriver: ""
    property var compatibilityWarnings: []
    
    signal backClicked()
    signal nextClicked()
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 32
        spacing: 24
        
        // Title
        Text {
            Layout.fillWidth: true
            text: "Hardware Detection"
            font.pixelSize: 28
            font.weight: Font.Bold
            color: "#ffffff"
        }
        
        Text {
            Layout.fillWidth: true
            text: "Detecting hardware and installing required drivers"
            font.pixelSize: 16
            color: "#cccccc"
        }
        
        // Hardware scanning progress
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 150
            variant: "dashboard"
            title: "Hardware Scanning"
            visible: hardwareDetectionScreen.scanning
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 16
                
                ProgressBar {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 32
                    variant: "linear"
                    value: hardwareDetectionScreen.scanProgress
                    status: "processing"
                }
                
                Text {
                    Layout.fillWidth: true
                    text: "Scanning hardware... " + Math.round(hardwareDetectionScreen.scanProgress) + "%"
                    font.pixelSize: 14
                    color: "#00d4ff"
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
        
        // Detected hardware list
        Card {
            Layout.fillWidth: true
            Layout.fillHeight: true
            variant: "dashboard"
            title: "Detected Hardware"
            visible: !hardwareDetectionScreen.scanning && hardwareDetectionScreen.detectedHardware.length > 0
            
            ScrollView {
                anchors.fill: parent
                anchors.margins: 24
                clip: true
                
                ColumnLayout {
                    width: hardwareDetectionScreen.width - 88
                    spacing: 12
                    
                    Repeater {
                        model: hardwareDetectionScreen.detectedHardware
                        
                        Card {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 100
                            variant: "dashboard"
                            
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
                                        text: modelData.type + " • " + modelData.vendor + " " + modelData.model
                                        font.pixelSize: 12
                                        color: "#b0b0b0"
                                    }
                                    
                                    Text {
                                        Layout.fillWidth: true
                                        text: "Driver: " + (modelData.driver || "Not installed")
                                        font.pixelSize: 12
                                        color: modelData.driver ? "#00cc66" : "#ffaa00"
                                    }
                                }
                                
                                StatusIndicator {
                                    variant: "badge"
                                    status: modelData.driver ? "success" : (modelData.compatible ? "warning" : "error")
                                    text: modelData.driver ? "Ready" : (modelData.compatible ? "Needs Driver" : "Incompatible")
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // Driver installation progress
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 150
            variant: "dashboard"
            title: "Driver Installation"
            visible: hardwareDetectionScreen.installingDrivers
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 16
                
                ProgressBar {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 32
                    variant: "linear"
                    value: hardwareDetectionScreen.driverProgress
                    status: "processing"
                }
                
                Text {
                    Layout.fillWidth: true
                    text: "Installing " + hardwareDetectionScreen.currentDriver + "... " + Math.round(hardwareDetectionScreen.driverProgress) + "%"
                    font.pixelSize: 14
                    color: "#00d4ff"
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
        
        // Compatibility warnings
        Repeater {
            model: hardwareDetectionScreen.compatibilityWarnings
            
            NotificationAlert {
                Layout.fillWidth: true
                variant: "warning"
                title: "Hardware Compatibility Warning"
                message: modelData.message
            }
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
                enabled: !hardwareDetectionScreen.scanning && !hardwareDetectionScreen.installingDrivers
                onClicked: {
                    hardwareDetectionScreen.backClicked()
                }
            }
            
            Item {
                Layout.fillWidth: true
            }
            
            Button {
                Layout.preferredWidth: 120
                Layout.preferredHeight: 48
                variant: "primary"
                text: "Next"
                enabled: !hardwareDetectionScreen.scanning && !hardwareDetectionScreen.installingDrivers
                onClicked: {
                    hardwareDetectionScreen.nextClicked()
                }
            }
        }
    }
    
    // Start scanning on load
    Component.onCompleted: {
        hardwareDetectionScreen.scanning = true
        // Simulate hardware scanning
        var progress = 0
        var timer = Qt.createQmlObject("import QtQuick 2.15; Timer { interval: 50; running: true; repeat: true }", hardwareDetectionScreen)
        timer.triggered.connect(function() {
            progress += 2
            hardwareDetectionScreen.scanProgress = progress
            if (progress >= 100) {
                timer.stop()
                hardwareDetectionScreen.scanning = false
                // Simulate detected hardware
                hardwareDetectionScreen.detectedHardware = [
                    {name: "Intel Core i7-9700K", type: "CPU", vendor: "Intel", model: "Core i7-9700K", driver: "intel-microcode", compatible: true},
                    {name: "NVIDIA GeForce RTX 3080", type: "GPU", vendor: "NVIDIA", model: "GeForce RTX 3080", driver: null, compatible: true},
                    {name: "Realtek RTL8111/8168", type: "Network", vendor: "Realtek", model: "RTL8111/8168", driver: "r8168", compatible: true},
                    {name: "Samsung SSD 980 PRO", type: "Storage", vendor: "Samsung", model: "SSD 980 PRO", driver: null, compatible: true}
                ]
                // Check for drivers that need installation
                var needsDrivers = hardwareDetectionScreen.detectedHardware.filter(function(hw) { return !hw.driver })
                if (needsDrivers.length > 0) {
                    hardwareDetectionScreen.installingDrivers = true
                    hardwareDetectionScreen.currentDriver = needsDrivers[0].name
                    // Simulate driver installation
                    var driverProgress = 0
                    var driverTimer = Qt.createQmlObject("import QtQuick 2.15; Timer { interval: 100; running: true; repeat: true }", hardwareDetectionScreen)
                    driverTimer.triggered.connect(function() {
                        driverProgress += 5
                        hardwareDetectionScreen.driverProgress = driverProgress
                        if (driverProgress >= 100) {
                            driverTimer.stop()
                            hardwareDetectionScreen.installingDrivers = false
                            hardwareDetectionScreen.driverProgress = 0
                            // Update hardware with installed driver
                            for (var i = 0; i < hardwareDetectionScreen.detectedHardware.length; i++) {
                                if (hardwareDetectionScreen.detectedHardware[i].name === hardwareDetectionScreen.currentDriver) {
                                    hardwareDetectionScreen.detectedHardware[i].driver = "installed"
                                    break
                                }
                            }
                        }
                    })
                }
            }
        })
    }
    
    // Accessibility
    Accessible.role: Accessible.Pane
    Accessible.name: "Hardware Detection"
    Accessible.description: "Detecting hardware and installing required drivers"
}

