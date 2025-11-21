import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: mainWindow
    width: 1400
    height: 900
    visible: true
    title: "MilOS Advanced Network Defense"
    
    color: "#0a0a0a"
    
    TabView {
        anchors.fill: parent
        anchors.margins: 20
        
        Tab {
            title: "ML Threat Detection"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                Text {
                    text: "Machine Learning Threat Detection"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffffff"
                }
                
                Text {
                    text: "Model Accuracy: " + (mlDetector ? mlDetector.modelAccuracy.toFixed(2) : "0.00")
                    font.pixelSize: 14
                    color: "#44ff44"
                }
                
                Button {
                    text: mlDetector && mlDetector.isTraining ? "Training..." : "Train Model"
                    enabled: mlDetector && !mlDetector.isTraining
                    onClicked: {
                        // Train model
                    }
                }
            }
        }
        
        Tab {
            title: "Network Forensics"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                Text {
                    text: "Network Forensics & Investigation"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffffff"
                }
                
                Button {
                    text: "Generate Report"
                    onClicked: {
                        // Generate forensic report
                    }
                }
            }
        }
        
        Tab {
            title: "Packet Analysis"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                Text {
                    text: "Advanced Packet Analysis"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffffff"
                }
            }
        }
        
        Tab {
            title: "IDS Rules"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                Text {
                    text: "Custom IDS Rules Engine"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffffff"
                }
                
                Button {
                    text: "Add Rule"
                    onClicked: {
                        // Add IDS rule
                    }
                }
            }
        }
        
        Tab {
            title: "Incident Response"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                Text {
                    text: "Automated Incident Response Workflows"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffffff"
                }
                
                Button {
                    text: "Create Workflow"
                    onClicked: {
                        // Create workflow
                    }
                }
            }
        }
    }
}

