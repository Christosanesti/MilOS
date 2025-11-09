import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: inputField
    
    // Properties
    property string variant: "text" // text, password, textarea, search, number
    property string state: "default" // default, focus, filled, error, disabled, readonly
    property string placeholder: ""
    property string label: ""
    property string value: ""
    property string errorMessage: ""
    property bool enabled: true
    property bool readOnly: false
    property int passwordStrength: 0 // 0-4 for password strength indicator
    property int maxLength: -1
    property real inputHeight: variant === "textarea" ? 120 : 48
    
    // Xenon color properties
    property color xenonColor: "#00d4ff"
    property color xenonFocusColor: "#00ffff"
    property color xenonValidColor: "#00cc66"
    property color xenonErrorColor: "#ff4444"
    
    // Dimensions
    width: 300
    height: label.length > 0 ? inputHeight + 24 : inputHeight // Add space for label
    implicitWidth: 300
    implicitHeight: height
    
    // Base styling
    color: "transparent"
    border.width: 0
    
    // Label (positioned above)
    Text {
        id: labelText
        anchors.top: parent.top
        anchors.left: parent.left
        text: inputField.label
        font.pixelSize: 14
        font.weight: Font.Medium
        color: inputField.state === "error" ? inputField.xenonErrorColor : inputField.xenonColor
        visible: inputField.label.length > 0
        height: visible ? 20 : 0
    }
    
    // Input container
    Rectangle {
        id: inputContainer
        anchors.top: labelText.visible ? labelText.bottom : parent.top
        anchors.topMargin: labelText.visible ? 4 : 0
        anchors.left: parent.left
        anchors.right: parent.right
        height: inputField.inputHeight
        color: "#2a2a2a"
        radius: 8
        
        // Xenon border glow
        Rectangle {
            id: borderGlow
            anchors.fill: parent
            radius: parent.radius
            color: "transparent"
            border.color: {
                if (!inputField.enabled || inputField.readOnly) return "#666666"
                if (inputField.state === "error") return inputField.xenonErrorColor
                if (inputField.state === "filled" && inputField.value.length > 0) return inputField.xenonValidColor
                if (inputField.state === "focus") return inputField.xenonFocusColor
                return inputField.xenonColor
            }
            border.width: {
                if (inputField.state === "focus" || inputField.state === "error") return 2
                return 1
            }
            opacity: {
                if (inputField.state === "focus") return 0.8
                if (inputField.state === "error") return 1.0
                if (inputField.state === "filled") return 0.6
                return 0.4 // Default
            }
            
            // Focus pulse animation (Animation 9: Xenon Focus Flash)
            SequentialAnimation on opacity {
                id: focusPulseAnimation
                running: inputField.state === "focus" && inputField.enabled
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
            
            // Error pulse animation (Animation 4: Xenon Alert Pulse)
            SequentialAnimation on opacity {
                id: errorPulseAnimation
                running: inputField.state === "error"
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
        }
        
        // Text input
        TextInput {
            id: textInput
            anchors.fill: parent
            anchors.leftMargin: 12
            anchors.rightMargin: 12
            anchors.verticalCenter: parent.verticalCenter
            text: inputField.value
            font.pixelSize: 16
            color: inputField.enabled ? "#ffffff" : "#666666"
            selectByMouse: true
            readOnly: inputField.readOnly
            echoMode: inputField.variant === "password" ? TextInput.Password : TextInput.Normal
            inputMethodHints: {
                if (inputField.variant === "number") return Qt.ImhDigitsOnly
                if (inputField.variant === "search") return Qt.ImhNoPredictiveText
                return Qt.ImhNone
            }
            maximumLength: inputField.maxLength > 0 ? inputField.maxLength : 32767
            
            onTextChanged: {
                inputField.value = text
                if (text.length > 0) {
                    inputField.state = "filled"
                } else {
                    inputField.state = "default"
                }
            }
            
            onActiveFocusChanged: {
                if (activeFocus && inputField.enabled) {
                    inputField.state = "focus"
                } else if (!activeFocus && inputField.value.length === 0) {
                    inputField.state = "default"
                } else if (!activeFocus && inputField.value.length > 0) {
                    inputField.state = "filled"
                }
            }
        }
        
        // TextArea variant
        TextArea {
            id: textArea
            anchors.fill: parent
            anchors.margins: 12
            visible: inputField.variant === "textarea"
            text: inputField.value
            font.pixelSize: 16
            color: inputField.enabled ? "#ffffff" : "#666666"
            background: Rectangle {
                color: "transparent"
            }
            readOnly: inputField.readOnly
            selectByMouse: true
            
            onTextChanged: {
                inputField.value = text
                if (text.length > 0) {
                    inputField.state = "filled"
                } else {
                    inputField.state = "default"
                }
            }
            
            onActiveFocusChanged: {
                if (activeFocus && inputField.enabled) {
                    inputField.state = "focus"
                } else if (!activeFocus && inputField.value.length === 0) {
                    inputField.state = "default"
                } else if (!activeFocus && inputField.value.length > 0) {
                    inputField.state = "filled"
                }
            }
        }
        
        // Placeholder text
        Text {
            anchors.left: parent.left
            anchors.leftMargin: 12
            anchors.verticalCenter: parent.verticalCenter
            text: inputField.placeholder
            font.pixelSize: 16
            color: "#666666"
            visible: inputField.value.length === 0 && !textInput.activeFocus && !textArea.activeFocus
        }
        
        // Search icon (for search variant)
        Text {
            anchors.right: parent.right
            anchors.rightMargin: 12
            anchors.verticalCenter: parent.verticalCenter
            text: "🔍"
            font.pixelSize: 16
            visible: inputField.variant === "search"
        }
        
        // Password strength indicator
        Row {
            id: passwordStrengthIndicator
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 4
            height: 4
            spacing: 4
            visible: inputField.variant === "password" && inputField.value.length > 0
            
            Repeater {
                model: 4
                Rectangle {
                    width: (parent.width - 12) / 4
                    height: 4
                    radius: 2
                    color: {
                        var strength = inputField.passwordStrength
                        if (index < strength) {
                            if (strength <= 1) return "#ff4444" // Red
                            if (strength <= 2) return "#ffaa00" // Amber
                            return "#00cc66" // Green
                        }
                        return "#2a2a2a"
                    }
                    
                    // Xenon glow for active segments
                    Rectangle {
                        anchors.fill: parent
                        radius: parent.radius
                        color: parent.color
                        opacity: index < inputField.passwordStrength ? 0.5 : 0.0
                        
                        SequentialAnimation on opacity {
                            running: index < inputField.passwordStrength
                            loops: Animation.Infinite
                            NumberAnimation {
                                from: 0.3
                                to: 0.7
                                duration: 1000
                                easing.type: Easing.InOutSine
                            }
                            NumberAnimation {
                                from: 0.7
                                to: 0.3
                                duration: 1000
                                easing.type: Easing.InOutSine
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Error message
    Text {
        id: errorText
        anchors.top: inputContainer.bottom
        anchors.left: parent.left
        anchors.topMargin: 4
        text: inputField.errorMessage
        font.pixelSize: 12
        color: inputField.xenonErrorColor
        visible: inputField.state === "error" && inputField.errorMessage.length > 0
        height: visible ? contentHeight : 0
    }
    
    // Update password strength
    function updatePasswordStrength() {
        if (inputField.variant !== "password") return
        
        var password = inputField.value
        var strength = 0
        
        if (password.length >= 8) strength++
        if (password.length >= 12) strength++
        if (/[a-z]/.test(password) && /[A-Z]/.test(password)) strength++
        if (/[0-9]/.test(password)) strength++
        if (/[^a-zA-Z0-9]/.test(password)) strength++
        
        inputField.passwordStrength = Math.min(strength, 4)
    }
    
    Connections {
        target: inputField
        function onValueChanged() {
            if (inputField.variant === "password") {
                updatePasswordStrength()
            }
        }
    }
    
    // Accessibility
    Accessible.role: Accessible.EditableText
    Accessible.name: inputField.label || inputField.placeholder
    Accessible.description: inputField.errorMessage.length > 0 ? inputField.errorMessage : ""
}

