import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents

ApplicationWindow {
    id: mainWindow
    width: 800
    height: 600
    visible: true
    title: "MilOS Zero-Knowledge Backup System"

    color: "#0a0a0a"

    // Backup Service backend
    property var backupService: null

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        // Title
        Text {
            text: "Zero-Knowledge Backup System"
            color: "#00ffff"
            font.pixelSize: 20
            font.bold: true
            Layout.fillWidth: true
        }

        // Status
        BackupStatus {
            id: backupStatus
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            backupService: mainWindow.backupService
        }

        // Backup List
        BackupList {
            id: backupList
            Layout.fillWidth: true
            Layout.fillHeight: true
            backupService: mainWindow.backupService
        }
    }
}

