import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: keyManagement
    color: "#0a0a0a"
    border.color: "#00ff88"
    border.width: 2
    radius: 5

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        Text {
            text: "USB Key Management"
            font.pixelSize: 20
            font.bold: true
            color: "#00ff88"
        }

        TabBar {
            id: tabBar
            Layout.fillWidth: true

            TabButton {
                text: "Keys"
            }

            TabButton {
                text: "Users"
            }

            TabButton {
                text: "Devices"
            }
        }

        StackLayout {
            currentIndex: tabBar.currentIndex
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Keys tab
            ScrollView {
                ListView {
                    id: keysList
                    model: []  // Would be populated from keyMgr
                    delegate: Rectangle {
                        width: keysList.width
                        height: 80
                        color: index % 2 === 0 ? "#1a1a1a" : "#0a0a0a"
                        border.color: "#00ff88"
                        border.width: 1

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 10

                            Text {
                                text: "Key ID: " + (modelData.key_id || "Unknown")
                                font.pixelSize: 14
                                color: "#00ff88"
                            }

                            Text {
                                text: "User: " + (modelData.user_id || "")
                                font.pixelSize: 12
                                color: "#ffffff"
                            }

                            Text {
                                text: "Status: " + (modelData.status || "")
                                font.pixelSize: 12
                                color: "#ffffff"
                            }
                        }
                    }
                }
            }

            // Users tab
            ScrollView {
                ListView {
                    id: usersList
                    model: []  // Would be populated from enrollment
                    delegate: Rectangle {
                        width: usersList.width
                        height: 60
                        color: index % 2 === 0 ? "#1a1a1a" : "#0a0a0a"
                        border.color: "#00ff88"
                        border.width: 1

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 10

                            Text {
                                text: modelData.username || "Unknown"
                                font.pixelSize: 14
                                color: "#00ff88"
                                Layout.fillWidth: true
                            }

                            Text {
                                text: modelData.role || ""
                                font.pixelSize: 12
                                color: "#ffffff"
                            }
                        }
                    }
                }
            }

            // Devices tab
            ScrollView {
                ListView {
                    id: devicesList
                    model: []  // Would be populated from usbAuth
                    delegate: Rectangle {
                        width: devicesList.width
                        height: 60
                        color: index % 2 === 0 ? "#1a1a1a" : "#0a0a0a"
                        border.color: "#00ff88"
                        border.width: 1

                        Text {
                            text: modelData || "Unknown"
                            font.pixelSize: 14
                            color: "#00ff88"
                            anchors.fill: parent
                            anchors.margins: 10
                        }
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true

            Button {
                text: "Generate Key"
                onClicked: {
                    // Generate key
                }
            }

            Button {
                text: "Detect USB"
                onClicked: {
                    // Detect USB devices
                }
            }
        }
    }
}

