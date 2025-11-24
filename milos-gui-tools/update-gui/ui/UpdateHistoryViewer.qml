import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: updateHistoryViewer
    color: "transparent"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // Header with refresh button
        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Text {
                text: "Update History"
                color: "#ffffff"
                font.pixelSize: 20
                font.bold: true
                Layout.fillWidth: true
            }

            Button {
                id: refreshButton
                Layout.preferredWidth: 120
                Layout.preferredHeight: 40
                text: "Refresh"
                variant: "secondary"
                enabled: !(updateManager && updateManager.checkingUpdates) && 
                         dbusInterface && dbusInterface.serviceAvailable

                onClicked: {
                    if (updateManager) {
                        updateManager.refreshHistory()
                    }
                }
            }
        }

        // History list
        Card {
            id: historyCard
            Layout.fillWidth: true
            Layout.fillHeight: true
            variant: "dashboard-card"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    ListView {
                        id: historyList
                        model: updateManager ? updateManager.updateHistory : []
                        spacing: 12

                        delegate: Card {
                            width: historyList.width
                            height: 120
                            variant: "detail-card"

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 16
                                spacing: 8

                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 12

                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 4

                                        Text {
                                            text: "Update ID: " + (modelData.update_id || "Unknown")
                                            color: "#ffffff"
                                            font.pixelSize: 14
                                            font.bold: true
                                            Layout.fillWidth: true
                                        }

                                        Text {
                                            text: "Date: " + (modelData.timestamp || "Unknown")
                                            color: "#b0b0b0"
                                            font.pixelSize: 12
                                            Layout.fillWidth: true
                                        }

                                        Text {
                                            text: "Packages: " + (modelData.package_list ? modelData.package_list.length : 0)
                                            color: "#b0b0b0"
                                            font.pixelSize: 12
                                            Layout.fillWidth: true
                                        }
                                    }

                                    StatusIndicator {
                                        variant: "status-badge"
                                        state: getStatusState(modelData.status)
                                        text: modelData.status || "Unknown"
                                    }
                                }

                                // Package list
                                Text {
                                    text: modelData.package_list ? modelData.package_list.join(", ") : ""
                                    color: "#e0e0e0"
                                    font.pixelSize: 11
                                    Layout.fillWidth: true
                                    wrapMode: Text.Wrap
                                    visible: modelData.package_list && modelData.package_list.length > 0
                                }
                            }
                        }

                        // Empty state
                        Text {
                            anchors.centerIn: parent
                            text: "No update history available"
                            color: "#b0b0b0"
                            font.pixelSize: 14
                            visible: historyList.count === 0
                        }
                    }
                }
            }
        }
    }

    function getStatusState(status) {
        switch(status) {
            case "COMPLETED": return "active"
            case "FAILED": return "critical"
            case "IN_PROGRESS": return "processing"
            case "ROLLED_BACK": return "warning"
            default: return "inactive"
        }
    }

    Component.onCompleted: {
        if (updateManager) {
            updateManager.refreshHistory()
        }
    }
}


