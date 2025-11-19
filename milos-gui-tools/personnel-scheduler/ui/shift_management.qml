import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: shiftManagement
    color: "#0a0a0a"
    border.color: "#00ff88"
    border.width: 2
    radius: 5

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        Text {
            text: "Shift Management"
            font.pixelSize: 20
            font.bold: true
            color: "#00ff88"
        }

        TabBar {
            id: tabBar
            Layout.fillWidth: true

            TabButton {
                text: "Shifts"
            }

            TabButton {
                text: "Leave Requests"
            }

            TabButton {
                text: "Swap Requests"
            }

            TabButton {
                text: "Coverage"
            }
        }

        StackLayout {
            currentIndex: tabBar.currentIndex
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Shifts tab
            ScrollView {
                ListView {
                    id: shiftsList
                    model: []  // Would be populated from shiftScheduler
                    delegate: Rectangle {
                        width: shiftsList.width
                        height: 100
                        color: index % 2 === 0 ? "#1a1a1a" : "#0a0a0a"
                        border.color: "#00ff88"
                        border.width: 1

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 10

                            Text {
                                text: "Shift: " + (modelData.personnel_id || "Unassigned")
                                font.pixelSize: 14
                                color: "#00ff88"
                            }

                            Text {
                                text: (modelData.start_date_time || "") + " - " + (modelData.end_date_time || "")
                                font.pixelSize: 12
                                color: "#ffffff"
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

            // Leave Requests tab
            ScrollView {
                ListView {
                    id: leaveList
                    model: []  // Would be populated from leaveManager
                    delegate: Rectangle {
                        width: leaveList.width
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
                                text: modelData.status === 0 ? "Pending" : (modelData.status === 1 ? "Approved" : "Rejected")
                                font.pixelSize: 12
                                color: "#ffffff"
                            }
                        }
                    }
                }
            }

            // Swap Requests tab
            ScrollView {
                ListView {
                    id: swapList
                    model: []  // Would be populated from swapManager
                    delegate: Rectangle {
                        width: swapList.width
                        height: 80
                        color: index % 2 === 0 ? "#1a1a1a" : "#0a0a0a"
                        border.color: "#00ff88"
                        border.width: 1

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 10

                            Text {
                                text: "Swap: " + (modelData.requester_id || "") + " ↔ " + (modelData.target_personnel_id || "")
                                font.pixelSize: 14
                                color: "#00ff88"
                                Layout.fillWidth: true
                            }

                            Text {
                                text: modelData.status === 0 ? "Pending" : (modelData.status === 1 ? "Approved" : "Rejected")
                                font.pixelSize: 12
                                color: "#ffffff"
                            }
                        }
                    }
                }
            }

            // Coverage tab
            ScrollView {
                ListView {
                    id: coverageList
                    model: []  // Would be populated from coverageManager
                    delegate: Rectangle {
                        width: coverageList.width
                        height: 80
                        color: index % 2 === 0 ? "#1a1a1a" : "#0a0a0a"
                        border.color: "#00ff88"
                        border.width: 1

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 10

                            Text {
                                text: "Coverage: " + (modelData.shift_id || "")
                                font.pixelSize: 14
                                color: "#00ff88"
                                Layout.fillWidth: true
                            }

                            Text {
                                text: modelData.fulfilled ? "Fulfilled" : "Pending"
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

