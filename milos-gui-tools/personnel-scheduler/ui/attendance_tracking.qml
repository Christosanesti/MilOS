import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: attendanceTracking
    color: "#0a0a0a"
    border.color: "#00ff88"
    border.width: 2
    radius: 5

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        Text {
            text: "Real-Time Attendance Tracking"
            font.pixelSize: 20
            font.bold: true
            color: "#00ff88"
        }

        RowLayout {
            Layout.fillWidth: true

            Button {
                text: "Record Entry"
                onClicked: {
                    // Record attendance entry
                    // This would call attendanceTracker.recordEntry()
                }
            }

            Button {
                text: "Record Exit"
                onClicked: {
                    // Record attendance exit
                    // This would call attendanceTracker.recordExit()
                }
            }

            Button {
                text: "Refresh"
                onClicked: {
                    // Refresh attendance records
                }
            }
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: attendanceList
                model: []  // Would be populated from attendanceTracker
                delegate: Rectangle {
                    width: attendanceList.width
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
                            text: modelData.event_type === 0 ? "Entry" : "Exit"
                            font.pixelSize: 12
                            color: "#ffffff"
                        }

                        Text {
                            text: modelData.timestamp || ""
                            font.pixelSize: 12
                            color: "#ffffff"
                        }

                        Text {
                            text: modelData.location || ""
                            font.pixelSize: 12
                            color: "#ffffff"
                        }
                    }
                }
            }
        }
    }
}

