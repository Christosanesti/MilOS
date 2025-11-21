import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: mainWindow
    width: 1400
    height: 900
    visible: true
    title: "MilOS Enterprise Management Console"
    
    color: "#0a0a0a"
    
    TabView {
        anchors.fill: parent
        anchors.margins: 20
        
        Tab {
            title: "Configuration"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                Text {
                    text: "Centralized Configuration Management"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffffff"
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10
                    
                    TextField {
                        id: configKeyField
                        Layout.fillWidth: true
                        placeholderText: "Configuration Key..."
                        color: "#ffffff"
                        background: Rectangle {
                            color: "#2a2a2a"
                            border.color: "#444444"
                            border.width: 1
                            radius: 4
                        }
                    }
                    
                    TextField {
                        id: configValueField
                        Layout.fillWidth: true
                        placeholderText: "Configuration Value..."
                        color: "#ffffff"
                        background: Rectangle {
                            color: "#2a2a2a"
                            border.color: "#444444"
                            border.width: 1
                            radius: 4
                        }
                    }
                    
                    Button {
                        text: "Set Config"
                        onClicked: {
                            configManager.setConfig(configKeyField.text, configValueField.text)
                        }
                    }
                }
                
                Button {
                    text: "Export Configuration"
                    onClicked: {
                        // Export config
                    }
                }
            }
        }
        
        Tab {
            title: "Deployments"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                Text {
                    text: "Deployment Orchestration"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffffff"
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10
                    
                    TextField {
                        id: deploymentNameField
                        Layout.fillWidth: true
                        placeholderText: "Deployment Name..."
                        color: "#ffffff"
                        background: Rectangle {
                            color: "#2a2a2a"
                            border.color: "#444444"
                            border.width: 1
                            radius: 4
                        }
                    }
                    
                    Button {
                        text: "Create Deployment"
                        onClicked: {
                            // Create deployment
                        }
                    }
                }
            }
        }
        
        Tab {
            title: "Multi-Site"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                Text {
                    text: "Multi-Site Management"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffffff"
                }
                
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    
                    ListView {
                        id: sitesList
                        model: multiSiteManager.getAllSites()
                        delegate: Rectangle {
                            width: ListView.view.width
                            height: 60
                            color: "#2a2a2a"
                            radius: 4
                            
                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.leftMargin: 10
                                text: modelData || "Unknown Site"
                                color: "#ffffff"
                            }
                        }
                    }
                }
            }
        }
    }
}

