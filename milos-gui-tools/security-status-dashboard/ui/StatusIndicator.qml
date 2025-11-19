import QtQuick 2.15
import QtQuick.Controls 2.15
import MilosComponents 1.0

Rectangle {
    id: statusIndicator
    width: 200
    height: 60
    radius: 8
    
    property string status: "Unknown"
    property string level: "low"
    
    color: {
        if (level === "high") return "#00ff88"
        if (level === "medium") return "#ffaa00"
        return "#ff4444"
    }
    
    Text {
        anchors.centerIn: parent
        text: statusIndicator.status
        color: "#000000"
        font.bold: true
        font.pixelSize: 18
    }
    
    // Xenon effect border
    Rectangle {
        anchors.fill: parent
        radius: parent.radius
        border.color: "#00ff88"
        border.width: 2
        color: "transparent"
        opacity: 0.5
    }
}

