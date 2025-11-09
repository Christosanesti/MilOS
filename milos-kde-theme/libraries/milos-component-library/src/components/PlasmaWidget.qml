import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: plasmaWidget
    
    // Properties
    property string variant: "security-posture" // security-posture, network-monitor, encryption-status, quick-actions, emergency-actions
    property string state: "default" // default, active-alert, hover, configuring
    property string status: "secure" // secure, warning, critical
    property string title: ""
    property string subtitle: ""
    property bool expanded: false
    property color xenonColor: "#00d4ff"
    property color xenonSuccessColor: "#00cc66"
    property color xenonWarningColor: "#ffaa00"
    property color xenonErrorColor: "#ff4444"
    
    // Dimensions
    width: expanded ? 300 : 200
    height: expanded ? implicitExpandedHeight : 60
    implicitExpandedHeight: {
        var baseHeight = 60
        var contentHeight = contentItem.childrenRect.height
        return baseHeight + contentHeight + 20
    }
    
    // Base styling
    color: "#1a1a1a"
    radius: 8
    border.width: 1
    border.color: getStatusColor()
    
    function getStatusColor() {
        if (status === "critical") return xenonErrorColor
        if (status === "warning") return xenonWarningColor
        return xenonSuccessColor
    }
    
    // Xenon border glow
    Rectangle {
        anchors.fill: parent
        radius: parent.radius
        color: "transparent"
        border.color: getStatusColor()
        border.width: 1
        opacity: getGlowOpacity()
        
        // Alert pulse (Animation 4: Xenon Alert Pulse)
        SequentialAnimation on opacity {
            id: alertPulse
            running: state === "active-alert" && status === "critical"
            loops: Animation.Infinite
            NumberAnimation {
                from: 0.6
                to: 1.0
                duration: 500
                easing.type: Easing.InOutSine
            }
            NumberAnimation {
                from: 1.0
                to: 0.6
                duration: 500
                easing.type: Easing.InOutSine
            }
        }
        
        // Status indicator pulse (Animation 1: Xenon Glow Pulse)
        SequentialAnimation on opacity {
            id: statusPulse
            running: state === "default" && status === "secure"
            loops: Animation.Infinite
            NumberAnimation {
                from: 0.4
                to: 0.7
                duration: 1000
                easing.type: Easing.InOutSine
            }
            NumberAnimation {
                from: 0.7
                to: 0.4
                duration: 1000
                easing.type: Easing.InOutSine
            }
        }
        
        Behavior on opacity {
            NumberAnimation { duration: 200; easing.type: Easing.EaseOut }
        }
    }
    
    function getGlowOpacity() {
        if (state === "active-alert") return 0.8
        if (state === "hover") return 0.6
        if (state === "configuring") return 0.5
        return 0.4
    }
    
    // Hover expansion (Animation 10: Xenon Expansion)
    Behavior on width {
        enabled: true
        NumberAnimation {
            duration: 300
            easing.type: Easing.InOutSine
        }
    }
    
    Behavior on height {
        enabled: true
        NumberAnimation {
            duration: 300
            easing.type: Easing.InOutSine
        }
    }
    
    // Content
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8
        
        // Header row
        RowLayout {
            Layout.fillWidth: true
            spacing: 12
            
            // Status indicator
            Rectangle {
                width: 12
                height: 12
                radius: 6
                color: getStatusColor()
                
                SequentialAnimation on opacity {
                    running: state === "default" || state === "active-alert"
                    loops: Animation.Infinite
                    NumberAnimation {
                        from: 0.6
                        to: 1.0
                        duration: 1000
                        easing.type: Easing.InOutSine
                    }
                    NumberAnimation {
                        from: 1.0
                        to: 0.6
                        duration: 1000
                        easing.type: Easing.InOutSine
                    }
                }
            }
            
            // Title
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2
                
                Text {
                    Layout.fillWidth: true
                    text: plasmaWidget.title || getDefaultTitle()
                    font.pixelSize: 14
                    font.weight: Font.Bold
                    color: "#ffffff"
                    elide: Text.ElideRight
                }
                
                Text {
                    Layout.fillWidth: true
                    text: plasmaWidget.subtitle || getDefaultSubtitle()
                    font.pixelSize: 11
                    color: "#cccccc"
                    visible: subtitle.length > 0 || expanded
                    elide: Text.ElideRight
                }
            }
            
            // Expand indicator
            Text {
                text: expanded ? "▼" : "▶"
                font.pixelSize: 10
                color: xenonColor
                visible: variant === "security-posture" || variant === "network-monitor"
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        
        // Content area (expanded)
        Item {
            id: contentItem
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: expanded
            
            // Security Posture Widget content
            Column {
                anchors.fill: parent
                spacing: 8
                visible: variant === "security-posture"
                
                Text {
                    width: parent.width
                    text: "Overall Security Status: " + status.toUpperCase()
                    font.pixelSize: 12
                    color: "#ffffff"
                }
                
                Text {
                    width: parent.width
                    text: "All systems operational"
                    font.pixelSize: 11
                    color: "#cccccc"
                    visible: status === "secure"
                }
            }
            
            // Network Monitor Widget content
            Column {
                anchors.fill: parent
                spacing: 8
                visible: variant === "network-monitor"
                
                Text {
                    width: parent.width
                    text: "Active Connections: 3"
                    font.pixelSize: 12
                    color: "#ffffff"
                }
                
                Text {
                    width: parent.width
                    text: "Bandwidth: 1.2 MB/s"
                    font.pixelSize: 11
                    color: "#cccccc"
                }
            }
            
            // Encryption Status Widget content
            Column {
                anchors.fill: parent
                spacing: 8
                visible: variant === "encryption-status"
                
                Text {
                    width: parent.width
                    text: "Active Operations: 2"
                    font.pixelSize: 12
                    color: "#ffffff"
                }
                
                Text {
                    width: parent.width
                    text: "Progress: 45%"
                    font.pixelSize: 11
                    color: "#cccccc"
                }
            }
            
            // Quick Actions Widget content
            Grid {
                anchors.fill: parent
                columns: 2
                spacing: 8
                visible: variant === "quick-actions"
                
                Repeater {
                    model: ["Encrypt", "Decrypt", "Backup", "Monitor"]
                    
                    Rectangle {
                        width: (parent.width - 8) / 2
                        height: 32
                        radius: 6
                        color: mouseArea.containsMouse ? xenonColor : "#2a2a2a"
                        border.color: xenonColor
                        border.width: 1
                        
                        Text {
                            anchors.centerIn: parent
                            text: modelData
                            font.pixelSize: 11
                            color: mouseArea.containsMouse ? "#0a0a0a" : "#ffffff"
                        }
                        
                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                actionTriggered(modelData)
                            }
                        }
                    }
                }
            }
            
            // Emergency Actions Widget content
            Column {
                anchors.fill: parent
                spacing: 8
                visible: variant === "emergency-actions"
                
                Rectangle {
                    width: parent.width
                    height: 36
                    radius: 6
                    color: mouseAreaEmergency.containsMouse ? xenonErrorColor : "#2a2a2a"
                    border.color: xenonErrorColor
                    border.width: 2
                    
                    Text {
                        anchors.centerIn: parent
                        text: "EMERGENCY SHUTDOWN"
                        font.pixelSize: 12
                        font.weight: Font.Bold
                        color: mouseAreaEmergency.containsMouse ? "#0a0a0a" : xenonErrorColor
                    }
                    
                    MouseArea {
                        id: mouseAreaEmergency
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            emergencyActionTriggered()
                        }
                    }
                }
            }
        }
    }
    
    function getDefaultTitle() {
        if (variant === "security-posture") return "Security Status"
        if (variant === "network-monitor") return "Network Monitor"
        if (variant === "encryption-status") return "Encryption"
        if (variant === "quick-actions") return "Quick Actions"
        if (variant === "emergency-actions") return "Emergency"
        return "Widget"
    }
    
    function getDefaultSubtitle() {
        if (variant === "security-posture") return status === "secure" ? "All systems secure" : "Action required"
        if (variant === "network-monitor") return "3 active connections"
        if (variant === "encryption-status") return "2 operations active"
        return ""
    }
    
    // Mouse area for hover expansion
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        
        onEntered: {
            if (state === "default") {
                state = "hover"
            }
            if (variant === "security-posture" || variant === "network-monitor") {
                expanded = true
            }
        }
        onExited: {
            if (state === "hover") {
                state = "default"
            }
            if (variant === "security-posture" || variant === "network-monitor") {
                expanded = false
            }
        }
        onClicked: {
            if (variant === "quick-actions" || variant === "emergency-actions") {
                expanded = !expanded
            }
        }
    }
    
    // Keyboard handling
    Keys.onPressed: {
        if (event.key === Qt.Key_Return || event.key === Qt.Key_Space) {
            expanded = !expanded
            event.accepted = true
        }
    }
    
    // Signals
    signal actionTriggered(string action)
    signal emergencyActionTriggered()
    
    // Accessibility
    Accessible.role: Accessible.Pane
    Accessible.name: title || getDefaultTitle()
    Accessible.description: subtitle || getDefaultSubtitle()
    Accessible.onPressAction: {
        expanded = !expanded
    }
}

