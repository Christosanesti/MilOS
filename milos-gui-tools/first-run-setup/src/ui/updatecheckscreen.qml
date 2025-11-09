import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: updateCheckScreen
    
    color: "#0a0a0a"
    
    property bool checking: false
    property bool installing: false
    property double installationProgress: 0.0
    property var availableUpdates: []
    property var selectedUpdates: []
    property bool hasCriticalUpdates: false
    
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
            text: "System Updates"
            font.pixelSize: 28
            font.weight: Font.Bold
            color: "#ffffff"
        }
        
        Text {
            Layout.fillWidth: true
            text: "Check for and install available system updates"
            font.pixelSize: 16
            color: "#cccccc"
        }
        
        // Update scanning progress
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 150
            variant: "dashboard"
            title: "Checking for Updates"
            visible: updateCheckScreen.checking
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 16
                
                ProgressBar {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 32
                    variant: "linear"
                    value: 0
                    status: "processing"
                    indeterminate: true
                }
                
                Text {
                    Layout.fillWidth: true
                    text: "Scanning package repositories..."
                    font.pixelSize: 14
                    color: "#00d4ff"
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
        
        // Available updates list
        Card {
            Layout.fillWidth: true
            Layout.fillHeight: true
            variant: "dashboard"
            title: "Available Updates"
            visible: !updateCheckScreen.checking && updateCheckScreen.availableUpdates.length > 0
            
            ScrollView {
                anchors.fill: parent
                anchors.margins: 24
                clip: true
                
                ColumnLayout {
                    width: updateCheckScreen.width - 88
                    spacing: 12
                    
                    Repeater {
                        model: updateCheckScreen.availableUpdates
                        
                        Card {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 100
                            variant: modelData.critical ? "interactive" : "dashboard"
                            state: modelData.critical ? "active" : "default"
                            
                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 16
                                spacing: 16
                                
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 4
                                    
                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 8
                                        
                                        Text {
                                            Layout.fillWidth: true
                                            text: modelData.name
                                            font.pixelSize: 16
                                            font.weight: Font.Medium
                                            color: "#ffffff"
                                        }
                                        
                                        StatusIndicator {
                                            variant: "badge"
                                            status: modelData.critical ? "error" : "default"
                                            text: modelData.critical ? "Critical" : "Regular"
                                            visible: modelData.critical
                                        }
                                    }
                                    
                                    Text {
                                        Layout.fillWidth: true
                                        text: modelData.description || "Package update"
                                        font.pixelSize: 12
                                        color: "#b0b0b0"
                                    }
                                    
                                    Text {
                                        Layout.fillWidth: true
                                        text: "Version: " + modelData.currentVersion + " → " + modelData.newVersion
                                        font.pixelSize: 12
                                        font.family: "JetBrains Mono"
                                        color: "#b0b0b0"
                                    }
                                }
                                
                                // Selection checkbox (simplified)
                                Rectangle {
                                    width: 24
                                    height: 24
                                    radius: 4
                                    color: modelData.selected ? "#00d4ff" : "#2a2a2a"
                                    border.color: "#00d4ff"
                                    border.width: 1
                                    
                                    Text {
                                        anchors.centerIn: parent
                                        text: "✓"
                                        font.pixelSize: 16
                                        color: "#0a0a0a"
                                        visible: modelData.selected
                                    }
                                    
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            updateCheckScreen.availableUpdates[index].selected = !updateCheckScreen.availableUpdates[index].selected
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // No updates available
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 150
            variant: "dashboard"
            title: "System Status"
            visible: !updateCheckScreen.checking && updateCheckScreen.availableUpdates.length === 0
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 16
                
                StatusIndicator {
                    Layout.alignment: Qt.AlignHCenter
                    variant: "badge"
                    status: "success"
                    text: "Up to Date"
                }
                
                Text {
                    Layout.fillWidth: true
                    text: "Your system is up to date. No updates are available."
                    font.pixelSize: 14
                    color: "#cccccc"
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
        
        // Installation progress
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 150
            variant: "dashboard"
            title: "Installing Updates"
            visible: updateCheckScreen.installing
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 16
                
                ProgressBar {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 32
                    variant: "linear"
                    value: updateCheckScreen.installationProgress
                    status: "processing"
                }
                
                Text {
                    Layout.fillWidth: true
                    text: "Installing updates... " + Math.round(updateCheckScreen.installationProgress) + "%"
                    font.pixelSize: 14
                    color: "#00d4ff"
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
        
        // Critical updates warning
        NotificationAlert {
            Layout.fillWidth: true
            variant: "error"
            title: "Critical Security Updates Available"
            message: "Critical security updates are available. It is strongly recommended to install these updates."
            visible: updateCheckScreen.hasCriticalUpdates && !updateCheckScreen.installing
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
                enabled: !updateCheckScreen.checking && !updateCheckScreen.installing
                onClicked: {
                    updateCheckScreen.backClicked()
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
                enabled: !updateCheckScreen.checking && !updateCheckScreen.installing
                onClicked: {
                    updateCheckScreen.skipClicked()
                }
            }
            
            Button {
                Layout.preferredWidth: 150
                Layout.preferredHeight: 48
                variant: "primary"
                text: updateCheckScreen.installing ? "Installing..." : "Install Updates"
                enabled: !updateCheckScreen.checking && !updateCheckScreen.installing && updateCheckScreen.availableUpdates.length > 0
                onClicked: {
                    updateCheckScreen.installing = true
                    // Simulate installation
                    var progress = 0
                    var timer = Qt.createQmlObject("import QtQuick 2.15; Timer { interval: 100; running: true; repeat: true }", updateCheckScreen)
                    timer.triggered.connect(function() {
                        progress += 2
                        updateCheckScreen.installationProgress = progress
                        if (progress >= 100) {
                            timer.stop()
                            updateCheckScreen.installing = false
                            updateCheckScreen.installationProgress = 0
                            updateCheckScreen.availableUpdates = []
                        }
                    })
                }
            }
        }
    }
    
    // Start checking on load
    Component.onCompleted: {
        updateCheckScreen.checking = true
        // Simulate update check
        Qt.callLater(function() {
            updateCheckScreen.checking = false
            updateCheckScreen.availableUpdates = [
                {name: "kernel", description: "Linux kernel security update", currentVersion: "6.1.0", newVersion: "6.1.1", critical: true, selected: true},
                {name: "openssl", description: "OpenSSL security patch", currentVersion: "3.0.0", newVersion: "3.0.1", critical: true, selected: true},
                {name: "firefox", description: "Firefox browser update", currentVersion: "120.0", newVersion: "121.0", critical: false, selected: false}
            ]
            updateCheckScreen.hasCriticalUpdates = updateCheckScreen.availableUpdates.some(function(u) { return u.critical })
        })
    }
    
    // Accessibility
    Accessible.role: Accessible.Pane
    Accessible.name: "System Updates"
    Accessible.description: "Check for and install available system updates"
}

