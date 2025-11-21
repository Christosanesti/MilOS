import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: mainWindow
    width: 1024
    height: 768
    visible: true
    title: "MilOS Compliance Checklist Manager"
    
    color: "#0a0a0a"
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20
        
        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: 20
            
            ComboBox {
                id: standardCombo
                Layout.preferredWidth: 200
                model: checklistManager.availableStandards
                currentIndex: 0
            }
            
            TextField {
                id: checklistNameField
                Layout.fillWidth: true
                placeholderText: "Enter checklist name..."
                color: "#ffffff"
                background: Rectangle {
                    color: "#2a2a2a"
                    border.color: "#444444"
                    border.width: 1
                    radius: 4
                }
            }
            
            Button {
                text: "Create Checklist"
                onClicked: {
                    var checklistId = checklistManager.createChecklist(standardCombo.currentText, checklistNameField.text)
                    if (checklistId) {
                        checklistNameField.text = ""
                        loadChecklists()
                    }
                }
            }
        }
        
        // Checklists List
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
                    text: "Compliance Checklists"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#ffffff"
                }
                
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    
                    ListView {
                        id: checklistsList
                        model: []
                        delegate: Rectangle {
                            width: ListView.view.width
                            height: 80
                            color: "#2a2a2a"
                            radius: 4
                            
                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 10
                                spacing: 15
                                
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 5
                                    
                                    Text {
                                        text: modelData.name || "Unnamed Checklist"
                                        font.pixelSize: 14
                                        font.bold: true
                                        color: "#ffffff"
                                    }
                                    
                                    Text {
                                        text: "Progress: " + (modelData.completion_percentage || 0) + "%"
                                        font.pixelSize: 12
                                        color: "#aaaaaa"
                                    }
                                }
                                
                                Button {
                                    text: "View"
                                    onClicked: {
                                        // Load checklist details
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    Component.onCompleted: {
        loadChecklists()
    }
    
    function loadChecklists() {
        var checklists = checklistManager.getAllChecklists()
        var checklistData = []
        for (var i = 0; i < checklists.length; i++) {
            var checklist = checklists[i]
            checklistData.push({
                "id": checklist.id,
                "name": checklist.name,
                "completion_percentage": checklist.completionPercentage
            })
        }
        checklistsList.model = checklistData
    }
}

