import QtQuick 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.1 as PlasmaCore

PlasmoidItem {
    id: root
    
    // Plasmoid configuration
    Plasmoid.preferredRepresentation: Plasmoid.compactRepresentation
    Plasmoid.toolTipSubText: "Emergency Actions Widget"
    Plasmoid.backgroundHints: PlasmaCore.Types.DefaultBackground | PlasmaCore.Types.ConfigurableBackground
    
    // Compact representation (icon-only mode)
    compactRepresentation: Item {
        id: compactRoot
        
        PlasmoidIcon {
            id: icon
            source: "security-high"  // KDE system icon
            anchors.fill: parent
        }
        
        MouseArea {
            anchors.fill: parent
            onClicked: {
                Plasmoid.expanded = !Plasmoid.expanded;
            }
        }
    }
    
    // Full representation (expanded mode with buttons)
    fullRepresentation: ColumnLayout {
        id: fullRoot
        spacing: units.smallSpacing
        
        // 4 Emergency Action Buttons
        EmergencyButton {
            id: networkButton
            Layout.fillWidth: true
            buttonText: "Disable Network"
            buttonIcon: "network-disconnect"
            buttonColor: "#FF6B6B"
            onClicked: {
                // TODO: Implement network kill switch (Story 2.2)
                console.log("Disable Network clicked");
            }
        }
        
        EmergencyButton {
            id: lockButton
            Layout.fillWidth: true
            buttonText: "Lock System"
            buttonIcon: "system-lock-screen"
            buttonColor: "#4ECDC4"
            onClicked: {
                // TODO: Implement screen lock (Story 2.3)
                console.log("Lock System clicked");
            }
        }
        
        EmergencyButton {
            id: secureModeButton
            Layout.fillWidth: true
            buttonText: "Secure Mode"
            buttonIcon: "security-high"
            buttonColor: "#FFE66D"
            onClicked: {
                // TODO: Implement secure mode (Story 2.4)
                console.log("Secure Mode clicked");
            }
        }
        
        EmergencyButton {
            id: shutdownButton
            Layout.fillWidth: true
            buttonText: "Emergency Shutdown"
            buttonIcon: "system-shutdown"
            buttonColor: "#FF6B6B"
            onClicked: {
                // TODO: Implement emergency shutdown (Story 2.5)
                console.log("Emergency Shutdown clicked");
            }
        }
    }
    
    // Switch between compact and full representation
    switchWidth: {
        if (Plasmoid.formFactor === PlasmaCore.Types.Horizontal) {
            return units.gridUnit * 10;
        } else {
            return units.gridUnit * 10;
        }
    }
    
    switchHeight: {
        if (Plasmoid.formFactor === PlasmaCore.Types.Horizontal) {
            return units.gridUnit * 10;
        } else {
            return units.gridUnit * 10;
        }
    }
}
