import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: toolList
    color: "#0a0a0a"
    
    signal toolSelected(string toolName)
    signal toggleFavorite(string toolName)
    
    property var tools: []
    
    ScrollView {
        anchors.fill: parent
        anchors.margins: 20
        
        GridLayout {
            width: toolList.width - 40
            columns: 3
            rowSpacing: 15
            columnSpacing: 15
            
            Repeater {
                model: toolList.tools
                
                ToolCard {
                    tool: modelData
                    onToolSelected: {
                        toolList.toolSelected(toolName)
                    }
                    onToggleFavorite: {
                        toolList.toggleFavorite(toolName)
                    }
                }
            }
        }
    }
}

