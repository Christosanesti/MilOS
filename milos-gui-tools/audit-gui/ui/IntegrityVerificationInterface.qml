import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: integrityVerificationInterface
    color: "transparent"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // Header
        Text {
            text: "Log Integrity Verification"
            color: "#ffffff"
            font.pixelSize: 20
            font.bold: true
            Layout.fillWidth: true
        }

        // Integrity status card
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            variant: "dashboard-card"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 16

                Text {
                    text: "Current Integrity Status"
                    color: "#ffffff"
                    font.pixelSize: 16
                    font.bold: true
                    Layout.fillWidth: true
                }

                StatusIndicator {
                    Layout.preferredWidth: 300
                    Layout.preferredHeight: 60
                    variant: "status-light"
                    state: auditManager && auditManager.integrityStatus ? "active" : "critical"
                    text: auditManager && auditManager.integrityStatus ? "Integrity Verified" : "Integrity Violation Detected"
                }

                Text {
                    text: auditManager && auditManager.integrityStatus 
                          ? "All audit logs are intact and have not been tampered with."
                          : "WARNING: Log integrity violation detected. Audit logs may have been tampered with."
                    color: auditManager && auditManager.integrityStatus ? "#00ff00" : "#ff0000"
                    font.pixelSize: 14
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }
        }

        // Verification button
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 150
            variant: "dashboard-card"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                Text {
                    text: "Manual Verification"
                    color: "#ffffff"
                    font.pixelSize: 16
                    font.bold: true
                    Layout.fillWidth: true
                }

                Button {
                    id: verifyButton
                    Layout.fillWidth: true
                    Layout.preferredHeight: 48
                    text: auditManager && auditManager.verifyingIntegrity ? "Verifying..." : "Verify Integrity"
                    variant: "primary"
                    state: auditManager && auditManager.verifyingIntegrity ? "loading" : "default"
                    enabled: !(auditManager && auditManager.verifyingIntegrity) && dbusInterface && dbusInterface.serviceAvailable

                    onClicked: {
                        if (auditManager) {
                            auditManager.verifyIntegrity()
                        }
                    }
                }

                Text {
                    text: "Click to manually verify the integrity of all audit logs using cryptographic hash chains."
                    color: "#cccccc"
                    font.pixelSize: 12
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }
        }

        // Integrity information
        Card {
            Layout.fillWidth: true
            Layout.fillHeight: true
            variant: "dashboard-card"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                Text {
                    text: "Integrity Information"
                    color: "#ffffff"
                    font.pixelSize: 16
                    font.bold: true
                    Layout.fillWidth: true
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    Text {
                        id: integrityInfoText
                        text: "Audit logs use cryptographic hash chains (SHA-256) to ensure tamper-proof logging.\n\n" +
                              "Each log entry includes a hash of the previous entry, creating an unbreakable chain.\n\n" +
                              "If any log entry is modified, the hash chain will be broken and integrity verification will fail.\n\n" +
                              "Regular integrity checks are recommended to detect any unauthorized modifications."
                        color: "#cccccc"
                        font.pixelSize: 12
                        wrapMode: Text.WordWrap
                        width: parent.width
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        if (auditManager) {
            auditManager.verifyIntegrity()
        }
    }

    Connections {
        target: auditManager
        function onIntegrityViolationDetected() {
            // Show notification alert
        }
    }
}

