import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: statusIndicator
    
    // Properties
    property string variant: "badge" // badge, icon, light, card, widget
    property string state: "active" // active, inactive, warning, critical, processing
    property string text: ""
    property string icon: ""
    property color xenonColor: "#00cc66" // Green for active
    property color xenonWarningColor: "#ffaa00" // Amber for warning
    property color xenonCriticalColor: "#ff4444" // Red for critical
    property color xenonProcessingColor: "#00d4ff" // Blue for processing
    
    // Dimensions
    width: {
        if (variant === "badge") return 80
        if (variant === "icon") return 32
        if (variant === "light") return 16
        if (variant === "card") return 200
        if (variant === "widget") return 150
        return 80
    }
    height: {
        if (variant === "badge") return 32
        if (variant === "icon") return 32
        if (variant === "light") return 16
        if (variant === "card") return 100
        if (variant === "widget") return 60
        return 32
    }
    implicitWidth: width
    implicitHeight: height
    
    // Base styling
    color: "#1a1a1a"
    radius: variant === "light" ? width / 2 : 8
    border.width: variant === "card" ? 2 : 0
    border.color: getXenonColor()
    
    // Get xenon color based on state
    function getXenonColor() {
        if (state === "active") return xenonColor
        if (state === "warning") return xenonWarningColor
        if (state === "critical") return xenonCriticalColor
        if (state === "processing") return xenonProcessingColor
        return "#666666" // Inactive
    }
    
    // Xenon glow effect
    Rectangle {
        id: xenonGlow
        anchors.fill: parent
        radius: parent.radius
        color: getXenonColor()
        opacity: {
            if (state === "inactive") return 0.1
            if (state === "active") return 0.6
            if (state === "warning") return 0.5
            if (state === "critical") return 0.8
            if (state === "processing") return 0.4
            return 0.3
        }
        
        // Active glow pulse (Animation 1: Xenon Glow Pulse)
        SequentialAnimation on opacity {
            id: activePulseAnimation
            running: state === "active"
            loops: Animation.Infinite
            NumberAnimation {
                from: 0.4
                to: 0.8
                duration: 1000
                easing.type: Easing.InOutSine
            }
            NumberAnimation {
                from: 0.8
                to: 0.4
                duration: 1000
                easing.type: Easing.InOutSine
            }
        }
        
        // Warning glow pulse (slow)
        SequentialAnimation on opacity {
            id: warningPulseAnimation
            running: state === "warning"
            loops: Animation.Infinite
            NumberAnimation {
                from: 0.3
                to: 0.7
                duration: 1500
                easing.type: Easing.InOutSine
            }
            NumberAnimation {
                from: 0.7
                to: 0.3
                duration: 1500
                easing.type: Easing.InOutSine
            }
        }
        
        // Critical glow pulse (Animation 4: Xenon Alert Pulse)
        SequentialAnimation on opacity {
            id: criticalPulseAnimation
            running: state === "critical"
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
    }
    
    // Processing scan animation (Animation 2: Xenon Border Scan)
    Rectangle {
        id: scanLine
        width: parent.width * 0.4
        height: 2
        x: state === "processing" ? -width : parent.width
        y: parent.height / 2 - height / 2
        color: xenonProcessingColor
        visible: state === "processing"
        radius: 1
        
        SequentialAnimation on x {
            running: state === "processing"
            loops: Animation.Infinite
            NumberAnimation {
                from: -scanLine.width
                to: statusIndicator.width + scanLine.width
                duration: 2000
                easing.type: Easing.Linear
            }
        }
    }
    
    // Content
    Row {
        anchors.centerIn: parent
        spacing: 8
        visible: variant !== "light"
        
        // Icon
        Text {
            id: iconText
            text: statusIndicator.icon
            font.pixelSize: variant === "icon" ? 20 : 16
            color: getXenonColor()
            visible: statusIndicator.icon.length > 0
            anchors.verticalCenter: parent.verticalCenter
        }
        
        // Text
        Text {
            id: statusText
            text: statusIndicator.text
            font.pixelSize: {
                if (variant === "card") return 18
                if (variant === "widget") return 16
                return 14
            }
            font.weight: variant === "card" ? Font.Bold : Font.Normal
            color: {
                if (state === "inactive") return "#999999"
                return "#ffffff"
            }
            anchors.verticalCenter: parent.verticalCenter
            visible: statusIndicator.text.length > 0
        }
    }
    
    // Status light variant (circular indicator)
    Rectangle {
        id: statusLight
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        radius: width / 2
        color: getXenonColor()
        visible: variant === "light"
        opacity: xenonGlow.opacity
        
        // Pulse animation for light
        SequentialAnimation on opacity {
            running: variant === "light" && state !== "inactive"
            loops: Animation.Infinite
            NumberAnimation {
                from: 0.4
                to: 1.0
                duration: state === "critical" ? 500 : (state === "warning" ? 1500 : 1000)
                easing.type: Easing.InOutSine
            }
            NumberAnimation {
                from: 1.0
                to: 0.4
                duration: state === "critical" ? 500 : (state === "warning" ? 1500 : 1000)
                easing.type: Easing.InOutSine
            }
        }
    }
    
    // Card variant - additional content area
    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8
        visible: variant === "card"
        
        Text {
            width: parent.width
            text: statusIndicator.text
            font.pixelSize: 18
            font.weight: Font.Bold
            color: "#ffffff"
            wrapMode: Text.WordWrap
        }
        
        Text {
            width: parent.width
            text: {
                if (state === "active") return "System Secure"
                if (state === "warning") return "Caution Required"
                if (state === "critical") return "Immediate Action Required"
                if (state === "processing") return "Operation in Progress"
                return "System Inactive"
            }
            font.pixelSize: 12
            color: "#cccccc"
            wrapMode: Text.WordWrap
        }
    }
    
    // Widget variant - compact display
    Row {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8
        visible: variant === "widget"
        
        Rectangle {
            width: 12
            height: 12
            radius: 6
            color: getXenonColor()
            anchors.verticalCenter: parent.verticalCenter
            
            SequentialAnimation on opacity {
                running: state !== "inactive"
                loops: Animation.Infinite
                NumberAnimation {
                    from: 0.5
                    to: 1.0
                    duration: 1000
                    easing.type: Easing.InOutSine
                }
                NumberAnimation {
                    from: 1.0
                    to: 0.5
                    duration: 1000
                    easing.type: Easing.InOutSine
                }
            }
        }
        
        Text {
            width: parent.width - 28
            text: statusIndicator.text
            font.pixelSize: 14
            color: "#ffffff"
            anchors.verticalCenter: parent.verticalCenter
            elide: Text.ElideRight
        }
    }
    
    // Accessibility
    Accessible.role: Accessible.Indicator
    Accessible.name: statusIndicator.text || getStateText()
    Accessible.description: getStateDescription()
    
    function getStateText() {
        if (state === "active") return "Active"
        if (state === "inactive") return "Inactive"
        if (state === "warning") return "Warning"
        if (state === "critical") return "Critical"
        if (state === "processing") return "Processing"
        return "Unknown"
    }
    
    function getStateDescription() {
        if (state === "active") return "System is secure and operational"
        if (state === "inactive") return "System is inactive"
        if (state === "warning") return "Warning: Caution required"
        if (state === "critical") return "Critical: Immediate action required"
        if (state === "processing") return "Operation in progress"
        return ""
    }
}

