import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: encryptionSetupScreen
    
    color: "#0a0a0a"
    
    property string encryptionPassword: ""
    property string passwordConfirmation: ""
    property string selectedAlgorithm: "aes-xts-plain64"
    property int algorithmCategoryIndex: 0 // 0 = Standard, 1 = Advanced
    property bool passwordsMatch: false
    property string passwordStrength: "weak"
    property bool encryptionInProgress: false
    property double encryptionProgress: 0.0
    property bool showEncryptionConfirmationDialog: false
    
    signal backClicked()
    signal nextClicked()
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 32
        spacing: 24
        
        // Title
        Text {
            Layout.fillWidth: true
            text: "Disk Encryption Setup"
            font.pixelSize: 28
            font.weight: Font.Bold
            color: "#ffffff"
        }
        
        Text {
            Layout.fillWidth: true
            text: "Configure full disk encryption to protect your data"
            font.pixelSize: 16
            color: "#cccccc"
        }
        
        // Encryption password input
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            variant: "dashboard"
            title: "Encryption Password"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 16
                
                // Password input
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    
                    Text {
                        Layout.fillWidth: true
                        text: "Password"
                        font.pixelSize: 14
                        font.weight: Font.Medium
                        color: "#e0e0e0"
                    }
                    
                    InputField {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 48
                        variant: "password"
                        placeholderText: "Enter encryption password"
                        text: encryptionSetupScreen.encryptionPassword
                        onTextChanged: {
                            encryptionSetupScreen.encryptionPassword = text
                            updatePasswordStrength()
                            checkPasswordMatch()
                        }
                    }
                }
                
                // Password strength indicator
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    visible: encryptionSetupScreen.encryptionPassword.length > 0
                    
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 8
                            radius: 4
                            color: {
                                if (encryptionSetupScreen.passwordStrength === "weak") return "#ff4444"
                                if (encryptionSetupScreen.passwordStrength === "medium") return "#ffaa00"
                                return "#00cc66"
                            }
                            
                            // Xenon pulse animation
                            SequentialAnimation on opacity {
                                running: encryptionSetupScreen.passwordStrength !== "weak"
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
                            if (encryptionSetupScreen.passwordStrength === "weak") return "Weak password"
                            if (encryptionSetupScreen.passwordStrength === "medium") return "Medium strength"
                            return "Strong password"
                        }
                        font.pixelSize: 12
                        color: {
                            if (encryptionSetupScreen.passwordStrength === "weak") return "#ff4444"
                            if (encryptionSetupScreen.passwordStrength === "medium") return "#ffaa00"
                            return "#00cc66"
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
                        placeholderText: "Re-enter encryption password"
                        text: encryptionSetupScreen.passwordConfirmation
                        onTextChanged: {
                            encryptionSetupScreen.passwordConfirmation = text
                            checkPasswordMatch()
                        }
                    }
                    
                    // Password match indicator
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        visible: encryptionSetupScreen.passwordConfirmation.length > 0
                        
                        StatusIndicator {
                            variant: "badge"
                            status: encryptionSetupScreen.passwordsMatch ? "success" : "error"
                            text: encryptionSetupScreen.passwordsMatch ? "Passwords match" : "Passwords do not match"
                        }
                    }
                }
            }
        }
        
        // Encryption algorithm selection with Tab Navigation
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            variant: "dashboard"
            title: "Encryption Algorithm"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 16
                
                // Tab Navigation for algorithm categories
                TabNavigation {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 48
                    variant: "standard"
                    currentIndex: encryptionSetupScreen.algorithmCategoryIndex
                    maxTabs: 10
                    tabs: [
                        {text: "Standard", icon: "", badge: 0, enabled: true},
                        {text: "Advanced", icon: "", badge: 0, enabled: true}
                    ]
                    onTabChanged: function(index) {
                        encryptionSetupScreen.algorithmCategoryIndex = index
                    }
                }
                
                // Algorithm selection based on category
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 16
                    
                    Text {
                        Layout.fillWidth: true
                        text: "Select encryption algorithm"
                        font.pixelSize: 14
                        color: "#ffffff"
                    }
                    
                    DropdownMenu {
                        Layout.preferredWidth: 300
                        Layout.preferredHeight: 48
                        variant: "standard"
                        items: encryptionSetupScreen.algorithmCategoryIndex === 0 ? [
                            {text: "AES-XTS (Recommended)", value: "aes-xts-plain64"},
                            {text: "Serpent-XTS", value: "serpent-xts-plain64"},
                            {text: "Twofish-XTS", value: "twofish-xts-plain64"}
                        ] : [
                            {text: "AES-XTS-256", value: "aes-xts-plain64-256"},
                            {text: "Serpent-XTS-256", value: "serpent-xts-plain64-256"},
                            {text: "Twofish-XTS-256", value: "twofish-xts-plain64-256"}
                        ]
                        selectedValue: encryptionSetupScreen.selectedAlgorithm
                        onSelectedValueChanged: {
                            encryptionSetupScreen.selectedAlgorithm = selectedValue
                        }
                    }
                }
            }
        }
        
        // Encryption progress (when applying)
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 12
            visible: encryptionSetupScreen.encryptionInProgress
            
            Text {
                Layout.fillWidth: true
                text: "Setting up disk encryption..."
                font.pixelSize: 16
                color: "#00d4ff"
                horizontalAlignment: Text.AlignHCenter
            }
            
            ProgressBar {
                Layout.fillWidth: true
                Layout.preferredHeight: 32
                variant: "linear"
                value: encryptionSetupScreen.encryptionProgress
                status: "processing"
            }
            
            Text {
                Layout.fillWidth: true
                text: Math.round(encryptionSetupScreen.encryptionProgress) + "%"
                font.pixelSize: 14
                color: "#cccccc"
                horizontalAlignment: Text.AlignHCenter
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
                enabled: !encryptionSetupScreen.encryptionInProgress
                onClicked: {
                    encryptionSetupScreen.backClicked()
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
                enabled: encryptionSetupScreen.encryptionPassword.length > 0 && 
                         encryptionSetupScreen.passwordsMatch && 
                         !encryptionSetupScreen.encryptionInProgress
                onClicked: {
                    encryptionSetupScreen.showEncryptionConfirmationDialog = true
                }
            }
        }
    }
    
    // Modal Dialog for encryption confirmation
    ModalDialog {
        id: encryptionConfirmationDialog
        anchors.fill: parent
        variant: "warning"
        title: "Confirm Disk Encryption"
        message: "Disk encryption will be applied to your system. This process cannot be undone. Are you sure you want to continue?"
        critical: true
        visible: encryptionSetupScreen.showEncryptionConfirmationDialog
        
        onConfirmed: {
            encryptionSetupScreen.showEncryptionConfirmationDialog = false
            encryptionSetupScreen.encryptionInProgress = true
            encryptionSetupScreen.encryptionProgress = 0.0
            // TODO: Start encryption process
            // Simulate encryption progress
            var progressTimer = Qt.createQmlObject("import QtQuick 2.15; Timer { interval: 100; running: true; repeat: true }", encryptionSetupScreen)
            progressTimer.triggered.connect(function() {
                encryptionSetupScreen.encryptionProgress += 2
                if (encryptionSetupScreen.encryptionProgress >= 100) {
                    encryptionSetupScreen.encryptionInProgress = false
                    progressTimer.stop()
                    encryptionSetupScreen.nextClicked()
                }
            })
        }
        
        onCancelled: {
            encryptionSetupScreen.showEncryptionConfirmationDialog = false
        }
    }
    
    // Password strength calculation
    function updatePasswordStrength() {
        var password = encryptionSetupScreen.encryptionPassword
        if (password.length === 0) {
            encryptionSetupScreen.passwordStrength = "weak"
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
            encryptionSetupScreen.passwordStrength = "weak"
        } else if (strength < 5) {
            encryptionSetupScreen.passwordStrength = "medium"
        } else {
            encryptionSetupScreen.passwordStrength = "strong"
        }
    }
    
    // Password match validation
    function checkPasswordMatch() {
        encryptionSetupScreen.passwordsMatch = 
            encryptionSetupScreen.encryptionPassword === encryptionSetupScreen.passwordConfirmation &&
            encryptionSetupScreen.encryptionPassword.length > 0
    }
    
    // Accessibility
    Accessible.role: Accessible.Pane
    Accessible.name: "Disk Encryption Setup"
    Accessible.description: "Configure full disk encryption for your system"
}

