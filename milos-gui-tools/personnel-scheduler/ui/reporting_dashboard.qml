import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: reportingDashboard
    color: "#0a0a0a"
    border.color: "#00ff88"
    border.width: 2
    radius: 5

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        Text {
            text: "Reporting Dashboard"
            font.pixelSize: 20
            font.bold: true
            color: "#00ff88"
        }

        RowLayout {
            Layout.fillWidth: true

            ComboBox {
                id: reportTypeCombo
                model: ["Daily", "Weekly", "Monthly", "Custom"]
                Layout.preferredWidth: 150
            }

            TextField {
                id: startDatePicker
                placeholderText: "Start Date (ISO)"
                Layout.preferredWidth: 150
            }

            TextField {
                id: endDatePicker
                placeholderText: "End Date (ISO)"
                Layout.preferredWidth: 150
            }

            Button {
                text: "Generate Report"
                onClicked: {
                    // Generate report
                }
            }

            Button {
                text: "Export"
                onClicked: {
                    // Export report
                }
            }
        }

        TabBar {
            id: tabBar
            Layout.fillWidth: true

            TabButton {
                text: "Attendance"
            }

            TabButton {
                text: "Access Control"
            }

            TabButton {
                text: "Compliance"
            }
        }

        StackLayout {
            currentIndex: tabBar.currentIndex
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Attendance tab
            ScrollView {
                ListView {
                    id: attendanceReportList
                    model: []  // Would be populated from reportGenerator
                    delegate: Rectangle {
                        width: attendanceReportList.width
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
                                text: modelData.timestamp || ""
                                font.pixelSize: 12
                                color: "#ffffff"
                            }
                        }
                    }
                }
            }

            // Access Control tab
            ScrollView {
                ListView {
                    id: accessLogList
                    model: []  // Would be populated from reportGenerator
                    delegate: Rectangle {
                        width: accessLogList.width
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
                                text: modelData.status || ""
                                font.pixelSize: 12
                                color: "#ffffff"
                            }
                        }
                    }
                }
            }

            // Compliance tab
            ScrollView {
                Text {
                    text: "Compliance Report"
                    font.pixelSize: 16
                    color: "#ffffff"
                }
            }
        }
    }
}

