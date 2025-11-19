import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: calendar
    color: "#0a0a0a"
    border.color: "#00ff88"
    border.width: 2
    radius: 5

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        Text {
            text: "Shift Scheduling Calendar"
            font.pixelSize: 20
            font.bold: true
            color: "#00ff88"
        }

        RowLayout {
            Layout.fillWidth: true

            Button {
                text: "Previous Week"
                onClicked: {
                    // Navigate to previous week
                }
            }

            Text {
                text: "Week View"
                font.pixelSize: 16
                color: "#ffffff"
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }

            Button {
                text: "Next Week"
                onClicked: {
                    // Navigate to next week
                }
            }
        }

        GridLayout {
            columns: 7
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Day headers
            Repeater {
                model: ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"]
                Text {
                    text: modelData
                    font.pixelSize: 14
                    font.bold: true
                    color: "#00ff88"
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            // Calendar cells (simplified - would show shifts)
            Repeater {
                model: 35  // 5 weeks * 7 days
                Rectangle {
                    width: 100
                    height: 80
                    color: "#1a1a1a"
                    border.color: "#00ff88"
                    border.width: 1

                    Text {
                        anchors.centerIn: parent
                        text: "Day " + (index + 1)
                        font.pixelSize: 12
                        color: "#ffffff"
                    }
                }
            }
        }
    }
}

