import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: notification
    
    // Properties
    property string variant: "info" // success, error, warning, info, security, toast
    property string state: "displaying" // default, displaying, closing, dismissed
    property string title: ""
    property string message: ""
    property bool dismissible: true
    property bool requiresAcknowledgment: false // For critical security alerts
    property int duration: {
        if (variant === "info") return 3000
        if (variant === "warning") return 5000
        if (variant === "error") return -1 // Persistent
        if (variant === "security") return -1 // Persistent until acknowledged
        if (variant === "success") return 3000
        if (variant === "toast") return 2000
        return 3000
    }
    
    // Xenon colors
    property color xenonSuccessColor: "#00cc66"
    property color xenonErrorColor: "#ff4444"
    property color xenonWarningColor: "#ffaa00"
    property color xenonInfoColor: "#00d4ff"
    
    // Dimensions
    width: variant === "toast" ? 300 : 400
    height: variant === "toast" ? 60 : implicitHeight
    implicitHeight: {
        var baseHeight = title.length > 0 ? 60 : 40
        var messageHeight = message.length > 0 ? (message.split('\n').length * 20) : 0
        return baseHeight + messageHeight + (requiresAcknowledgment ? 40 : 0) + 20
    }
    
    // Base styling
    color: "#1a1a1a"
    radius: 8
    border.width: 2
    border.color: getXenonColor()
    
    function getXenonColor() {
        if (variant === "success") return xenonSuccessColor
        if (variant === "error") return xenonErrorColor
        if (variant === "warning") return xenonWarningColor
        if (variant === "security") return xenonErrorColor
        return xenonInfoColor
    }
    
    // Xenon glow effect
    Rectangle {
        anchors.fill: parent
        radius: parent.radius
        color: getXenonColor()
        opacity: {
            if (state === "closing") return 0.0
            if (variant === "success") return 0.3
            if (variant === "error") return 0.5
            if (variant === "warning") return 0.4
            if (variant === "security") return 0.6
            return 0.2
        }
        
        // Success celebration pulse (Animation 5)
        SequentialAnimation on opacity {
            id: successPulse
            running: variant === "success" && state === "displaying"
            NumberAnimation {
                from: 0.2
                to: 0.5
                duration: 200
                easing.type: Easing.EaseOut
            }
            NumberAnimation {
                from: 0.5
                to: 0.3
                duration: 200
                easing.type: Easing.EaseIn
            }
            NumberAnimation {
                from: 0.3
                to: 0.5
                duration: 200
                easing.type: Easing.EaseOut
            }
            NumberAnimation {
                from: 0.5
                to: 0.3
                duration: 200
                easing.type: Easing.EaseIn
            }
            NumberAnimation {
                from: 0.3
                to: 0.5
                duration: 200
                easing.type: Easing.EaseOut
            }
            NumberAnimation {
                from: 0.5
                to: 0.3
                duration: 300
                easing.type: Easing.EaseIn
            }
        }
        
        // Error alert pulse (Animation 4)
        SequentialAnimation on opacity {
            id: errorPulse
            running: (variant === "error" || variant === "security") && state === "displaying"
            loops: Animation.Infinite
            NumberAnimation {
                from: 0.4
                to: 0.7
                duration: 500
                easing.type: Easing.InOutSine
            }
            NumberAnimation {
                from: 0.7
                to: 0.4
                duration: 500
                easing.type: Easing.InOutSine
            }
        }
        
        // Warning pulse (slow)
        SequentialAnimation on opacity {
            id: warningPulse
            running: variant === "warning" && state === "displaying"
            loops: Animation.Infinite
            NumberAnimation {
                from: 0.3
                to: 0.5
                duration: 1000
                easing.type: Easing.InOutSine
            }
            NumberAnimation {
                from: 0.5
                to: 0.3
                duration: 1000
                easing.type: Easing.InOutSine
            }
        }
    }
    
    // Slide-in animation (Animation 6: Xenon Slide-In)
    x: state === "displaying" ? 0 : (state === "closing" ? parent.width : -parent.width)
    y: 0
    
    ParallelAnimation {
        id: slideInAnimation
        running: state === "displaying"
        NumberAnimation {
            target: notification
            property: "x"
            from: -notification.width
            to: 0
            duration: 300
            easing.type: Easing.EaseOut
        }
        NumberAnimation {
            target: notification
            property: "opacity"
            from: 0.0
            to: 1.0
            duration: 300
            easing.type: Easing.EaseOut
        }
    }
    
    // Fade-out animation
    ParallelAnimation {
        id: fadeOutAnimation
        running: state === "closing"
        NumberAnimation {
            target: notification
            property: "opacity"
            from: 1.0
            to: 0.0
            duration: 200
            easing.type: Easing.EaseIn
        }
        NumberAnimation {
            target: notification
            property: "x"
            from: 0
            to: notification.width
            duration: 200
            easing.type: Easing.EaseIn
        }
    }
    
    // Content
    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 8
        
        // Header row
        Row {
            width: parent.width
            spacing: 12
            
            // Icon
            Text {
                text: {
                    if (variant === "success") return "✓"
                    if (variant === "error") return "✕"
                    if (variant === "warning") return "⚠"
                    if (variant === "security") return "🔒"
                    return "ℹ"
                }
                font.pixelSize: 20
                color: getXenonColor()
                anchors.verticalCenter: parent.verticalCenter
            }
            
            // Title
            Text {
                width: parent.width - (dismissButton.visible ? 60 : 40)
                text: notification.title
                font.pixelSize: 16
                font.weight: Font.Bold
                color: "#ffffff"
                elide: Text.ElideRight
                anchors.verticalCenter: parent.verticalCenter
            }
            
            // Dismiss button
            Rectangle {
                id: dismissButton
                width: 24
                height: 24
                radius: 12
                color: mouseAreaDismiss.containsMouse ? "#2a2a2a" : "transparent"
                visible: dismissible && !requiresAcknowledgment
                anchors.verticalCenter: parent.verticalCenter
                
                Text {
                    anchors.centerIn: parent
                    text: "×"
                    font.pixelSize: 18
                    color: "#ffffff"
                }
                
                MouseArea {
                    id: mouseAreaDismiss
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        notification.state = "closing"
                    }
                }
            }
        }
        
        // Message
        Text {
            width: parent.width
            text: notification.message
            font.pixelSize: 14
            color: "#cccccc"
            wrapMode: Text.WordWrap
            visible: message.length > 0
        }
        
        // Acknowledgment button (for security alerts)
        Rectangle {
            width: 120
            height: 32
            radius: 6
            color: mouseAreaAck.containsMouse ? xenonErrorColor : "#2a2a2a"
            border.color: xenonErrorColor
            border.width: 1
            visible: requiresAcknowledgment
            
            Text {
                anchors.centerIn: parent
                text: "Acknowledge"
                font.pixelSize: 12
                font.weight: Font.Bold
                color: mouseAreaAck.containsMouse ? "#0a0a0a" : xenonErrorColor
            }
            
            MouseArea {
                id: mouseAreaAck
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    notification.state = "closing"
                }
            }
        }
    }
    
    // Auto-dismiss timer
    Timer {
        id: autoDismissTimer
        running: state === "displaying" && duration > 0 && !requiresAcknowledgment
        interval: duration
        onTriggered: {
            notification.state = "closing"
        }
    }
    
    // Keyboard handling
    Keys.onPressed: {
        if (event.key === Qt.Key_Return || event.key === Qt.Key_Space) {
            if (requiresAcknowledgment) {
                notification.state = "closing"
            } else if (dismissible) {
                notification.state = "closing"
            }
            event.accepted = true
        }
        if (event.key === Qt.Key_Escape && dismissible) {
            notification.state = "closing"
            event.accepted = true
        }
    }
    
    // Signals
    signal dismissed()
    signal acknowledged()
    
    onStateChanged: {
        if (state === "closing") {
            fadeOutAnimation.start()
        }
        if (state === "dismissed") {
            dismissed()
        }
    }
    
    // Accessibility
    Accessible.role: Accessible.Alert
    Accessible.name: title || "Notification"
    Accessible.description: message
    Accessible.onPressAction: {
        if (requiresAcknowledgment) {
            acknowledged()
            state = "closing"
        } else if (dismissible) {
            state = "closing"
        }
    }
}

