import QtQuick 2.15
import QtQuick.Controls 2.15
import MilosComponents 1.0

Rectangle {
    id: hardeningCard
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
            text: "Hardening Status"
            color: "#ffffff"
            font.bold: true
            font.pixelSize: 18
        }
        
        Text {
            text: "Score: " + (status.score || 0) + "/100"
            color: "#00ff88"
            font.pixelSize: 14
        }
        
        Text {
            text: "Passed: " + (status.passedChecks || 0) + " / " + (status.totalChecks || 0)
            color: "#aaaaaa"
            font.pixelSize: 12
        }
        
        StatusIndicator {
            status: status.status || "Unknown"
            level: status.score >= 80 ? "high" : (status.score >= 60 ? "medium" : "low")
        }
    }
}

