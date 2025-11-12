import QtQuick 2.15
import QtQuick.Controls 2.15
import org.kde.plasma.core 2.0 as PlasmaCore

/**
 * Status Indicator Component
 * 
 * Displays clipboard encryption status with xenon-accented indicators.
 */
Rectangle {
    id: statusIndicator
    property var clipboardService: null

    color: "#1a1a1a"
    border.color: clipboardService && clipboardService.isEncrypted ? "#00ffff" : "#888888"
    border.width: 2
    radius: 4

    // Xenon glow animation for encrypted status
    SequentialAnimation on border.width {
        running: clipboardService && clipboardService.isEncrypted
        loops: Animation.Infinite
        NumberAnimation {
            from: 2
            to: 4
            duration: 1000
        }
        NumberAnimation {
            from: 4
            to: 2
            duration: 1000
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 15

        // Encryption status icon
        Rectangle {
            width: 20
            height: 20
            radius: 10
            color: clipboardService && clipboardService.isEncrypted ? "#00ffff" : "#888888"
            
            // Xenon glow
            Rectangle {
                anchors.fill: parent
                color: "transparent"
                border.color: clipboardService && clipboardService.isEncrypted ? "#00ffff" : "#888888"
                border.width: 2
                radius: 10
                opacity: 0.5
            }
        }

        Column {
            Layout.fillWidth: true
            spacing: 5

            Text {
                text: clipboardService && clipboardService.isEncrypted ? "Encrypted" : "Not Encrypted"
                color: clipboardService && clipboardService.isEncrypted ? "#00ffff" : "#888888"
                font.pixelSize: 14
                font.bold: true
            }

            Text {
                text: clipboardService ? "Timeout: " + clipboardService.timeoutSeconds + " seconds" : "Timeout: N/A"
                color: "#888888"
                font.pixelSize: 11
            }
        }
    }
}

