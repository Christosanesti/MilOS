import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: analyticsDashboard
    color: "#0a0a0a"
    border.color: "#00ff88"
    border.width: 2
    radius: 5

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        Text {
            text: "Analytics Dashboard"
            font.pixelSize: 20
            font.bold: true
            color: "#00ff88"
        }

        GridLayout {
            columns: 2
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Current Attendance Widget
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 150
                color: "#1a1a1a"
                border.color: "#00ff88"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10

                    Text {
                        text: "Current Attendance"
                        font.pixelSize: 14
                        font.bold: true
                        color: "#00ff88"
                    }

                    Text {
                        text: "0"
                        font.pixelSize: 32
                        color: "#ffffff"
                    }
                }
            }

            // Today's Entries Widget
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 150
                color: "#1a1a1a"
                border.color: "#00ff88"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10

                    Text {
                        text: "Today's Entries"
                        font.pixelSize: 14
                        font.bold: true
                        color: "#00ff88"
                    }

                    Text {
                        text: "0"
                        font.pixelSize: 32
                        color: "#ffffff"
                    }
                }
            }

            // Pending Requests Widget
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 150
                color: "#1a1a1a"
                border.color: "#00ff88"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10

                    Text {
                        text: "Pending Leave Requests"
                        font.pixelSize: 14
                        font.bold: true
                        color: "#00ff88"
                    }

                    Text {
                        text: "0"
                        font.pixelSize: 32
                        color: "#ffffff"
                    }
                }
            }

            // Active Shifts Widget
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 150
                color: "#1a1a1a"
                border.color: "#00ff88"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10

                    Text {
                        text: "Active Shifts"
                        font.pixelSize: 14
                        font.bold: true
                        color: "#00ff88"
                    }

                    Text {
                        text: "0"
                        font.pixelSize: 32
                        color: "#ffffff"
                    }
                }
            }
        }

        Button {
            text: "Refresh"
            Layout.fillWidth: true
            onClicked: {
                // Refresh dashboard data
            }
        }
    }
}

