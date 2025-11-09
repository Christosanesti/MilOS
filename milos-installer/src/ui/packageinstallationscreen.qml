import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: packageInstallationScreen
    
    color: "#0a0a0a"
    
    property bool installing: false
    property double installationProgress: 0.0
    property int packagesInstalled: 0
    property int totalPackages: 100
    property string currentPackage: ""
    property string estimatedTimeRemaining: "5 minutes"
    property var packageCategories: [
        {name: "Base System", icon: "⚙", installed: true, progress: 100},
        {name: "Security Tools", icon: "🔒", installed: false, progress: 45},
        {name: "Desktop Environment", icon: "🖥", installed: false, progress: 0},
        {name: "System Utilities", icon: "🛠", installed: false, progress: 0}
    ]
    property bool showLog: false
    
    signal backClicked()
    signal nextClicked()
    signal errorOccurred(string error)
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 32
        spacing: 24
        
        // Title
        Text {
            Layout.fillWidth: true
            text: "Installing Packages"
            font.pixelSize: 28
            font.weight: Font.Bold
            color: "#ffffff"
        }
        
        Text {
            Layout.fillWidth: true
            text: "Installing required packages and configuring system"
            font.pixelSize: 16
            color: "#cccccc"
        }
        
        // Package categories
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 120
            variant: "dashboard"
            title: "Package Categories"
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 16
                
                Repeater {
                    model: packageInstallationScreen.packageCategories
                    
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        
                        Rectangle {
                            Layout.alignment: Qt.AlignHCenter
                            width: 64
                            height: 64
                            radius: 32
                            color: modelData.installed ? "#00cc66" : (modelData.progress > 0 ? "#00d4ff" : "#2a2a2a")
                            border.color: "#00d4ff"
                            border.width: 2
                            
                            // Xenon glow animation
                            SequentialAnimation on opacity {
                                running: modelData.progress > 0 && modelData.progress < 100
                                loops: Animation.Infinite
                                NumberAnimation {
                                    from: 0.6
                                    to: 1.0
                                    duration: 1000
                                    easing.type: Easing.EaseInOut
                                }
                                NumberAnimation {
                                    from: 1.0
                                    to: 0.6
                                    duration: 1000
                                    easing.type: Easing.EaseInOut
                                }
                            }
                            
                            Text {
                                anchors.centerIn: parent
                                text: modelData.icon
                                font.pixelSize: 32
                            }
                        }
                        
                        Text {
                            Layout.fillWidth: true
                            text: modelData.name
                            font.pixelSize: 12
                            color: "#ffffff"
                            horizontalAlignment: Text.AlignHCenter
                        }
                        
                        ProgressBar {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 4
                            variant: "linear"
                            value: modelData.progress
                            status: modelData.installed ? "success" : (modelData.progress > 0 ? "processing" : "default")
                        }
                    }
                }
            }
        }
        
        // Installation progress
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            variant: "dashboard"
            title: "Installation Progress"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 16
                
                ProgressBar {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 32
                    variant: "linear"
                    value: packageInstallationScreen.installationProgress
                    status: "processing"
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 16
                    
                    Text {
                        Layout.fillWidth: true
                        text: "Packages: " + packageInstallationScreen.packagesInstalled + " / " + packageInstallationScreen.totalPackages
                        font.pixelSize: 14
                        color: "#ffffff"
                    }
                    
                    Text {
                        text: "Time remaining: " + packageInstallationScreen.estimatedTimeRemaining
                        font.pixelSize: 14
                        color: "#00d4ff"
                    }
                }
                
                Text {
                    Layout.fillWidth: true
                    text: "Installing: " + (packageInstallationScreen.currentPackage || "Preparing...")
                    font.pixelSize: 14
                    color: "#00d4ff"
                    font.weight: Font.Medium
                }
                
                // Installation log toggle
                Button {
                    Layout.preferredWidth: 150
                    Layout.preferredHeight: 36
                    variant: "secondary"
                    text: packageInstallationScreen.showLog ? "Hide Log" : "Show Log"
                    onClicked: {
                        packageInstallationScreen.showLog = !packageInstallationScreen.showLog
                    }
                }
                
                // Installation log
                ScrollView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 100
                    visible: packageInstallationScreen.showLog
                    clip: true
                    
                    Text {
                        width: packageInstallationScreen.width - 88
                        text: "[INFO] Installing base system packages...\n[INFO] Installing security tools...\n[INFO] Configuring GRUB bootloader...\n[INFO] Package installation in progress..."
                        font.pixelSize: 12
                        font.family: "JetBrains Mono"
                        color: "#b0b0b0"
                        wrapMode: Text.WordWrap
                    }
                }
            }
        }
        
        // Checkpoint indicators
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            variant: "dashboard"
            title: "Progress Checkpoints"
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 16
                
                Repeater {
                    model: ["Partitioning", "Encryption", "User Account", "Network", "Hardware", "Packages"]
                    
                    StatusIndicator {
                        variant: "badge"
                        status: {
                            var stepIndex = index
                            var currentStep = Math.floor(packageInstallationScreen.installationProgress / 16.67)
                            if (stepIndex < currentStep) return "success"
                            if (stepIndex === currentStep) return "processing"
                            return "default"
                        }
                        text: modelData
                    }
                }
            }
        }
        
        // Navigation buttons (disabled during installation)
        RowLayout {
            Layout.fillWidth: true
            spacing: 16
            
            Button {
                Layout.preferredWidth: 120
                Layout.preferredHeight: 48
                variant: "secondary"
                text: "Back"
                enabled: !packageInstallationScreen.installing
                onClicked: {
                    packageInstallationScreen.backClicked()
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
                enabled: !packageInstallationScreen.installing && packageInstallationScreen.installationProgress >= 100
                onClicked: {
                    packageInstallationScreen.nextClicked()
                }
            }
        }
    }
    
    // Start installation on load
    Component.onCompleted: {
        packageInstallationScreen.installing = true
        // Simulate package installation
        var progress = 0
        var packages = 0
        var timer = Qt.createQmlObject("import QtQuick 2.15; Timer { interval: 100; running: true; repeat: true }", packageInstallationScreen)
        timer.triggered.connect(function() {
            progress += 1
            packages = Math.floor(progress / 100 * packageInstallationScreen.totalPackages)
            packageInstallationScreen.installationProgress = progress
            packageInstallationScreen.packagesInstalled = packages
            packageInstallationScreen.currentPackage = "package-" + packages + ".pkg"
            
            // Update category progress
            if (progress < 25) {
                packageInstallationScreen.packageCategories[1].progress = (progress / 25) * 100
            } else if (progress < 50) {
                packageInstallationScreen.packageCategories[1].progress = 100
                packageInstallationScreen.packageCategories[1].installed = true
                packageInstallationScreen.packageCategories[2].progress = ((progress - 25) / 25) * 100
            } else if (progress < 75) {
                packageInstallationScreen.packageCategories[2].progress = 100
                packageInstallationScreen.packageCategories[2].installed = true
                packageInstallationScreen.packageCategories[3].progress = ((progress - 50) / 25) * 100
            } else {
                packageInstallationScreen.packageCategories[3].progress = 100
                packageInstallationScreen.packageCategories[3].installed = true
            }
            
            // Update estimated time
            var remaining = Math.max(0, (100 - progress) / 2)
            packageInstallationScreen.estimatedTimeRemaining = Math.ceil(remaining) + " minutes"
            
            if (progress >= 100) {
                timer.stop()
                packageInstallationScreen.installing = false
                packageInstallationScreen.currentPackage = "Installation complete"
            }
        })
    }
    
    // Accessibility
    Accessible.role: Accessible.Pane
    Accessible.name: "Package Installation"
    Accessible.description: "Installing packages and configuring system"
}

