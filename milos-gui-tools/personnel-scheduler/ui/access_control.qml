import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: accessControl
    color: "#0a0a0a"
    border.color: "#00ff88"
    border.width: 2
    radius: 5

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        Text {
            text: "Access Control"
            font.pixelSize: 20
            font.bold: true
            color: "#00ff88"
        }

        TabBar {
            id: tabBar
            Layout.fillWidth: true

            TabButton {
                text: "Permissions"
            }

            TabButton {
                text: "Roles"
            }

            TabButton {
                text: "Restrictions"
            }
        }

        StackLayout {
            currentIndex: tabBar.currentIndex
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Permissions tab
            ScrollView {
                ListView {
                    id: permissionsList
                    model: []  // Would be populated from accessControl
                    delegate: Rectangle {
                        width: permissionsList.width
                        height: 80
                        color: index % 2 === 0 ? "#1a1a1a" : "#0a0a0a"
                        border.color: "#00ff88"
                        border.width: 1

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 10

                            Text {
                                text: modelData.personnel_id || "Unknown"
                                font.pixelSize: 14
                                color: "#00ff88"
                                Layout.fillWidth: true
                            }

                            Text {
                                text: modelData.location || ""
                                font.pixelSize: 12
                                color: "#ffffff"
                            }

                            Text {
                                text: modelData.enabled ? "Enabled" : "Disabled"
                                font.pixelSize: 12
                                color: modelData.enabled ? "#00ff88" : "#ff0000"
                            }
                        }
                    }
                }
            }

            // Roles tab
            ScrollView {
                ListView {
                    id: rolesList
                    model: []  // Would be populated from roleManager
                    delegate: Rectangle {
                        width: rolesList.width
                        height: 60
                        color: index % 2 === 0 ? "#1a1a1a" : "#0a0a0a"
                        border.color: "#00ff88"
                        border.width: 1

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 10

                            Text {
                                text: modelData.personnel_id || "Unknown"
                                font.pixelSize: 14
                                color: "#00ff88"
                                Layout.fillWidth: true
                            }

                            Text {
                                text: modelData.role || "Guest"
                                font.pixelSize: 12
                                color: "#ffffff"
                            }
                        }
                    }
                }
            }

            // Restrictions tab
            ScrollView {
                ListView {
                    id: restrictionsList
                    model: []  // Would be populated from restrictionsManager
                    delegate: Rectangle {
                        width: restrictionsList.width
                        height: 80
                        color: index % 2 === 0 ? "#1a1a1a" : "#0a0a0a"
                        border.color: "#00ff88"
                        border.width: 1

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 10

                            Text {
                                text: "Restriction: " + (modelData.personnel_id || "Unknown")
                                font.pixelSize: 14
                                color: "#00ff88"
                            }

                            Text {
                                text: "Location: " + (modelData.location || "")
                                font.pixelSize: 12
                                color: "#ffffff"
                            }
                        }
                    }
                }
            }
        }
    }
}

