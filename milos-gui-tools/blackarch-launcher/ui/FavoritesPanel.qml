import QtQuick 2.15
import QtQuick.Controls 2.15
import MilosComponents 1.0

Rectangle {
    id: favoritesPanel
    color: "#1a1a1a"
    radius: 5
    
    signal toolSelected(string toolName)
    
    property var favorites: []
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 5
        
        Text {
            text: "Favorites"
            color: "#ffffff"
            font.bold: true
        }
        
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ListView {
                id: favoritesList
                model: favoritesPanel.favorites
                
                delegate: Button {
                    width: favoritesList.width
                    text: modelData.displayName || modelData.name
                    onClicked: {
                        favoritesPanel.toolSelected(modelData.name)
                    }
                }
            }
        }
    }
}

