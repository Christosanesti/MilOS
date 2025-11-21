import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3

ApplicationWindow {
    id: mainWindow
    width: 1024
    height: 768
    visible: true
    title: "MilOS Document Classification System"
    
    color: "#0a0a0a"
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20
        
        // Classification Level Indicator
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 100
            color: "#1a1a1a"
            border.color: "#444444"
            border.width: 1
            radius: 8
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 20
                
                Text {
                    text: "Current Classification:"
                    font.pixelSize: 16
                    color: "#ffffff"
                }
                
                Text {
                    id: classificationText
                    text: "Unclassified"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffff44"
                }
            }
        }
        
        // File Selection
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            TextField {
                id: filePathField
                Layout.fillWidth: true
                placeholderText: "Enter file path or click Browse..."
                color: "#ffffff"
                background: Rectangle {
                    color: "#2a2a2a"
                    border.color: "#444444"
                    border.width: 1
                    radius: 4
                }
            }
            
            Button {
                text: "Browse"
                onClicked: fileDialog.open()
            }
            
            Button {
                text: "Classify"
                onClicked: {
                    if (filePathField.text) {
                        var level = documentClassifier.classifyDocument(filePathField.text)
                        updateClassificationDisplay(level)
                    }
                }
            }
        }
        
        // Handling Instructions
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#1a1a1a"
            border.color: "#444444"
            border.width: 1
            radius: 8
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 10
                
                Text {
                    text: "Handling Instructions"
                    font.pixelSize: 16
                    font.bold: true
                    color: "#ffffff"
                }
                
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    
                    ListView {
                        id: instructionsList
                        model: []
                        delegate: Text {
                            width: ListView.view.width
                            text: modelData
                            color: "#aaaaaa"
                            font.pixelSize: 12
                            wrapMode: Text.WordWrap
                        }
                    }
                }
            }
        }
    }
    
    FileDialog {
        id: fileDialog
        title: "Select Document to Classify"
        onAccepted: {
            filePathField.text = fileUrl.toString().replace("file://", "")
        }
    }
    
    function updateClassificationDisplay(level) {
        var levelText = ""
        var color = "#ffff44"
        
        switch(level) {
            case 0: levelText = "Unclassified"; color = "#44ff44"; break
            case 1: levelText = "Confidential"; color = "#ffff44"; break
            case 2: levelText = "Secret"; color = "#ff8844"; break
            case 3: levelText = "Top Secret"; color = "#ff4444"; break
        }
        
        classificationText.text = levelText
        classificationText.color = color
        
        // Update handling instructions
        if (filePathField.text) {
            var instructions = classificationHandler.getHandlingInstructions(filePathField.text)
            instructionsList.model = instructions
        }
    }
}

