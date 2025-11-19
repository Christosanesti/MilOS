import QtQuick 2.15
import QtQuick.Controls 2.15
import MilosComponents 1.0

Rectangle {
    id: postureCard
    color: "#1a1a1a"
    radius: 8
    border.color: "#00ff88"
    border.width: 2
    
    property var status: ({})
    
    RowLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20
        
        // Security score
        ColumnLayout {
            spacing: 10
            
            Text {
                text: "Security Score"
                color: "#aaaaaa"
                font.pixelSize: 14
            }
            
            Text {
                text: (status.score || 0) + "/100"
                color: "#00ff88"
                font.bold: true
                font.pixelSize: 48
            }
        }
        
        // Status indicator
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 10
            
            Text {
                text: "Overall Status"
                color: "#aaaaaa"
                font.pixelSize: 14
            }
            
            StatusIndicator {
                status: status.status || "Unknown"
                level: status.level || "low"
            }
        }
    }
}

