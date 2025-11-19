import QtQuick 2.15
import QtQuick.Controls 2.15
import MilosComponents 1.0

Rectangle {
    id: updateCard
    color: "#1a1a1a"
    radius: 8
    border.color: "#00ff88"
    border.width: 2
    
    property var status: ({})
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 10
        
        Text {
            text: "Update Status"
            color: "#ffffff"
            font.bold: true
            font.pixelSize: 18
        }
        
        Text {
            text: "Up to Date: " + (status.upToDate ? "Yes" : "No")
            color: status.upToDate ? "#00ff88" : "#ff4444"
            font.pixelSize: 14
        }
        
        Text {
            text: "Available Updates: " + (status.availableUpdates || 0)
            color: "#aaaaaa"
            font.pixelSize: 12
        }
        
        StatusIndicator {
            status: status.status || "Unknown"
            level: status.upToDate ? "high" : "low"
        }
    }
}

