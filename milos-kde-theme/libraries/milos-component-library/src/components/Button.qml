import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: button
    
    // Properties
    property string text: ""
    property string variant: "primary" // primary, secondary, tertiary, icon, toggle
    property string state: "default" // default, hover, active, disabled, loading, success, error
    property bool checked: false // for toggle variant
    property bool enabled: true
    property alias icon: iconItem.source
    
    // Xenon color properties
    property color xenonColor: "#00d4ff"
    property color xenonHoverColor: "#00ffff"
    property color xenonSuccessColor: "#00cc66"
    property color xenonErrorColor: "#ff4444"
    
    // Size properties
    property int buttonHeight: 48 // Minimum 48px for accessibility
    property int buttonWidth: variant === "icon" ? buttonHeight : implicitWidth
    
    // Signals
    signal clicked()
    signal pressed()
    signal released()
    
    // Dimensions
    width: buttonWidth
    height: buttonHeight
    implicitWidth: variant === "icon" ? buttonHeight : (text.length > 0 ? textMetrics.width + 32 : 100)
    implicitHeight: buttonHeight
    
    // Base styling
    color: {
        if (!enabled) return "#1a1a1a"
        if (variant === "primary") return "#0a0a0a"
        if (variant === "secondary") return "#1a1a1a"
        if (variant === "tertiary") return "transparent"
        if (variant === "icon") return "transparent"
        if (variant === "toggle") return checked ? "#1a1a1a" : "#0a0a0a"
        return "#0a0a0a"
    }
    
    radius: variant === "icon" ? buttonHeight / 2 : 8
    border.width: 0
    
    // Xenon glow effect (base)
    Rectangle {
        id: xenonGlow
        anchors.fill: parent
        radius: parent.radius
        color: {
            if (!button.enabled) return button.xenonColor
            if (button.state === "error") return button.xenonErrorColor
            if (button.state === "success") return button.xenonSuccessColor
            return button.xenonColor
        }
        opacity: {
            if (!button.enabled) return 0.1
            if (button.state === "hover") return 0.8 * 1.5 // 150% intensity
            if (button.state === "active") return 0.8
            if (button.state === "loading") return 0.6
            if (button.state === "success") return 0.8
            if (button.state === "error") return 0.8
            return 0.4 // Default 40% opacity
        }
        
        // Glow pulse animation (Animation 1: Xenon Glow Pulse)
        SequentialAnimation on opacity {
            id: pulseAnimation
            running: button.state === "active" && button.enabled
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
        
        // Success celebration animation (Animation 5: Xenon Success Celebration)
        SequentialAnimation on opacity {
            id: successAnimation
            running: button.state === "success"
            NumberAnimation {
                from: 0.8
                to: 1.0
                duration: 200
                easing.type: Easing.EaseOut
            }
            NumberAnimation {
                from: 1.0
                to: 0.6
                duration: 200
                easing.type: Easing.EaseIn
            }
            NumberAnimation {
                from: 0.6
                to: 1.0
                duration: 200
                easing.type: Easing.EaseOut
            }
            NumberAnimation {
                from: 1.0
                to: 0.6
                duration: 200
                easing.type: Easing.EaseIn
            }
            NumberAnimation {
                from: 0.6
                to: 1.0
                duration: 200
                easing.type: Easing.EaseOut
            }
            NumberAnimation {
                from: 1.0
                to: 0.8
                duration: 300
                easing.type: Easing.EaseIn
            }
        }
        
        // Error alert pulse (Animation 4: Xenon Alert Pulse)
        SequentialAnimation on opacity {
            id: errorAnimation
            running: button.state === "error"
            loops: Animation.Infinite
            NumberAnimation {
                from: 0.8
                to: 1.0
                duration: 500
                easing.type: Easing.InOutSine
            }
            NumberAnimation {
                from: 1.0
                to: 0.8
                duration: 500
                easing.type: Easing.InOutSine
            }
        }
        
        Behavior on opacity {
            NumberAnimation { duration: 200; easing.type: Easing.EaseOut }
        }
    }
    
    // Processing scan animation (Animation 2: Xenon Border Scan)
    Rectangle {
        id: scanLine
        width: parent.width * 0.3
        height: 2
        x: button.state === "loading" ? -width : parent.width
        y: parent.height / 2 - height / 2
        color: button.xenonColor
        visible: button.state === "loading"
        
        SequentialAnimation on x {
            running: button.state === "loading"
            loops: Animation.Infinite
            NumberAnimation {
                from: -scanLine.width
                to: button.width + scanLine.width
                duration: 1500
                easing.type: Easing.Linear
            }
        }
    }
    
    // Border glow for keyboard focus
    Rectangle {
        id: focusBorder
        anchors.fill: parent
        radius: parent.radius
        color: "transparent"
        border.color: button.xenonColor
        border.width: 2
        opacity: button.activeFocus ? 1.0 : 0.0
        
        Behavior on opacity {
            NumberAnimation { duration: 200; easing.type: Easing.EaseOut }
        }
    }
    
    // Content
    Row {
        anchors.centerIn: parent
        spacing: 8
        visible: variant !== "icon"
        
        // Icon (if provided)
        Image {
            id: iconItem
            width: 20
            height: 20
            visible: source !== ""
            anchors.verticalCenter: parent.verticalCenter
        }
        
        // Text
        Text {
            id: textItem
            text: button.text
            font.pixelSize: {
                if (variant === "primary") return 16
                if (variant === "secondary") return 14
                if (variant === "tertiary") return 14
                return 16
            }
            font.weight: variant === "primary" ? Font.Bold : Font.Normal
            color: {
                if (!button.enabled) return "#666666"
                if (button.state === "success") return button.xenonSuccessColor
                if (button.state === "error") return button.xenonErrorColor
                return "#ffffff"
            }
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    
    // Icon-only variant
    Image {
        id: iconOnlyItem
        anchors.centerIn: parent
        width: 24
        height: 24
        visible: variant === "icon"
        source: button.icon
    }
    
    // Toggle indicator
    Rectangle {
        id: toggleIndicator
        visible: variant === "toggle"
        width: 12
        height: 12
        radius: 6
        x: button.checked ? parent.width - width - 8 : 8
        y: parent.height / 2 - height / 2
        color: button.checked ? button.xenonColor : "#666666"
        
        Behavior on x {
            NumberAnimation { duration: 200; easing.type: Easing.EaseOut }
        }
    }
    
    // Mouse area
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        enabled: button.enabled
        hoverEnabled: true
        cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
        
        onEntered: {
            if (button.enabled) button.state = "hover"
        }
        onExited: {
            if (button.enabled) button.state = "default"
        }
        onPressed: {
            if (button.enabled) {
                button.state = "active"
                button.pressed()
            }
        }
        onReleased: {
            if (button.enabled) {
                button.state = "hover"
                button.released()
                button.clicked()
                if (variant === "toggle") {
                    button.checked = !button.checked
                }
            }
        }
    }
    
    // Keyboard handling
    Keys.onPressed: {
        if (event.key === Qt.Key_Return || event.key === Qt.Key_Space) {
            if (button.enabled) {
                button.clicked()
                if (variant === "toggle") {
                    button.checked = !button.checked
                }
            }
            event.accepted = true
        }
    }
    
    // Text metrics for width calculation
    TextMetrics {
        id: textMetrics
        font: textItem.font
        text: button.text
    }
    
    // Accessibility
    Accessible.role: Accessible.Button
    Accessible.name: button.text || (variant === "icon" ? "Icon button" : "Button")
    Accessible.description: variant + " button"
    Accessible.onPressAction: {
        if (button.enabled) {
            button.clicked()
            if (variant === "toggle") {
                button.checked = !button.checked
            }
        }
    }
}

