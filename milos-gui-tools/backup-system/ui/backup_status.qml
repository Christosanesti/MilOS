import QtQuick 2.15
import QtQuick.Controls 2.15
import org.kde.plasma.core 2.0 as PlasmaCore

/**
 * Backup Status Component
 * 
 * Displays backup system status with xenon-accented indicators.
 */
Rectangle {
    id: backupStatus
    property var backupService: null

    color: "#1a1a1a"
    border.color: "#00ffff"
    border.width: 2
    radius: 4

    RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 15

        // Status icon
        Rectangle {
            width: 20
            height: 20
            radius: 10
            color: backupService && backupService.currentBackupStatus === "Ready" ? "#00ffff" : "#888888"
        }

        Column {
            Layout.fillWidth: true
            spacing: 5

            Text {
                text: backupService ? backupService.currentBackupStatus : "Not Available"
                color: backupService && backupService.currentBackupStatus === "Ready" ? "#00ffff" : "#888888"
                font.pixelSize: 14
                font.bold: true
            }

            Text {
                text: backupService ? "Progress: " + backupService.backupProgress.toFixed(1) + "%" : "Progress: N/A"
                color: "#888888"
                font.pixelSize: 11
            }
        }
    }
}

