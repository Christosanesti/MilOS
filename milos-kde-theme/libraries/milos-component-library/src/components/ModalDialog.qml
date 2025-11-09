import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: dialog
    
    // Properties
    property string variant: "confirmation" // confirmation, warning, error, form, progress
    property string state: "displaying" // opening, displaying, closing, processing
    property string title: ""
    property string message: ""
    property bool critical: false // Cannot be dismissed without action
    property bool visible: false
    property color xenonColor: "#00d4ff"
    property color xenonWarningColor: "#ffaa00"
    property color xenonErrorColor: "#ff4444"
    
    // Dimensions
    width: parent ? parent.width : 600
    height: parent ? parent.height : 400
    
    // Overlay
    Rectangle {
        id: overlay
        anchors.fill: parent
        color: "#000000"
        opacity: 0.7
        
        // Subtle xenon accent
        Rectangle {
            anchors.fill: parent
            color: xenonColor
            opacity: 0.1
        }
        
        MouseArea {
            anchors.fill: parent
            enabled: !critical && state === "displaying"
            onClicked: {
                if (!critical) {
                    state = "closing"
                }
            }
        }
    }
    
    // Dialog container
    Rectangle {
        id: dialogContainer
        anchors.centerIn: parent
        width: 400
        height: implicitHeight
        implicitHeight: {
            var baseHeight = title.length > 0 ? 60 : 40
            var messageHeight = message.length > 0 ? (message.split('\n').length * 20) : 0
            var contentHeight = contentItem.childrenRect.height
            var buttonHeight = 50
            return baseHeight + messageHeight + contentHeight + buttonHeight + 40
        }
        
        color: "#1a1a1a"
        radius: 12
        border.width: {
            if (variant === "warning") return 3
            if (variant === "error") return 3
            return 2
        }
        border.color: getBorderColor()
        
        // Xenon border glow
        Rectangle {
            anchors.fill: parent
            radius: parent.radius
            color: "transparent"
            border.color: getBorderColor()
            border.width: parent.border.width
            opacity: getBorderOpacity()
            
            // Warning/Error pulse (Animation 4: Xenon Alert Pulse)
            SequentialAnimation on opacity {
                id: alertPulse
                running: (variant === "warning" || variant === "error") && state === "displaying"
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
        
        // Open/Close animation (Animation 6 variant: Fade-in)
        opacity: {
            if (state === "opening" || state === "displaying") return 1.0
            if (state === "closing") return 0.0
            return 0.0
        }
        scale: {
            if (state === "opening" || state === "displaying") return 1.0
            if (state === "closing") return 0.95
            return 0.95
        }
        
        ParallelAnimation {
            id: openAnimation
            running: state === "opening"
            NumberAnimation {
                target: dialogContainer
                property: "opacity"
                from: 0.0
                to: 1.0
                duration: 200
                easing.type: Easing.EaseOut
            }
            NumberAnimation {
                target: dialogContainer
                property: "scale"
                from: 0.95
                to: 1.0
                duration: 200
                easing.type: Easing.EaseOut
            }
        }
        
        ParallelAnimation {
            id: closeAnimation
            running: state === "closing"
            NumberAnimation {
                target: dialogContainer
                property: "opacity"
                from: 1.0
                to: 0.0
                duration: 200
                easing.type: Easing.EaseIn
            }
            NumberAnimation {
                target: dialogContainer
                property: "scale"
                from: 1.0
                to: 0.95
                duration: 200
                easing.type: Easing.EaseIn
            }
        }
        
        // Content
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 16
            
            // Header
            Text {
                Layout.fillWidth: true
                text: dialog.title
                font.pixelSize: 20
                font.weight: Font.Bold
                color: "#ffffff"
                visible: title.length > 0
            }
            
            // Message
            Text {
                Layout.fillWidth: true
                text: dialog.message
                font.pixelSize: 14
                color: "#cccccc"
                wrapMode: Text.WordWrap
                visible: message.length > 0
            }
            
            // Content area (for form/progress dialogs)
            Item {
                id: contentItem
                Layout.fillWidth: true
                Layout.fillHeight: true
                visible: variant === "form" || variant === "progress"
                
                // Progress bar (for progress dialog)
                Rectangle {
                    anchors.fill: parent
                    visible: variant === "progress"
                    
                    Rectangle {
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        width: parent.width * 0.6 // Progress value
                        color: xenonColor
                        opacity: 0.7
                    }
                }
            }
            
            // Buttons
            RowLayout {
                Layout.fillWidth: true
                spacing: 12
                
                // Cancel button (if not critical)
                Rectangle {
                    Layout.fillWidth: true
                    height: 40
                    radius: 6
                    color: mouseAreaCancel.containsMouse ? "#2a2a2a" : "transparent"
                    border.color: "#666666"
                    border.width: 1
                    visible: !critical || variant === "form"
                    
                    Text {
                        anchors.centerIn: parent
                        text: "Cancel"
                        font.pixelSize: 14
                        color: "#ffffff"
                    }
                    
                    MouseArea {
                        id: mouseAreaCancel
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            if (!critical) {
                                state = "closing"
                                cancelled()
                            }
                        }
                    }
                }
                
                // Confirm/Action button
                Rectangle {
                    Layout.fillWidth: true
                    height: 40
                    radius: 6
                    color: mouseAreaConfirm.containsMouse ? xenonColor : "#2a2a2a"
                    border.color: xenonColor
                    border.width: 1
                    
                    Text {
                        anchors.centerIn: parent
                        text: variant === "progress" ? "Close" : "Confirm"
                        font.pixelSize: 14
                        font.weight: Font.Bold
                        color: mouseAreaConfirm.containsMouse ? "#0a0a0a" : xenonColor
                    }
                    
                    MouseArea {
                        id: mouseAreaConfirm
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            confirmed()
                            if (!critical) {
                                state = "closing"
                            }
                        }
                    }
                }
            }
        }
    }
    
    function getBorderColor() {
        if (variant === "warning") return xenonWarningColor
        if (variant === "error") return xenonErrorColor
        return xenonColor
    }
    
    function getBorderOpacity() {
        if (variant === "warning" || variant === "error") return 0.8
        return 0.6
    }
    
    // Show dialog
    function show() {
        visible = true
        state = "opening"
        Qt.callLater(function() {
            state = "displaying"
            dialogContainer.forceActiveFocus()
        })
    }
    
    // Hide dialog
    function hide() {
        state = "closing"
        Qt.callLater(function() {
            visible = false
        })
    }
    
    // Keyboard handling
    Keys.onPressed: {
        if (event.key === Qt.Key_Escape && !critical && state === "displaying") {
            state = "closing"
            cancelled()
            event.accepted = true
        }
        if (event.key === Qt.Key_Return && state === "displaying") {
            confirmed()
            if (!critical) {
                state = "closing"
            }
            event.accepted = true
        }
    }
    
    // Focus management
    onVisibleChanged: {
        if (visible) {
            show()
        } else {
            hide()
        }
    }
    
    // Signals
    signal confirmed()
    signal cancelled()
    
    // Accessibility
    Accessible.role: Accessible.Dialog
    Accessible.name: title || "Dialog"
    Accessible.description: message
    Accessible.onPressAction: {
        if (!critical) {
            confirmed()
        }
    }
}

