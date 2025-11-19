import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

ApplicationWindow {
    id: mainWindow
    width: 1600
    height: 1000
    visible: true
    title: "MilOS Security Status Dashboard"
    
    color: "#0a0a0a"
    
    // Main layout
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20
        
        // Header
        RowLayout {
            Layout.fillWidth: true
            
            Text {
                text: "Security Status Dashboard"
                color: "#ffffff"
                font.bold: true
                font.pixelSize: 24
            }
            
            Item {
                Layout.fillWidth: true
            }
            
            Button {
                text: "Refresh"
                onClicked: {
                    statusAggregator.refreshAll()
                }
            }
        }
        
        // Security Posture Card
        SecurityPostureCard {
            Layout.fillWidth: true
            height: 150
            status: statusAggregator.getSecurityPosture()
        }
        
        // Status cards grid
        GridLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            columns: 2
            rowSpacing: 20
            columnSpacing: 20
            
            HardeningStatusCard {
                Layout.fillWidth: true
                Layout.fillHeight: true
                status: statusAggregator.getHardeningStatus()
            }
            
            EncryptionStatusCard {
                Layout.fillWidth: true
                Layout.fillHeight: true
                status: statusAggregator.getEncryptionStatus()
            }
            
            NetworkSecurityStatusCard {
                Layout.fillWidth: true
                Layout.fillHeight: true
                status: statusAggregator.getNetworkSecurityStatus()
            }
            
            UpdateStatusCard {
                Layout.fillWidth: true
                Layout.fillHeight: true
                status: statusAggregator.getUpdateStatus()
            }
        }
    }
}

