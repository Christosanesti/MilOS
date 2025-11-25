import QtQuick 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.plasmoid 2.1
import org.kde.plasma.core 2.1 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents
import MilosComponents 1.0
import AlertZone 1.0

PlasmoidItem {
    id: root
    
    // Properties
    property bool compactMode: plasmoid.formFactor === PlasmaCore.Types.Horizontal || plasmoid.formFactor === PlasmaCore.Types.Vertical
    property var alertHistory: []
    property int maxHistoryItems: 20
    property bool historyExpanded: false
    property bool configDialogVisible: false
    
    // Always visible - cannot be hidden
    Plasmoid.hideOnWindowDeactivate: false
    
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
    
    // Compact representation (always-visible alert bar)
    Plasmoid.compactRepresentation: Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
        
        Rectangle {
            id: alertBar
            anchors.fill: parent
            color: "#1a1a1a"
            border.width: 2
            border.color: getSeverityColor()
            
            // Xenon glow based on severity
            Rectangle {
                anchors.fill: parent
                color: "transparent"
                border.width: 2
                border.color: getSeverityColor()
                opacity: getSeverityGlow()
                
                // Pulse animation for medium/high/critical
                SequentialAnimation on opacity {
                    running: getSeverityLevel() >= 2 // Medium and above
                    loops: Animation.Infinite
                    NumberAnimation {
                        from: getSeverityGlow()
                        to: getSeverityGlow() * 1.5
                        duration: getSeverityPulseSpeed()
                        easing.type: Easing.InOutSine
                    }
                    NumberAnimation {
                        from: getSeverityGlow() * 1.5
                        to: getSeverityGlow()
                        duration: getSeverityPulseSpeed()
                        easing.type: Easing.InOutSine
                    }
                }
            }
            
            // Alert text
            Text {
                id: alertText
                anchors.centerIn: parent
                text: getCurrentAlertText()
                color: "#ffffff"
                font.pixelSize: 12
                font.bold: true
                elide: Text.ElideRight
                width: parent.width - 20
            }
            
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    plasmoid.expanded = !plasmoid.expanded
                }
            }
        }
    }
    
    // Full representation (alert bar + history)
    Plasmoid.fullRepresentation: Item {
        Layout.fillWidth: true
        Layout.preferredHeight: historyExpanded ? alertColumn.implicitHeight + 20 : 60
        
        Column {
            id: alertColumn
            anchors.fill: parent
            anchors.margins: 10
            spacing: 10
            
            // Always-visible alert bar
            Rectangle {
                id: fullAlertBar
                width: parent.width
                height: 50
                color: "#1a1a1a"
                border.width: 2
                border.color: getSeverityColor()
                radius: 4
                
                // Xenon glow based on severity
                Rectangle {
                    anchors.fill: parent
                    color: "transparent"
                    border.width: 2
                    border.color: getSeverityColor()
                    radius: parent.radius
                    opacity: getSeverityGlow()
                    
                    // Pulse animation
                    SequentialAnimation on opacity {
                        running: getSeverityLevel() >= 2
                        loops: Animation.Infinite
                        NumberAnimation {
                            from: getSeverityGlow()
                            to: getSeverityGlow() * 1.5
                            duration: getSeverityPulseSpeed()
                            easing.type: Easing.InOutSine
                        }
                        NumberAnimation {
                            from: getSeverityGlow() * 1.5
                            to: getSeverityGlow()
                            duration: getSeverityPulseSpeed()
                            easing.type: Easing.InOutSine
                        }
                    }
                }
                
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10
                    
                    // Alert icon
                    PlasmaCore.IconItem {
                        source: getSeverityIcon()
                        width: 24
                        height: 24
                        color: getSeverityColor()
                    }
                    
                    // Alert text
                    Column {
                        Layout.fillWidth: true
                        spacing: 2
                        
                        Text {
                            text: getCurrentAlertText()
                            color: "#ffffff"
                            font.pixelSize: 14
                            font.bold: true
                            elide: Text.ElideRight
                            width: parent.width
                        }
                        
                        Text {
                            text: getCurrentAlertCategory()
                            color: "#b0b0b0"
                            font.pixelSize: 11
                            elide: Text.ElideRight
                            width: parent.width
                        }
                        visible: getCurrentAlertText() !== ""
                    }
                    
                    // History toggle button
                    PlasmaComponents.Button {
                        text: historyExpanded ? "Hide" : "History"
                        onClicked: {
                            historyExpanded = !historyExpanded
                        }
                    }
                }
            }
            
            // Alert history (collapsible)
            Rectangle {
                id: historyContainer
                width: parent.width
                height: historyExpanded ? Math.min(historyList.contentHeight + 20, 300) : 0
                color: "#0a0a0a"
                border.width: 1
                border.color: "#2a2a2a"
                radius: 4
                clip: true
                visible: historyExpanded
                
                Behavior on height {
                    NumberAnimation {
                        duration: 200
                        easing.type: Easing.OutCubic
                    }
                }
                
                ListView {
                    id: historyList
                    anchors.fill: parent
                    anchors.margins: 10
                    model: alertHistory
                    spacing: 5
                    
                    delegate: Rectangle {
                        id: alertItem
                        width: historyList.width
                        height: 60
                        color: alertMouseArea.containsMouse ? "#2a2a2a" : "#1a1a1a"
                        border.width: 2
                        border.color: getAlertSeverityColor(modelData.severity)
                        radius: 4
                        
                        // Xenon transition effect on click
                        SequentialAnimation {
                            id: navigationAnimation
                            PropertyAnimation {
                                target: alertItem
                                property: "scale"
                                from: 1.0
                                to: 0.95
                                duration: 100
                            }
                            PropertyAnimation {
                                target: alertItem
                                property: "scale"
                                from: 0.95
                                to: 1.0
                                duration: 200
                                easing.type: Easing.OutCubic
                            }
                            PropertyAnimation {
                                target: alertItem
                                property: "opacity"
                                from: 1.0
                                to: 0.3
                                duration: 300
                            }
                        }
                        
                        // Xenon hover glow
                        Rectangle {
                            anchors.fill: parent
                            anchors.margins: -2
                            radius: parent.radius + 2
                            color: "transparent"
                            border.color: getAlertSeverityColor(modelData.severity)
                            border.width: 2
                            opacity: alertMouseArea.containsMouse ? 0.6 : 0
                            
                            Behavior on opacity {
                                NumberAnimation { duration: 200 }
                            }
                        }
                        
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 10
                            spacing: 10
                            
                            PlasmaCore.IconItem {
                                source: getSeverityIconForLevel(modelData.severity)
                                width: 20
                                height: 20
                                color: getAlertSeverityColor(modelData.severity)
                            }
                            
                            Column {
                                Layout.fillWidth: true
                                spacing: 2
                                
                                Text {
                                    text: modelData.message || "Alert"
                                    color: "#ffffff"
                                    font.pixelSize: 12
                                    elide: Text.ElideRight
                                    width: parent.width
                                }
                                
                                Text {
                                    text: modelData.category || "Unknown"
                                    color: "#b0b0b0"
                                    font.pixelSize: 10
                                    elide: Text.ElideRight
                                    width: parent.width
                                }
                                
                                Text {
                                    text: modelData.timestamp || ""
                                    color: "#808080"
                                    font.pixelSize: 9
                                    elide: Text.ElideRight
                                    width: parent.width
                                }
                            }
                        }
                        
                        MouseArea {
                            id: alertMouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                // Xenon transition effect
                                navigationAnimation.start()
                                
                                // Navigate to alert source
                                if (modelData.alertId) {
                                    alertZoneService.navigateToAlertSource(modelData.alertId)
                                } else if (modelData.category) {
                                    // Fallback: navigate by category
                                    var alertId = "alert_" + modelData.category + "_" + Date.now()
                                    alertZoneService.navigateToAlertSource(alertId)
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Helper functions
    function getCurrentAlert() {
        if (alertHistory.length > 0) {
            return alertHistory[0]
        }
        return null
    }
    
    function getCurrentAlertText() {
        var alert = getCurrentAlert()
        if (alert) {
            return alert.message || "Alert"
        }
        return "No alerts"
    }
    
    function getCurrentAlertCategory() {
        var alert = getCurrentAlert()
        if (alert) {
            return alert.category || "Unknown"
        }
        return ""
    }
    
    function getSeverityLevel() {
        var alert = getCurrentAlert()
        if (alert) {
            var severity = alert.severity || "low"
            if (severity === "critical") return 4
            if (severity === "high") return 3
            if (severity === "medium") return 2
            return 1
        }
        return 0
    }
    
    function getSeverityColor() {
        var level = getSeverityLevel()
        if (level === 4) return "#ff4444" // Critical - red
        if (level === 3) return "#ff4444" // High - red
        if (level === 2) return "#ffaa00" // Medium - amber
        return "#00d4ff" // Low - cyan
    }
    
    function getSeverityGlow() {
        var level = getSeverityLevel()
        if (level === 4) return 0.8 // Critical - intense
        if (level === 3) return 0.6 // High - strong
        if (level === 2) return 0.4 // Medium - moderate
        return 0.2 // Low - subtle
    }
    
    function getSeverityPulseSpeed() {
        var level = getSeverityLevel()
        if (level === 4) return 500 // Critical - rapid pulse
        if (level === 3) return 800 // High - fast pulse
        if (level === 2) return 1200 // Medium - slow pulse
        return 2000 // Low - very slow pulse
    }
    
    function getSeverityIcon() {
        var level = getSeverityLevel()
        if (level >= 3) return "security-high"
        if (level === 2) return "security-medium"
        return "security-low"
    }
    
    function getSeverityIconForLevel(severity) {
        if (severity === "critical" || severity === "high") return "security-high"
        if (severity === "medium") return "security-medium"
        return "security-low"
    }
    
    function getAlertSeverityColor(severity) {
        if (severity === "critical" || severity === "high") return "#ff4444"
        if (severity === "medium") return "#ffaa00"
        return "#00d4ff"
    }
    
    // Add alert function (called from D-Bus or other sources)
    function addAlert(severity, category, message) {
        var alert = {
            "severity": severity || "low",
            "category": category || "Unknown",
            "message": message || "Alert",
            "timestamp": new Date().toLocaleString()
        }
        
        alertHistory.unshift(alert)
        
        // Limit to maxHistoryItems
        if (alertHistory.length > maxHistoryItems) {
            alertHistory = alertHistory.slice(0, maxHistoryItems)
        }
        
        // Update plasmoid
        plasmoid.status = severity === "critical" || severity === "high" ? PlasmaCore.Types.ActiveStatus : PlasmaCore.Types.PassiveStatus
    }
    
    // Alert zone service instance
    AlertZoneService {
        id: alertZoneService
        
        onAlertReceived: function(severity, category, message) {
            addAlert(severity, category, message)
        }
        
        onAvailableActionsChanged: {
            // Update alert history with alert IDs from service
            updateAlertHistory()
        }
    }
    
    function updateAlertHistory() {
        var activeAlerts = alertZoneService.getActiveAlerts()
        // Merge active alerts with history, preserving alert IDs
        for (var i = 0; i < activeAlerts.length; i++) {
            var activeAlert = activeAlerts[i]
            var found = false
            for (var j = 0; j < alertHistory.length; j++) {
                if (alertHistory[j].alertId === activeAlert.alertId) {
                    // Update existing alert
                    alertHistory[j] = activeAlert
                    found = true
                    break
                }
            }
            if (!found) {
                // Add new alert
                alertHistory.unshift(activeAlert)
            }
        }
        
        // Limit to maxHistoryItems
        if (alertHistory.length > maxHistoryItems) {
            alertHistory = alertHistory.slice(0, maxHistoryItems)
        }
    }
    
    // Configuration dialog loader
    Loader {
        id: configDialogLoader
        active: configDialogVisible
        source: "ConfigDialog.qml"
        
        onLoaded: {
            item.service = alertZoneService
            item.open()
        }
    }
    
    // Initialize with test alert
    Component.onCompleted: {
        // Add test alerts for demonstration
        addAlert("low", "System", "System operational")
    }
}

