import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.core 2.1 as PlasmaCore

/**
 * Token Enrollment Wizard
 * 
 * Step-by-step wizard for enrolling hardware tokens with xenon progress indicators
 */
Dialog {
    id: wizard
    
    title: "Enroll Hardware Token"
    width: 600
    height: 500
    modal: true
    
    property var tokenManager: null
    property int currentStep: 0
    property string selectedTokenType: ""
    property var enrollmentData: ({})
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 16
        
        // Xenon-accented progress indicator
        ProgressBar {
            id: progressBar
            Layout.fillWidth: true
            from: 0
            to: 4
            value: currentStep
            
            background: Rectangle {
                color: "#1a1a1a"
                border.color: "#00d4ff"
                border.width: 1
                radius: 4
            }
            
            contentItem: Item {
                Rectangle {
                    width: progressBar.visualPosition * parent.width
                    height: parent.height
                    color: "#00d4ff"
                    radius: 4
                    
                    SequentialAnimation on opacity {
                        running: true
                        loops: Animation.Infinite
                        NumberAnimation {
                            from: 0.6
                            to: 1.0
                            duration: 2000
                            easing.type: Easing.InOutSine
                        }
                        NumberAnimation {
                            from: 1.0
                            to: 0.6
                            duration: 2000
                            easing.type: Easing.InOutSine
                        }
                    }
                }
            }
        }
        
        // Step content
        StackLayout {
            id: stepStack
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: currentStep
            
            // Step 1: Token Type Selection
            ColumnLayout {
                spacing: 16
                Text {
                    text: "Select Token Type"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#ffffff"
                }
                
                RadioButton {
                    text: "YubiKey"
                    checked: selectedTokenType === "yubikey"
                    onCheckedChanged: if (checked) selectedTokenType = "yubikey"
                }
                RadioButton {
                    text: "Smart Card"
                    checked: selectedTokenType === "smartcard"
                    onCheckedChanged: if (checked) selectedTokenType = "smartcard"
                }
                RadioButton {
                    text: "FIDO2 Device"
                    checked: selectedTokenType === "fido2"
                    onCheckedChanged: if (checked) selectedTokenType = "fido2"
                }
            }
            
            // Step 2: Connect Token
            ColumnLayout {
                spacing: 16
                Text {
                    text: "Connect Token"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#ffffff"
                }
                Text {
                    text: "Please connect your hardware token to the system."
                    color: "#b0b0b0"
                }
            }
            
            // Step 3: Configure Token
            ColumnLayout {
                spacing: 16
                Text {
                    text: "Configure Token"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#ffffff"
                }
                
                TextField {
                    id: tokenNameField
                    placeholderText: "Token Name"
                    Layout.fillWidth: true
                }
            }
            
            // Step 4: Complete
            ColumnLayout {
                spacing: 16
                Text {
                    text: "Enrollment Complete"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#00cc66"
                }
                Text {
                    text: "Token has been successfully enrolled."
                    color: "#b0b0b0"
                }
            }
        }
        
        // Navigation buttons
        RowLayout {
            Layout.fillWidth: true
            
            Button {
                text: "Cancel"
                onClicked: wizard.close()
                visible: currentStep < 3
            }
            
            Item {
                Layout.fillWidth: true
            }
            
            Button {
                text: currentStep === 0 ? "Next" : (currentStep === 3 ? "Close" : "Next")
                onClicked: {
                    if (currentStep === 0) {
                        if (selectedTokenType === "") return
                        currentStep = 1
                        if (tokenManager) tokenManager.startEnrollment()
                    } else if (currentStep === 1) {
                        currentStep = 2
                    } else if (currentStep === 2) {
                        enrollmentData["name"] = tokenNameField.text
                        enrollmentData["type"] = selectedTokenType
                        if (tokenManager) tokenManager.completeEnrollment(enrollmentData)
                        currentStep = 3
                    } else {
                        wizard.close()
                    }
                }
            }
        }
    }
    
    function open() {
        visible = true
        currentStep = 0
        selectedTokenType = ""
        enrollmentData = {}
    }
}

