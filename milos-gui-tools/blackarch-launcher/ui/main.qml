import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

ApplicationWindow {
    id: mainWindow
    width: 1200
    height: 800
    visible: true
    title: "MilOS BlackArch Tool Launcher"
    
    color: "#0a0a0a"
    
    // Main layout
    RowLayout {
        anchors.fill: parent
        spacing: 0
        
        // Sidebar with categories and favorites
        Rectangle {
            Layout.preferredWidth: 250
            Layout.fillHeight: true
            color: "#1a1a1a"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10
                
                // Search bar
                SearchBar {
                    id: searchBar
                    Layout.fillWidth: true
                    onSearchTextChanged: {
                        if (searchText.length > 0) {
                            toolList.tools = toolManager.searchTools(searchText)
                        } else {
                            toolList.tools = toolManager.tools
                        }
                    }
                }
                
                // Category filter
                CategoryFilter {
                    id: categoryFilter
                    Layout.fillWidth: true
                    categories: toolManager.categories
                    onCategorySelected: {
                        if (category === "All") {
                            toolList.tools = toolManager.tools
                        } else {
                            toolList.tools = toolManager.getToolsByCategory(category)
                        }
                    }
                }
                
                // Favorites panel
                FavoritesPanel {
                    id: favoritesPanel
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    favorites: toolManager.favorites
                    onToolSelected: {
                        toolManager.launchTool(toolName)
                    }
                }
            }
        }
        
        // Main content area
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#0a0a0a"
            
            ToolList {
                id: toolList
                anchors.fill: parent
                tools: toolManager.tools
                onToolSelected: {
                    toolManager.launchTool(toolName)
                }
                onToggleFavorite: {
                    if (toolManager.isFavorite(toolName)) {
                        toolManager.removeFromFavorites(toolName)
                    } else {
                        toolManager.addToFavorites(toolName)
                    }
                }
            }
        }
    }
}

