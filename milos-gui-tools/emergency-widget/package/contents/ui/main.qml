import QtQuick 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.plasmoid 2.1
import org.kde.plasma.core 2.1 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents

PlasmoidItem {
    id: root
    
    // Properties
    property bool compactMode: plasmoid.formFactor === PlasmaCore.Types.Horizontal || plasmoid.formFactor === PlasmaCore.Types.Vertical
    
    // Compact representation (icon-only)
    Plasmoid.compactRepresentation: Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
        
        PlasmaCore.IconItem {
            anchors.centerIn: parent
            source: "security-high"
            width: Math.min(parent.width, parent.height)
            height: width
            color: "#00d4ff" // Xenon cyan
            
            // Xenon pulse animation
            SequentialAnimation on opacity {
                running: true
                loops: Animation.Infinite
                NumberAnimation {
                    from: 0.7
                    to: 1.0
                    duration: 2000
                    easing.type: Easing.InOutSine
                }
                NumberAnimation {
                    from: 1.0
                    to: 0.7
                    duration: 2000
                    easing.type: Easing.InOutSine
                }
            }
        }
        
        MouseArea {
            anchors.fill: parent
            onClicked: {
                plasmoid.expanded = !plasmoid.expanded
            }
        }
    }
    
    // Full representation (buttons)
    Plasmoid.fullRepresentation: Item {
        Layout.fillWidth: true
        Layout.preferredHeight: buttonColumn.implicitHeight + 20
        
        Column {
            id: buttonColumn
            anchors.fill: parent
            anchors.margins: 10
            spacing: 10
            
            // Title
            Text {
                text: "Emergency Actions"
                color: "#ffffff"
                font.pixelSize: 16
                font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
            }
            
            // Emergency buttons
            EmergencyButton {
                id: networkButton
                actionType: "network"
                label: "Disable Network"
                iconName: "network-disconnect"
                accentColor: "#ff4444" // Red
                compact: compactMode
                width: parent.width
                
                onClicked: {
                    if (networkKillSwitch.requiresConfirmation) {
                        confirmationDialog.actionType = "network"
                        confirmationDialog.message = "Disable all network interfaces? This will cut off all network connectivity."
                        confirmationDialog.show()
                    } else {
                        networkKillSwitch.disableNetwork()
                    }
                }
            }
            
            EmergencyButton {
                id: lockButton
                actionType: "lock"
                label: "Lock System"
                iconName: "system-lock-screen"
                accentColor: "#00d4ff" // Xenon cyan
                compact: compactMode
                width: parent.width
                
                onClicked: {
                    screenLock.lockScreen()
                }
            }
            
            EmergencyButton {
                id: secureButton
                actionType: "secure"
                label: "Secure Mode"
                iconName: "security-high"
                accentColor: "#ffaa00" // Yellow/Orange
                compact: compactMode
                width: parent.width
                
                onClicked: {
                    if (secureMode.active) {
                        secureMode.deactivateSecureMode()
                    } else {
                        confirmationDialog.actionType = "secure"
                        confirmationDialog.message = "Activate secure mode? This will disable non-essential services."
                        confirmationDialog.show()
                    }
                }
            }
            
            EmergencyButton {
                id: shutdownButton
                actionType: "shutdown"
                label: "Emergency Shutdown"
                iconName: "system-shutdown"
                accentColor: "#ff4444" // Red
                compact: compactMode
                width: parent.width
                
                onClicked: {
                    confirmationDialog.actionType = "shutdown"
                    confirmationDialog.message = "Emergency shutdown? This will immediately shut down the system."
                    confirmationDialog.show()
                }
            }
        }
        
        // Confirmation dialog
        ConfirmationDialog {
            id: confirmationDialog
            anchors.fill: parent
            z: 1000
            
            onConfirmed: {
                if (actionType === "network") {
                    networkKillSwitch.disableNetwork()
                } else if (actionType === "secure") {
                    secureMode.activateSecureMode()
                } else if (actionType === "shutdown") {
                    emergencyShutdown.shutdown()
                }
                visible = false
            }
            
            onCancelled: {
                visible = false
            }
        }
    }
    
    // Service connections
    Connections {
        target: networkKillSwitch
        function onNetworkDisabled() {
            // Xenon success pulse
            networkButton.accentColor = "#00cc66" // Green
            setTimeout(function() {
                networkButton.accentColor = "#ff4444"
            }, 1000)
        }
    }
    
    Connections {
        target: screenLock
        function onScreenLocked() {
            // Xenon confirmation pulse
            lockButton.accentColor = "#00cc66" // Green
            setTimeout(function() {
                lockButton.accentColor = "#00d4ff"
            }, 1000)
        }
    }
    
    Connections {
        target: secureMode
        function onSecureModeActivated() {
            secureButton.accentColor = "#00cc66" // Green
            setTimeout(function() {
                secureButton.accentColor = "#ffaa00"
            }, 1000)
        }
    }
    
    Connections {
        target: emergencyShutdown
        function onShutdownInitiated() {
            // System will shutdown, no need to update UI
        }
    }
}

