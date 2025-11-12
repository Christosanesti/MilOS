import QtQuick 2.15
import QtQuick.Controls 2.15
import MilosComponents 1.0

Rectangle {
    id: searchBar
    height: 40
    color: "#2a2a2a"
    radius: 5
    
    signal searchTextChanged(string searchText)
    
    property string searchText: ""
    
    RowLayout {
        anchors.fill: parent
        anchors.margins: 5
        
        TextField {
            id: searchField
            Layout.fillWidth: true
            placeholderText: "Search tools..."
            color: "#ffffff"
            background: Rectangle {
                color: "#1a1a1a"
                radius: 3
            }
            
            onTextChanged: {
                searchBar.searchText = text
                searchTextChanged(text)
            }
        }
        
        Button {
            text: "Search"
            onClicked: {
                searchBar.searchText = searchField.text
                searchTextChanged(searchField.text)
            }
        }
    }
}

