import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents

ApplicationWindow {
    id: mainWindow
    width: 600
    height: 500
    visible: true
    title: "MilOS Secure Clipboard Manager"

    color: "#0a0a0a"

    // Clipboard Service backend
    property var clipboardService: null

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        // Title
        Text {
            text: "Secure Clipboard Manager"
            color: "#00ffff"
            font.pixelSize: 20
            font.bold: true
            Layout.fillWidth: true
        }

        // Status Indicator
        StatusIndicator {
            id: statusIndicator
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            clipboardService: mainWindow.clipboardService
        }

        // Current Clipboard Display
        ClipboardView {
            id: clipboardView
            Layout.fillWidth: true
            Layout.preferredHeight: 100
            clipboardService: mainWindow.clipboardService
        }

        // Clipboard History
        ClipboardHistory {
            id: clipboardHistory
            Layout.fillWidth: true
            Layout.fillHeight: true
            clipboardService: mainWindow.clipboardService
        }
    }
}

