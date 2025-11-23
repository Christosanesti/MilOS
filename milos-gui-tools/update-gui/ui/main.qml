import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

ApplicationWindow {
    id: mainWindow
    width: 1200
    height: 800
    visible: true
    title: "MilOS Update Manager"

    color: "#0a0a0a"

    // Main layout with tabs
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: 16

            Text {
                text: "MilOS Update Manager"
                color: "#00ffff"
                font.pixelSize: 24
                font.bold: true
                Layout.fillWidth: true
            }

            // Service status indicator
            StatusIndicator {
                id: serviceStatusIndicator
                Layout.preferredWidth: 200
                Layout.preferredHeight: 40
                variant: "status-light"
                state: dbusInterface && dbusInterface.serviceAvailable ? "active" : "critical"
                text: dbusInterface && dbusInterface.serviceAvailable ? "Service Available" : "Service Unavailable"
            }
        }

        // Tab navigation
        TabNavigation {
            id: tabNavigation
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            tabs: ["Check Updates", "Install Updates", "History", "Rollback", "Status"]
            currentIndex: 0

            onTabChanged: {
                currentIndex = index
                contentLoader.source = getTabComponent(index)
            }
        }

        // Content area
        Loader {
            id: contentLoader
            Layout.fillWidth: true
            Layout.fillHeight: true
            source: "UpdateCheckInterface.qml"
        }
    }

    function getTabComponent(index) {
        switch(index) {
            case 0: return "UpdateCheckInterface.qml"
            case 1: return "UpdateInstallationInterface.qml"
            case 2: return "UpdateHistoryViewer.qml"
            case 3: return "RollbackInterface.qml"
            case 4: return "ServiceStatusDashboard.qml"
            default: return "UpdateCheckInterface.qml"
        }
    }
}

