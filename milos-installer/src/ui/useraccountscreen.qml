import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: userAccountScreen
    
    color: "#0a0a0a"
    
    property string username: ""
    property string password: ""
    property string passwordConfirmation: ""
    property string fullName: ""
    property bool passwordsMatch: false
    property string passwordStrength: "weak"
    property bool usernameValid: false
    property var passwordRequirements: ({
        length: false,
        uppercase: false,
        lowercase: false,
        number: false,
        special: false
    })
    
    signal backClicked()
    signal nextClicked()
    
    ScrollView {
        anchors.fill: parent
        clip: true
        
        ColumnLayout {
            width: userAccountScreen.width - 64
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 32
            spacing: 24
            
            // Title
            Text {
                Layout.fillWidth: true
                text: "Create User Account"
                font.pixelSize: 28
                font.weight: Font.Bold
                color: "#ffffff"
            }
            
            Text {
                Layout.fillWidth: true
                text: "Set up your user account for MilOS"
                font.pixelSize: 16
                color: "#cccccc"
            }
            
            // Username input
            Card {
                Layout.fillWidth: true
                Layout.preferredHeight: 120
                variant: "dashboard"
                title: "Username"
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 24
                    spacing: 8
                    
                    InputField {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 48
                        variant: "text"
                        placeholderText: "Enter username"
                        text: userAccountScreen.username
                        onTextChanged: {
                            userAccountScreen.username = text
                            validateUsername()
                        }
                    }
                    
                    StatusIndicator {
                        variant: "badge"
                        status: userAccountScreen.usernameValid ? "success" : "warning"
                        text: userAccountScreen.usernameValid ? "Username available" : "Username must be 3-32 characters, alphanumeric and underscores only"
                        visible: userAccountScreen.username.length > 0
                    }
                }
            }
            
            // Full name input
            Card {
                Layout.fillWidth: true
                Layout.preferredHeight: 120
                variant: "dashboard"
                title: "Full Name"
                
                InputField {
                    anchors.fill: parent
                    anchors.margins: 24
                    variant: "text"
                    placeholderText: "Enter your full name"
                    text: userAccountScreen.fullName
                    onTextChanged: {
                        userAccountScreen.fullName = text
                    }
                }
            }
            
            // Password input
            Card {
                Layout.fillWidth: true
                Layout.preferredHeight: 300
                variant: "dashboard"
                title: "Password"
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 24
                    spacing: 16
                    
                    // Password input
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        
                        InputField {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 48
                            variant: "password"
                            placeholderText: "Enter password"
                            text: userAccountScreen.password
                            onTextChanged: {
                                userAccountScreen.password = text
                                updatePasswordStrength()
                                checkPasswordRequirements()
                                checkPasswordMatch()
                            }
                        }
                    }
                    
                    // Password strength indicator
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        visible: userAccountScreen.password.length > 0
                        
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8
                            
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 8
                                radius: 4
                                color: {
                                    if (userAccountScreen.passwordStrength === "weak") return "#ff4444"
                                    if (userAccountScreen.passwordStrength === "medium") return "#ffaa00"
                                    return "#00cc66"
                                }
                                
                                // Xenon pulse animation
                                SequentialAnimation on opacity {
                                    running: userAccountScreen.passwordStrength !== "weak"
                                    loops: Animation.Infinite
                                    NumberAnimation {
                                        from: 0.6
                                        to: 1.0
                                        duration: 1000
                                        easing.type: Easing.EaseInOut
                                    }
                                    NumberAnimation {
                                        from: 1.0
                                        to: 0.6
                                        duration: 1000
                                        easing.type: Easing.EaseInOut
                                    }
                                }
                            }
                        }
                        
                        Text {
                            Layout.fillWidth: true
                            text: {
                                if (userAccountScreen.passwordStrength === "weak") return "Weak password"
                                if (userAccountScreen.passwordStrength === "medium") return "Medium strength"
                                return "Strong password"
                            }
                            font.pixelSize: 12
                            color: {
                                if (userAccountScreen.passwordStrength === "weak") return "#ff4444"
                                if (userAccountScreen.passwordStrength === "medium") return "#ffaa00"
                                return "#00cc66"
                            }
                        }
                    }
                    
                    // Password requirements
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        visible: userAccountScreen.password.length > 0
                        
                        Text {
                            Layout.fillWidth: true
                            text: "Password Requirements:"
                            font.pixelSize: 14
                            font.weight: Font.Medium
                            color: "#e0e0e0"
                        }
                        
                        Repeater {
                            model: [
                                {text: "At least 12 characters", key: "length"},
                                {text: "Contains uppercase letter", key: "uppercase"},
                                {text: "Contains lowercase letter", key: "lowercase"},
                                {text: "Contains number", key: "number"},
                                {text: "Contains special character", key: "special"}
                            ]
                            
                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 8
                                
                                StatusIndicator {
                                    variant: "badge"
                                    status: userAccountScreen.passwordRequirements[modelData.key] ? "success" : "warning"
                                    text: modelData.text
                                }
                            }
                        }
                    }
                    
                    // Password confirmation
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        
                        Text {
                            Layout.fillWidth: true
                            text: "Confirm Password"
                            font.pixelSize: 14
                            font.weight: Font.Medium
                            color: "#e0e0e0"
                        }
                        
                        InputField {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 48
                            variant: "password"
                            placeholderText: "Re-enter password"
                            text: userAccountScreen.passwordConfirmation
                            onTextChanged: {
                                userAccountScreen.passwordConfirmation = text
                                checkPasswordMatch()
                            }
                        }
                        
                        // Password match indicator
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8
                            visible: userAccountScreen.passwordConfirmation.length > 0
                            
                            StatusIndicator {
                                variant: "badge"
                                status: userAccountScreen.passwordsMatch ? "success" : "error"
                                text: userAccountScreen.passwordsMatch ? "Passwords match" : "Passwords do not match"
                            }
                        }
                    }
                }
            }
            
            // Security recommendations
            Card {
                Layout.fillWidth: true
                Layout.preferredHeight: 150
                variant: "dashboard"
                title: "Security Recommendations"
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 24
                    spacing: 12
                    
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        
                        StatusIndicator {
                            variant: "badge"
                            status: "success"
                            text: "Use a unique password"
                        }
                    }
                    
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        
                        StatusIndicator {
                            variant: "badge"
                            status: "success"
                            text: "Enable two-factor authentication after installation"
                        }
                    }
                    
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        
                        StatusIndicator {
                            variant: "badge"
                            status: "success"
                            text: "Keep your password secure and never share it"
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
                        userAccountScreen.backClicked()
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
                    enabled: userAccountScreen.usernameValid && 
                             userAccountScreen.password.length >= 12 && 
                             userAccountScreen.passwordsMatch &&
                             Object.values(userAccountScreen.passwordRequirements).every(function(v) { return v })
                    onClicked: {
                        userAccountScreen.nextClicked()
                    }
                }
            }
        }
    }
    
    // Username validation
    function validateUsername() {
        var username = userAccountScreen.username
        if (username.length < 3 || username.length > 32) {
            userAccountScreen.usernameValid = false
            return
        }
        // Alphanumeric and underscores only
        if (!/^[a-zA-Z0-9_]+$/.test(username)) {
            userAccountScreen.usernameValid = false
            return
        }
        // Must start with letter
        if (!/^[a-zA-Z]/.test(username)) {
            userAccountScreen.usernameValid = false
            return
        }
        userAccountScreen.usernameValid = true
    }
    
    // Password strength calculation
    function updatePasswordStrength() {
        var password = userAccountScreen.password
        if (password.length === 0) {
            userAccountScreen.passwordStrength = "weak"
            return
        }
        
        var strength = 0
        if (password.length >= 12) strength += 1
        if (password.length >= 16) strength += 1
        if (/[a-z]/.test(password)) strength += 1
        if (/[A-Z]/.test(password)) strength += 1
        if (/[0-9]/.test(password)) strength += 1
        if (/[^a-zA-Z0-9]/.test(password)) strength += 1
        
        if (strength < 3) {
            userAccountScreen.passwordStrength = "weak"
        } else if (strength < 5) {
            userAccountScreen.passwordStrength = "medium"
        } else {
            userAccountScreen.passwordStrength = "strong"
        }
    }
    
    // Password requirements check
    function checkPasswordRequirements() {
        var password = userAccountScreen.password
        userAccountScreen.passwordRequirements.length = password.length >= 12
        userAccountScreen.passwordRequirements.uppercase = /[A-Z]/.test(password)
        userAccountScreen.passwordRequirements.lowercase = /[a-z]/.test(password)
        userAccountScreen.passwordRequirements.number = /[0-9]/.test(password)
        userAccountScreen.passwordRequirements.special = /[^a-zA-Z0-9]/.test(password)
    }
    
    // Password match validation
    function checkPasswordMatch() {
        userAccountScreen.passwordsMatch = 
            userAccountScreen.password === userAccountScreen.passwordConfirmation &&
            userAccountScreen.password.length > 0
    }
    
    // Accessibility
    Accessible.role: Accessible.Pane
    Accessible.name: "User Account Creation"
    Accessible.description: "Create your user account for MilOS"
}

