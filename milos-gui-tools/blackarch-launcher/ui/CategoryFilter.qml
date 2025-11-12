import QtQuick 2.15
import QtQuick.Controls 2.15
import MilosComponents 1.0

Rectangle {
    id: categoryFilter
    color: "#1a1a1a"
    radius: 5
    
    signal categorySelected(string category)
    
    property var categories: []
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 5
        
        Text {
            text: "Categories"
            color: "#ffffff"
            font.bold: true
        }
        
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ListView {
                id: categoryList
                model: ["All"].concat(categoryFilter.categories)
                
                delegate: Button {
                    width: categoryList.width
                    text: modelData
                    onClicked: {
                        categoryFilter.categorySelected(modelData)
                    }
                }
            }
        }
    }
}

