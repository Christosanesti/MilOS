import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: securityPreferencesScreen
    
    color: "#0a0a0a"
    
    property bool autoUpdate: true
    property int screenLockTimeout: 5
    property bool auditLogging: true
    property bool networkSecurity: true
    
    signal backClicked()
    signal nextClicked()
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 32
        spacing: 24
        
        // Title
        Text {
            Layout.fillWidth: true
            text: "Security Preferences"
            font.pixelSize: 28
            font.weight: Font.Bold
            color: "#ffffff"
        }
        
        Text {
            Layout.fillWidth: true
            text: "Configure your security preferences"
            font.pixelSize: 16
            color: "#cccccc"
        }
        
        // Preferences cards
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            
            ColumnLayout {
                width: securityPreferencesScreen.width - 64
                spacing: 16
                
                // Auto-update settings
                Card {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 120
                    variant: "dashboard"
                    title: "Auto-Update Settings"
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 24
                        spacing: 16
                        
                        Text {
                            Layout.fillWidth: true
                            text: "Automatically install security updates"
                            font.pixelSize: 14
                            color: "#ffffff"
                        }
                        
                        // Toggle (simplified)
                        Rectangle {
                            width: 48
                            height: 24
                            radius: 12
                            color: securityPreferencesScreen.autoUpdate ? "#00d4ff" : "#2a2a2a"
                            border.color: "#00d4ff"
                            border.width: 1
                            
                            Rectangle {
                                anchors.verticalCenter: parent.verticalCenter
                                x: securityPreferencesScreen.autoUpdate ? parent.width - width - 2 : 2
                                width: 20
                                height: 20
                                radius: 10
                                color: "#ffffff"
                                
                                Behavior on x {
                                    NumberAnimation { duration: 200 }
                                }
                            }
                            
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    securityPreferencesScreen.autoUpdate = !securityPreferencesScreen.autoUpdate
                                }
                            }
                        }
                    }
                }
                
                // Screen lock timeout
                Card {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 120
                    variant: "dashboard"
                    title: "Screen Lock Timeout"
                    
                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 24
                        spacing: 12
                        
                        RowLayout {
                            Layout.fillWidth: true
                            
                            Text {
                                Layout.fillWidth: true
                                text: "Lock screen after inactivity (minutes)"
                                font.pixelSize: 14
                                color: "#ffffff"
                            }
                            
                            DropdownMenu {
                                Layout.preferredWidth: 150
                                Layout.preferredHeight: 40
                                variant: "standard"
                                items: [
                                    {text: "1 minute", value: "1"},
                                    {text: "5 minutes", value: "5"},
                                    {text: "10 minutes", value: "10"},
                                    {text: "30 minutes", value: "30"},
                                    {text: "Never", value: "0"}
                                ]
                                selectedValue: securityPreferencesScreen.screenLockTimeout.toString()
                            }
                        }
                    }
                }
                
                // Audit logging
                Card {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 120
                    variant: "dashboard"
                    title: "Audit Logging"
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 24
                        spacing: 16
                        
                        Text {
                            Layout.fillWidth: true
                            text: "Enable comprehensive audit logging"
                            font.pixelSize: 14
                            color: "#ffffff"
                        }
                        
                        Rectangle {
                            width: 48
                            height: 24
                            radius: 12
                            color: securityPreferencesScreen.auditLogging ? "#00d4ff" : "#2a2a2a"
                            border.color: "#00d4ff"
                            border.width: 1
                            
                            Rectangle {
                                anchors.verticalCenter: parent.verticalCenter
                                x: securityPreferencesScreen.auditLogging ? parent.width - width - 2 : 2
                                width: 20
                                height: 20
                                radius: 10
                                color: "#ffffff"
                                
                                Behavior on x {
                                    NumberAnimation { duration: 200 }
                                }
                            }
                            
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    securityPreferencesScreen.auditLogging = !securityPreferencesScreen.auditLogging
                                }
                            }
                        }
                    }
                }
                
                // Network security
                Card {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 120
                    variant: "dashboard"
                    title: "Network Security"
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 24
                        spacing: 16
                        
                        Text {
                            Layout.fillWidth: true
                            text: "Enable enhanced network security features"
                            font.pixelSize: 14
                            color: "#ffffff"
                        }
                        
                        Rectangle {
                            width: 48
                            height: 24
                            radius: 12
                            color: securityPreferencesScreen.networkSecurity ? "#00d4ff" : "#2a2a2a"
                            border.color: "#00d4ff"
                            border.width: 1
                            
                            Rectangle {
                                anchors.verticalCenter: parent.verticalCenter
                                x: securityPreferencesScreen.networkSecurity ? parent.width - width - 2 : 2
                                width: 20
                                height: 20
                                radius: 10
                                color: "#ffffff"
                                
                                Behavior on x {
                                    NumberAnimation { duration: 200 }
                                }
                            }
                            
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    securityPreferencesScreen.networkSecurity = !securityPreferencesScreen.networkSecurity
                                }
                            }
                        }
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
                    securityPreferencesScreen.backClicked()
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
                    securityPreferencesScreen.nextClicked()
                }
            }
        }
    }
    
    // Accessibility
    Accessible.role: Accessible.Pane
    Accessible.name: "Security Preferences"
    Accessible.description: "Configure security preferences for your system"
}

