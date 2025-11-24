import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: policyConfigurationInterface
    color: "transparent"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // Header
        Text {
            text: "Policy Configuration"
            color: "#ffffff"
            font.pixelSize: 20
            font.bold: true
            Layout.fillWidth: true
        }

        // Policy form
        Card {
            Layout.fillWidth: true
            Layout.fillHeight: true
            variant: "dashboard-card"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                Text {
                    text: "Configure Transmission Policy"
                    color: "#ffffff"
                    font.pixelSize: 16
                    font.bold: true
                    Layout.fillWidth: true
                }

                GridLayout {
                    Layout.fillWidth: true
                    columns: 2
                    columnSpacing: 16
                    rowSpacing: 12

                    Text {
                        text: "Policy Name:"
                        color: "#cccccc"
                        Layout.preferredWidth: 150
                    }
                    TextField {
                        id: policyNameField
                        Layout.fillWidth: true
                        placeholderText: "Enter policy name"
                    }

                    Text {
                        text: "Policy Type:"
                        color: "#cccccc"
                        Layout.preferredWidth: 150
                    }
                    ComboBox {
                        id: policyTypeCombo
                        Layout.fillWidth: true
                        model: ["ENCRYPTION_REQUIRED", "PROTOCOL_ALLOWLIST", "CUSTOM"]
                    }

                    Text {
                        text: "Source IP/Network:"
                        color: "#cccccc"
                        Layout.preferredWidth: 150
                    }
                    TextField {
                        id: sourceField
                        Layout.fillWidth: true
                        placeholderText: "e.g., 192.168.1.0/24 or leave empty"
                    }

                    Text {
                        text: "Destination IP/Network:"
                        color: "#cccccc"
                        Layout.preferredWidth: 150
                    }
                    TextField {
                        id: destinationField
                        Layout.fillWidth: true
                        placeholderText: "e.g., 10.0.0.0/8 or leave empty"
                    }

                    Text {
                        text: "Protocol:"
                        color: "#cccccc"
                        Layout.preferredWidth: 150
                    }
                    ComboBox {
                        id: protocolCombo
                        Layout.fillWidth: true
                        model: ["Any", "TCP", "UDP", "ICMP"]
                    }

                    Text {
                        text: "Port:"
                        color: "#cccccc"
                        Layout.preferredWidth: 150
                    }
                    TextField {
                        id: portField
                        Layout.fillWidth: true
                        placeholderText: "Port number or range (e.g., 443 or 8000-9000)"
                    }

                    Text {
                        text: "Encryption Required:"
                        color: "#cccccc"
                        Layout.preferredWidth: 150
                    }
                    CheckBox {
                        id: encryptionRequiredCheck
                        checked: true
                    }

                    Text {
                        text: "Priority:"
                        color: "#cccccc"
                        Layout.preferredWidth: 150
                    }
                    SpinBox {
                        id: prioritySpinBox
                        Layout.fillWidth: true
                        from: 1
                        to: 100
                        value: 50
                    }

                    Text {
                        text: "Enabled:"
                        color: "#cccccc"
                        Layout.preferredWidth: 150
                    }
                    CheckBox {
                        id: enabledCheck
                        checked: true
                    }
                }

                // Action buttons
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    Button {
                        id: configureButton
                        Layout.fillWidth: true
                        Layout.preferredHeight: 48
                        text: dataGuardManager && dataGuardManager.configuringPolicy ? "Configuring..." : "Configure Policy"
                        variant: "primary"
                        state: dataGuardManager && dataGuardManager.configuringPolicy ? "loading" : "default"
                        enabled: !(dataGuardManager && dataGuardManager.configuringPolicy) && dbusInterface && dbusInterface.serviceAvailable && policyNameField.text !== ""

                        onClicked: {
                            if (dataGuardManager) {
                                var policy = {
                                    "policy_name": policyNameField.text,
                                    "policy_type": policyTypeCombo.currentText,
                                    "enabled": enabledCheck.checked,
                                    "priority": prioritySpinBox.value,
                                    "rules": [{
                                        "source": sourceField.text || "",
                                        "destination": destinationField.text || "",
                                        "protocol": protocolCombo.currentText === "Any" ? "" : protocolCombo.currentText,
                                        "port": portField.text || "",
                                        "encryption_required": encryptionRequiredCheck.checked,
                                        "action": "BLOCK"
                                    }]
                                }
                                dataGuardManager.configurePolicy(policy)
                            }
                        }
                    }

                    Button {
                        text: "Clear"
                        variant: "secondary"
                        Layout.preferredWidth: 120
                        onClicked: {
                            policyNameField.text = ""
                            sourceField.text = ""
                            destinationField.text = ""
                            portField.text = ""
                            policyTypeCombo.currentIndex = 0
                            protocolCombo.currentIndex = 0
                            encryptionRequiredCheck.checked = true
                            prioritySpinBox.value = 50
                            enabledCheck.checked = true
                        }
                    }
                }

                // Status message
                Text {
                    id: statusMessage
                    text: ""
                    color: "#00ffff"
                    font.pixelSize: 12
                    Layout.fillWidth: true
                    visible: text !== ""
                }
            }
        }
    }

    Connections {
        target: dataGuardManager
        function onPolicyConfigured(success) {
            if (success) {
                statusMessage.text = "Policy configured successfully"
                statusMessage.color = "#00ff00"
            } else {
                statusMessage.text = "Policy configuration failed"
                statusMessage.color = "#ff0000"
            }
        }
        function onPolicyConfigurationFailed(error) {
            statusMessage.text = "Policy configuration failed: " + error
            statusMessage.color = "#ff0000"
        }
    }
}

