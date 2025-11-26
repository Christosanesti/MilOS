import QtQuick 2.15
import QtQuick.Controls 2.15
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.plasmoid 2.0

PlasmoidItem {
    id: root
    
    Plasmoid.backgroundHints: PlasmaCore.Types.ShadowBackground
    Plasmoid.preferredRepresentation: Plasmoid.compactRepresentation
    
    AnonymityController {
        id: controller
        onStatusChanged: {
            // Update widget display
        }
    }
    
    compactRepresentation: Item {
        width: PlasmaCore.Units.iconSizes.medium
        height: PlasmaCore.Units.iconSizes.medium
        
        PlasmaCore.IconItem {
            id: icon
            anchors.fill: parent
            source: controller.torConnected || controller.i2pConnected ? "network-connect" : "network-disconnect"
            
            // Xenon pulse effect
            SequentialAnimation on opacity {
                running: controller.torConnected || controller.i2pConnected
                loops: Animation.Infinite
                NumberAnimation {
                    from: 0.6
                    to: 1.0
                    duration: 2000
                }
                NumberAnimation {
                    from: 1.0
                    to: 0.6
                    duration: 2000
                }
            }
        }
        
        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (controller.torConnected) {
                    controller.toggleTor()
                } else {
                    controller.toggleTor()
                }
            }
        }
    }
    
    fullRepresentation: Item {
        Column {
            anchors.centerIn: parent
            spacing: PlasmaCore.Units.smallSpacing
            
            Text {
                text: controller.connectionStatus
                color: PlasmaCore.Theme.textColor
            }
            
            Button {
                text: "Toggle Tor"
                onClicked: controller.toggleTor()
            }
            
            Button {
                text: "Toggle I2P"
                onClicked: controller.toggleI2P()
            }
        }
    }
    
    Plasmoid.contextualActions: [
        PlasmaCore.Action {
            text: "Toggle Tor"
            onTriggered: controller.toggleTor()
        },
        PlasmaCore.Action {
            text: "Toggle I2P"
            onTriggered: controller.toggleI2P()
        }
    ]
}

