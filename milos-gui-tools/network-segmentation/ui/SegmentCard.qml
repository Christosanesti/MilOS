import QtQuick 2.15
import QtQuick.Controls 2.15
import MilosComponents 1.0

Rectangle {
    id: segmentCard
    height: 120
    color: "#2a2a2a"
    radius: 8
    border.color: "#00ff88"
    border.width: 2
    
    signal selected(string segmentId)
    signal deleted(string segmentId)
    
    property var segment
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 5
        
        RowLayout {
            Layout.fillWidth: true
            
            Text {
                Layout.fillWidth: true
                text: segment.name || "Unnamed Segment"
                color: "#ffffff"
                font.bold: true
                font.pixelSize: 16
            }
            
            Button {
                text: "Delete"
                onClicked: {
                    segmentCard.deleted(segment.segmentId)
                }
            }
        }
        
        Text {
            Layout.fillWidth: true
            text: segment.description || "No description"
            color: "#aaaaaa"
            font.pixelSize: 12
            wrapMode: Text.WordWrap
        }
        
        RowLayout {
            Layout.fillWidth: true
            
            Text {
                text: segment.networkAddress || "No network"
                color: "#00ff88"
                font.pixelSize: 12
            }
            
            Item {
                Layout.fillWidth: true
            }
            
            Text {
                text: segment.isIsolated ? "Isolated" : "Connected"
                color: segment.isIsolated ? "#ff8800" : "#00ff88"
                font.pixelSize: 10
            }
            
            Button {
                text: "Edit"
                onClicked: {
                    segmentCard.selected(segment.segmentId)
                }
            }
        }
    }
}

