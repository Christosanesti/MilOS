import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    anchors.fill: parent

    property var dashboardData: adminDashboard ? adminDashboard.getDashboardData() : ({})

    Rectangle {
        anchors.fill: parent
        color: "#0a0a0a"

        ScrollView {
            anchors.fill: parent
            anchors.margins: 20

            ColumnLayout {
                width: parent.width
                spacing: 20

                // Header
                Text {
                    text: "Commanding Officer Dashboard"
                    font.pixelSize: 28
                    font.bold: true
                    color: "#ffffff"
                    Layout.fillWidth: true
                }

                // System Statistics Section
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 200
                    color: "#1a1a1a"
                    radius: 8
                    border.color: "#333333"
                    border.width: 1

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 15
                        spacing: 10

                        Text {
                            text: "System Statistics"
                            font.pixelSize: 18
                            font.bold: true
                            color: "#ffffff"
                        }

                        GridLayout {
                            columns: 2
                            rowSpacing: 10
                            columnSpacing: 20

                            Text {
                                text: "Active Users:"
                                color: "#aaaaaa"
                            }
                            Text {
                                text: dashboardData.activeUsers || "0"
                                color: "#00ff00"
                                font.bold: true
                            }

                            Text {
                                text: "Total Messages:"
                                color: "#aaaaaa"
                            }
                            Text {
                                text: dashboardData.totalMessages || "0"
                                color: "#00ff00"
                                font.bold: true
                            }

                            Text {
                                text: "Network Peers:"
                                color: "#aaaaaa"
                            }
                            Text {
                                text: dashboardData.networkPeers || "0"
                                color: "#00ff00"
                                font.bold: true
                            }

                            Text {
                                text: "Active Conversations:"
                                color: "#aaaaaa"
                            }
                            Text {
                                text: dashboardData.activeConversations || "0"
                                color: "#00ff00"
                                font.bold: true
                            }
                        }
                    }
                }

                // Emergency Controls Section
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 300
                    color: "#1a1a1a"
                    radius: 8
                    border.color: "#ff0000"
                    border.width: 2

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 15
                        spacing: 15

                        Text {
                            text: "Emergency Controls"
                            font.pixelSize: 18
                            font.bold: true
                            color: "#ff0000"
                        }

                        Text {
                            text: "WARNING: These actions are irreversible and will delete all data!"
                            color: "#ff6666"
                            font.pixelSize: 12
                            wrapMode: Text.WordWrap
                        }

                        RowLayout {
                            spacing: 10

                            Button {
                                text: "Emergency Eject"
                                Layout.preferredWidth: 200
                                background: Rectangle {
                                    color: parent.pressed ? "#cc0000" : "#ff0000"
                                    radius: 4
                                }
                                contentItem: Text {
                                    text: parent.text
                                    color: "#ffffff"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                onClicked: {
                                    ejectDialog.open()
                                }
                            }

                            Button {
                                text: "Data Wipe"
                                Layout.preferredWidth: 200
                                background: Rectangle {
                                    color: parent.pressed ? "#cc6600" : "#ff8800"
                                    radius: 4
                                }
                                contentItem: Text {
                                    text: parent.text
                                    color: "#ffffff"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                onClicked: {
                                    wipeDialog.open()
                                }
                            }

                            Button {
                                text: "Emergency Shutdown"
                                Layout.preferredWidth: 200
                                background: Rectangle {
                                    color: parent.pressed ? "#cc0000" : "#ff0000"
                                    radius: 4
                                }
                                contentItem: Text {
                                    text: parent.text
                                    color: "#ffffff"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                onClicked: {
                                    shutdownDialog.open()
                                }
                            }
                        }
                    }
                }

                // Network Health Section
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 150
                    color: "#1a1a1a"
                    radius: 8
                    border.color: "#333333"
                    border.width: 1

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 15
                        spacing: 10

                        Text {
                            text: "Network Health"
                            font.pixelSize: 18
                            font.bold: true
                            color: "#ffffff"
                        }

                        RowLayout {
                            spacing: 20

                            Text {
                                text: "Status:"
                                color: "#aaaaaa"
                            }
                            Text {
                                text: dashboardData.networkStatus || "Unknown"
                                color: dashboardData.networkStatus === "Healthy" ? "#00ff00" : "#ff8800"
                                font.bold: true
                            }

                            Text {
                                text: "Latency:"
                                color: "#aaaaaa"
                            }
                            Text {
                                text: (dashboardData.networkLatency || "0") + " ms"
                                color: "#00ff00"
                            }
                        }
                    }
                }

                // Security Status Section
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 150
                    color: "#1a1a1a"
                    radius: 8
                    border.color: "#333333"
                    border.width: 1

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 15
                        spacing: 10

                        Text {
                            text: "Security Status"
                            font.pixelSize: 18
                            font.bold: true
                            color: "#ffffff"
                        }

                        RowLayout {
                            spacing: 20

                            Text {
                                text: "E2E Encryption:"
                                color: "#aaaaaa"
                            }
                            Text {
                                text: dashboardData.e2eEnabled ? "Enabled" : "Disabled"
                                color: dashboardData.e2eEnabled ? "#00ff00" : "#ff0000"
                                font.bold: true
                            }

                            Text {
                                text: "Forward Secrecy:"
                                color: "#aaaaaa"
                            }
                            Text {
                                text: dashboardData.forwardSecrecyEnabled ? "Enabled" : "Disabled"
                                color: dashboardData.forwardSecrecyEnabled ? "#00ff00" : "#ff0000"
                                font.bold: true
                            }
                        }
                    }
                }
            }
        }
    }

    // Emergency Eject Dialog
    Dialog {
        id: ejectDialog
        title: "Emergency Eject"
        width: 400
        height: 200
        modal: true

        ColumnLayout {
            anchors.fill: parent
            spacing: 15

            Text {
                text: "WARNING: This will delete ALL data and dismantle the system!"
                color: "#ff0000"
                font.bold: true
                wrapMode: Text.WordWrap
            }

            TextField {
                id: ejectCodeField
                placeholderText: "Enter confirmation code: EMERGENCY_EJECT"
                Layout.fillWidth: true
            }

            RowLayout {
                Layout.fillWidth: true

                Button {
                    text: "Cancel"
                    Layout.fillWidth: true
                    onClicked: ejectDialog.close()
                }

                Button {
                    text: "Execute"
                    Layout.fillWidth: true
                    background: Rectangle {
                        color: parent.pressed ? "#cc0000" : "#ff0000"
                        radius: 4
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "#ffffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: {
                        if (ejectCodeField.text === "EMERGENCY_EJECT") {
                            if (emergencyEject) {
                                emergencyEject.executeEject(ejectCodeField.text)
                            }
                            ejectDialog.close()
                        } else {
                            ejectCodeField.text = ""
                            ejectCodeField.placeholderText = "Invalid code! Enter: EMERGENCY_EJECT"
                        }
                    }
                }
            }
        }
    }

    // Data Wipe Dialog
    Dialog {
        id: wipeDialog
        title: "Secure Data Wipe"
        width: 400
        height: 200
        modal: true

        ColumnLayout {
            anchors.fill: parent
            spacing: 15

            Text {
                text: "Select wipe method:"
                color: "#ffffff"
            }

            ComboBox {
                id: wipeTypeCombo
                Layout.fillWidth: true
                model: ["Cryptographic", "Overwrite", "Secure Delete"]
            }

            RowLayout {
                Layout.fillWidth: true

                Button {
                    text: "Cancel"
                    Layout.fillWidth: true
                    onClicked: wipeDialog.close()
                }

                Button {
                    text: "Execute"
                    Layout.fillWidth: true
                    background: Rectangle {
                        color: parent.pressed ? "#cc6600" : "#ff8800"
                        radius: 4
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "#ffffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: {
                        // Data wipe would be executed via D-Bus
                        wipeDialog.close()
                    }
                }
            }
        }
    }

    // Emergency Shutdown Dialog
    Dialog {
        id: shutdownDialog
        title: "Emergency Shutdown"
        width: 400
        height: 200
        modal: true

        ColumnLayout {
            anchors.fill: parent
            spacing: 15

            Text {
                text: "Enter shutdown reason:"
                color: "#ffffff"
            }

            TextField {
                id: shutdownReasonField
                placeholderText: "Reason for emergency shutdown"
                Layout.fillWidth: true
            }

            RowLayout {
                Layout.fillWidth: true

                Button {
                    text: "Cancel"
                    Layout.fillWidth: true
                    onClicked: shutdownDialog.close()
                }

                Button {
                    text: "Execute"
                    Layout.fillWidth: true
                    background: Rectangle {
                        color: parent.pressed ? "#cc0000" : "#ff0000"
                        radius: 4
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "#ffffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: {
                        if (shutdownReasonField.text.length > 0) {
                            // Emergency shutdown would be executed via D-Bus
                            shutdownDialog.close()
                        }
                    }
                }
            }
        }
    }
}

