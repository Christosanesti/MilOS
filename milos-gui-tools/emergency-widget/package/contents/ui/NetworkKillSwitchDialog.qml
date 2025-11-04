import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.core 2.1 as PlasmaCore
import org.kde.kirigami 2.20 as Kirigami

Dialog {
    id: confirmationDialog
    
    property var networkKillSwitch: null
    
    title: "Confirm Network Kill Switch"
    modal: true
    width: units.gridUnit * 25
    height: units.gridUnit * 15
    
    // Xenon warning border
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.color: "#FF6B6B"  // Warning red
        border.width: 3
        radius: units.smallSpacing
        
        // Glow effect (xenon styling)
        SequentialAnimation on opacity {
            running: true
            loops: Animation.Infinite
            NumberAnimation { from: 0.6; to: 1.0; duration: 1000 }
            NumberAnimation { from: 1.0; to: 0.6; duration: 1000 }
        }
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: units.largeSpacing
        spacing: units.mediumSpacing
        
        PlasmaCore.IconItem {
            source: "network-disconnect"
            width: units.iconSizes.huge
            height: units.iconSizes.huge
            Layout.alignment: Qt.AlignHCenter
        }
        
        Label {
            text: "WARNING: Network Kill Switch"
            font.pixelSize: units.fontSizes.large
            font.bold: true
            color: "#FF6B6B"
            Layout.alignment: Qt.AlignHCenter
        }
        
        Label {
            text: "This action will disable ALL network interfaces.\n\n" +
                  "You will lose all network connectivity until you manually re-enable network interfaces.\n\n" +
                  "Are you sure you want to proceed?"
            font.pixelSize: units.fontSizes.medium
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
        }
        
        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            spacing: units.mediumSpacing
            
            Button {
                text: "Cancel"
                Layout.preferredWidth: units.gridUnit * 8
                onClicked: confirmationDialog.reject()
            }
            
            Button {
                text: "Disable Network"
                Layout.preferredWidth: units.gridUnit * 8
                // Xenon-accented button
                background: Rectangle {
                    color: "#FF6B6B"
                    radius: units.smallSpacing
                    border.color: "#00FFFF"
                    border.width: 2
                }
                onClicked: {
                    if (networkKillSwitch) {
                        networkKillSwitch.disableNetwork();
                    }
                    confirmationDialog.accept();
                }
            }
        }
    }
}

