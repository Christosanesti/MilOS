import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents

/**
 * Clipboard View Component
 * 
 * Displays current clipboard content.
 */
Rectangle {
    id: clipboardView
    property var clipboardService: null

    color: "#0a0a0a"
    border.color: "#00ffff"
    border.width: 1
    radius: 4

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        Text {
            text: "Current Clipboard"
            color: "#00ffff"
            font.pixelSize: 12
            font.bold: true
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            TextArea {
                id: clipboardText
                readOnly: true
                text: clipboardService ? clipboardService.currentClipboard : ""
                color: "#ffffff"
                background: Rectangle {
                    color: "#1a1a1a"
                    border.color: "#333333"
                    border.width: 1
                    radius: 2
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            PlasmaComponents.Button {
                text: "Clear"
                Layout.fillWidth: true
                onClicked: {
                    if (clipboardService) {
                        clipboardService.clearClipboard();
                    }
                }
            }
        }
    }
}

