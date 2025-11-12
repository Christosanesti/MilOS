import QtQuick 2.15
import QtQuick.Controls 2.15
import MilosComponents 1.0

Rectangle {
    id: toolCard
    width: 300
    height: 150
    color: "#1a1a1a"
    radius: 8
    border.color: tool.isInstalled ? "#00ff88" : "#666666"
    border.width: 2
    
    signal toolSelected(string toolName)
    signal toggleFavorite(string toolName)
    
    property var tool
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 5
        
        RowLayout {
            Layout.fillWidth: true
            
            Text {
                Layout.fillWidth: true
                text: tool.displayName || tool.name
                color: "#ffffff"
                font.bold: true
                font.pixelSize: 16
            }
            
            Button {
                text: tool.isFavorite ? "★" : "☆"
                onClicked: {
                    toolCard.toggleFavorite(tool.name)
                }
            }
        }
        
        Text {
            Layout.fillWidth: true
            text: tool.description || "No description"
            color: "#aaaaaa"
            font.pixelSize: 12
            wrapMode: Text.WordWrap
        }
        
        RowLayout {
            Layout.fillWidth: true
            
            Text {
                text: tool.category || "Other"
                color: "#00ff88"
                font.pixelSize: 10
            }
            
            Item {
                Layout.fillWidth: true
            }
            
            Button {
                text: tool.isInstalled ? "Launch" : "Not Installed"
                enabled: tool.isInstalled
                onClicked: {
                    toolCard.toolSelected(tool.name)
                }
            }
        }
    }
}

