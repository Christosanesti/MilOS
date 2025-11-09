import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: securityOverview
    
    color: "#0a0a0a"
    
    property string firewallStatus: "inactive" // not_configured, inactive, active
    property string updateStatus: "checking" // checking, up_to_date, updates_available
    property double hardeningProgress: 0.0
    
    signal backClicked()
    signal nextClicked()
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 32
        spacing: 24
        
        // Title
        Text {
            Layout.fillWidth: true
            text: "Security Overview"
            font.pixelSize: 28
            font.weight: Font.Bold
            color: "#ffffff"
        }
        
        Text {
            Layout.fillWidth: true
            text: "Current security status of your system"
            font.pixelSize: 16
            color: "#cccccc"
        }
        
        // Security status cards
        GridLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            columns: 2
            columnSpacing: 16
            rowSpacing: 16
            
            // Firewall status
            Card {
                Layout.fillWidth: true
                Layout.preferredHeight: 180
                variant: "dashboard"
                title: "Firewall"
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 24
                    spacing: 16
                    
                    StatusIndicator {
                        variant: "badge"
                        status: {
                            if (securityOverview.firewallStatus === "active") return "success"
                            if (securityOverview.firewallStatus === "inactive") return "warning"
                            return "error"
                        }
                        text: {
                            if (securityOverview.firewallStatus === "active") return "Active"
                            if (securityOverview.firewallStatus === "inactive") return "Inactive"
                            return "Not Configured"
                        }
                    }
                    
                    Text {
                        Layout.fillWidth: true
                        text: {
                            if (securityOverview.firewallStatus === "active") return "Firewall is protecting your system"
                            if (securityOverview.firewallStatus === "inactive") return "Firewall is not active"
                            return "Firewall needs to be configured"
                        }
                        font.pixelSize: 14
                        color: "#cccccc"
                        wrapMode: Text.WordWrap
                    }
                }
            }
            
            // Update status
            Card {
                Layout.fillWidth: true
                Layout.preferredHeight: 180
                variant: "dashboard"
                title: "System Updates"
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 24
                    spacing: 16
                    
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12
                        
                        StatusIndicator {
                            variant: "badge"
                            status: {
                                if (securityOverview.updateStatus === "up_to_date") return "success"
                                if (securityOverview.updateStatus === "updates_available") return "warning"
                                return "processing"
                            }
                            text: {
                                if (securityOverview.updateStatus === "up_to_date") return "Up to Date"
                                if (securityOverview.updateStatus === "updates_available") return "Updates Available"
                                return "Checking..."
                            }
                        }
                        
                        // Xenon scanning animation
                        ProgressBar {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 8
                            variant: "linear"
                            value: 0
                            status: "processing"
                            indeterminate: securityOverview.updateStatus === "checking"
                            visible: securityOverview.updateStatus === "checking"
                        }
                    }
                    
                    Text {
                        Layout.fillWidth: true
                        text: {
                            if (securityOverview.updateStatus === "up_to_date") return "Your system is up to date"
                            if (securityOverview.updateStatus === "updates_available") return "Security updates are available"
                            return "Checking for available updates..."
                        }
                        font.pixelSize: 14
                        color: "#cccccc"
                        wrapMode: Text.WordWrap
                    }
                }
            }
            
            // Hardening status
            Card {
                Layout.fillWidth: true
                Layout.preferredHeight: 180
                Layout.columnSpan: 2
                variant: "dashboard"
                title: "Security Hardening"
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 24
                    spacing: 16
                    
                    ProgressBar {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 32
                        variant: "linear"
                        value: securityOverview.hardeningProgress
                        status: securityOverview.hardeningProgress < 100 ? "processing" : "success"
                    }
                    
                    Text {
                        Layout.fillWidth: true
                        text: "Hardening Level: " + (securityOverview.hardeningProgress < 25 ? "Basic" : 
                                                      securityOverview.hardeningProgress < 50 ? "Standard" :
                                                      securityOverview.hardeningProgress < 75 ? "High" : "Maximum")
                        font.pixelSize: 14
                        color: "#ffffff"
                    }
                    
                    Text {
                        Layout.fillWidth: true
                        text: Math.round(securityOverview.hardeningProgress) + "% complete"
                        font.pixelSize: 12
                        color: "#b0b0b0"
                    }
                }
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
                onClicked: {
                    securityOverview.backClicked()
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
                onClicked: {
                    securityOverview.nextClicked()
                }
            }
        }
    }
    
    // Simulate status updates
    Component.onCompleted: {
        // Simulate firewall status check
        Qt.callLater(function() {
            securityOverview.firewallStatus = "inactive"
        })
        
        // Simulate update check
        var updateTimer = Qt.createQmlObject("import QtQuick 2.15; Timer { interval: 2000; running: true; repeat: false }", securityOverview)
        updateTimer.triggered.connect(function() {
            securityOverview.updateStatus = "updates_available"
        })
        
        // Simulate hardening progress
        securityOverview.hardeningProgress = 30.0
    }
    
    // Accessibility
    Accessible.role: Accessible.Pane
    Accessible.name: "Security Overview"
    Accessible.description: "Overview of current security status"
}

