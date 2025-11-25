import QtQuick 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.plasmoid 2.1
import org.kde.plasma.core 2.1 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents
import EmergencyWidget 1.0

PlasmoidItem {
    id: root
    
    // Properties
    property bool compactMode: plasmoid.formFactor === PlasmaCore.Types.Horizontal || plasmoid.formFactor === PlasmaCore.Types.Vertical
    property bool configDialogVisible: false
    
    // Context menu
    Plasmoid.contextualActions: [
        PlasmaComponents.Action {
            text: "Configure Widget"
            icon: "configure"
            onTriggered: {
                configDialogVisible = true
            }
        }
    ]
    property int shutdownConfirmationStep: 0 // 0 = none, 1 = first confirmation, 2 = second confirmation
    
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
                visible: widgetConfig.showNetworkButton
                
                onClicked: {
                    if (networkKillSwitch.requiresConfirmation && widgetConfig.requireConfirmations) {
                        confirmationDialog.actionType = "network"
                        confirmationDialog.message = "Disable all network interfaces? This will cut off all network connectivity."
                        confirmationDialog.show()
                    } else {
                        networkKillSwitch.disableNetwork()
                        auditLogger.logEmergencyAction("network_kill_switch", {"action": "disable"})
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
                visible: widgetConfig.showLockButton
                
                onClicked: {
                    screenLock.lockScreen()
                    auditLogger.logEmergencyAction("screen_lock", {})
                }
            }
            
            EmergencyButton {
                id: secureButton
                actionType: "secure"
                label: secureMode.active ? "Deactivate Secure Mode" : "Secure Mode"
                iconName: "security-high"
                accentColor: secureMode.active ? "#00cc66" : "#ffaa00" // Green if active, Yellow/Orange if inactive
                compact: compactMode
                width: parent.width
                visible: widgetConfig.showSecureModeButton
                
                onClicked: {
                    if (secureMode.active) {
                        secureMode.deactivateSecureMode()
                        auditLogger.logEmergencyAction("secure_mode", {"action": "deactivate"})
                    } else {
                        if (widgetConfig.requireConfirmations) {
                            confirmationDialog.actionType = "secure"
                            confirmationDialog.message = "Activate secure mode? This will disable non-essential services."
                            confirmationDialog.show()
                        } else {
                            secureMode.activateSecureMode()
                            auditLogger.logEmergencyAction("secure_mode", {"action": "activate"})
                        }
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
                visible: widgetConfig.showShutdownButton
                
                onClicked: {
                    if (widgetConfig.requireConfirmations) {
                        // First confirmation dialog
                        shutdownConfirmationStep = 1
                        confirmationDialog.actionType = "shutdown"
                        confirmationDialog.message = "WARNING: Emergency shutdown will immediately power down the system.\n\nThis action cannot be undone. Are you absolutely sure?"
                        confirmationDialog.show()
                    } else {
                        emergencyShutdown.shutdown()
                        auditLogger.logEmergencyAction("emergency_shutdown", {})
                    }
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
                    auditLogger.logEmergencyAction("network_kill_switch", {"action": "disable"})
                    visible = false
                } else if (actionType === "secure") {
                    secureMode.activateSecureMode()
                    auditLogger.logEmergencyAction("secure_mode", {"action": "activate"})
                    visible = false
                } else if (actionType === "shutdown") {
                    // Double confirmation for shutdown
                    if (shutdownConfirmationStep === 1) {
                        // First confirmation passed, show second confirmation
                        shutdownConfirmationStep = 2
                        confirmationDialog.message = "FINAL WARNING: This will immediately shut down the system.\n\nAll unsaved work will be lost. Confirm shutdown?"
                        // Keep dialog visible for second confirmation
                    } else if (shutdownConfirmationStep === 2) {
                        // Second confirmation passed, proceed with shutdown
                        emergencyShutdown.shutdown()
                        auditLogger.logEmergencyAction("emergency_shutdown", {})
                        shutdownConfirmationStep = 0
                        visible = false
                    }
                }
            }
            
            onCancelled: {
                if (actionType === "shutdown" && shutdownConfirmationStep > 0) {
                    // Reset shutdown confirmation step if cancelled
                    shutdownConfirmationStep = 0
                }
                visible = false
            }
        }
    }
    
    // Service instances
    NetworkKillSwitch {
        id: networkKillSwitch
        requiresConfirmation: widgetConfig.requireConfirmations
    }
    
    ScreenLock {
        id: screenLock
    }
    
    SecureMode {
        id: secureMode
    }
    
    EmergencyShutdown {
        id: emergencyShutdown
    }
    
    AuditLogger {
        id: auditLogger
    }
    
    WidgetConfig {
        id: widgetConfig
    }
    
    KeyboardShortcut {
        id: keyboardShortcut
        shortcut: widgetConfig.keyboardShortcut
        
        onActivated: {
            // Xenon flash feedback
            xenonFlash.start()
            // Toggle widget expansion
            plasmoid.expanded = !plasmoid.expanded
        }
    }
    
    StatusIndicators {
        id: statusIndicators
        
        Component.onCompleted: {
            updateAll()
            // Update status every 5 seconds
            statusUpdateTimer.start()
        }
    }
    
    // Xenon flash animation for keyboard shortcut
    SequentialAnimation {
        id: xenonFlash
        PropertyAnimation {
            target: root
            property: "opacity"
            from: 1.0
            to: 0.5
            duration: 100
        }
        PropertyAnimation {
            target: root
            property: "opacity"
            from: 0.5
            to: 1.0
            duration: 100
        }
    }
    
    // Status update timer
    Timer {
        id: statusUpdateTimer
        interval: 5000
        running: false
        repeat: true
        onTriggered: statusIndicators.updateAll()
    }
    
    // Status indicators display
    RowLayout {
        anchors.bottom: buttonColumn.top
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10
        visible: plasmoid.expanded
        
        Text {
            text: "Network: " + statusIndicators.networkStatus
            color: statusIndicators.networkActive ? "#00cc66" : "#ff4444"
            font.pixelSize: 10
        }
        
        Text {
            text: "Secure Mode: " + statusIndicators.secureModeStatus
            color: statusIndicators.secureModeActive ? "#00cc66" : "#ff4444"
            font.pixelSize: 10
        }
    }
    
    // Configuration dialog
    Loader {
        id: configDialogLoader
        active: configDialogVisible
        source: "ConfigDialog.qml"
        
        onLoaded: {
            item.open()
        }
    }
    
    // Service connections
    Connections {
        target: networkKillSwitch
        function onNetworkDisabled() {
            // Xenon success pulse
            networkButton.accentColor = "#00cc66" // Green
            timer1.restart()
        }
    }
    
    Connections {
        target: screenLock
        function onScreenLocked() {
            // Xenon confirmation pulse
            lockButton.accentColor = "#00cc66" // Green
            timer2.restart()
        }
    }
    
    Connections {
        target: secureMode
        function onSecureModeActivated() {
            secureButton.accentColor = "#00cc66" // Green
            timer3.restart()
        }
    }
    
    Connections {
        target: emergencyShutdown
        function onShutdownInitiated() {
            // System will shutdown, no need to update UI
        }
    }
    
    // Timers for resetting button colors
    Timer {
        id: timer1
        interval: 1000
        onTriggered: networkButton.accentColor = "#ff4444"
    }
    
    Timer {
        id: timer2
        interval: 1000
        onTriggered: lockButton.accentColor = "#00d4ff"
    }
    
    Timer {
        id: timer3
        interval: 1000
        onTriggered: secureButton.accentColor = "#ffaa00"
    }
}

