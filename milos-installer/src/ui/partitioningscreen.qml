import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: partitioningScreen
    
    color: "#0a0a0a"
    
    property bool autoPartition: true
    property var disks: [
        {name: "sda", size: "500 GB", model: "Samsung SSD 980", partitions: []},
        {name: "sdb", size: "1 TB", model: "Western Digital HDD", partitions: []}
    ]
    
    signal backClicked()
    signal nextClicked()
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 32
        spacing: 24
        
        // Title
        Text {
            Layout.fillWidth: true
            text: "Disk Partitioning"
            font.pixelSize: 24
            font.weight: Font.Bold
            color: "#ffffff"
        }
        
        // Auto-partition option (Recommended)
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 120
            variant: "dashboard"
            title: "Automatic Partitioning"
            subtitle: "Recommended for most users"
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 16
                
                // Recommended badge
                StatusIndicator {
                    variant: "badge"
                    status: "success"
                    text: "Recommended"
                }
                
                Text {
                    Layout.fillWidth: true
                    text: "Automatically create partitions with optimal settings for MilOS"
                    font.pixelSize: 14
                    color: "#cccccc"
                    wrapMode: Text.WordWrap
                }
                
                // Radio button (simplified)
                Rectangle {
                    width: 24
                    height: 24
                    radius: 12
                    border.color: "#00d4ff"
                    border.width: 2
                    color: partitioningScreen.autoPartition ? "#00d4ff" : "transparent"
                    
                    Rectangle {
                        anchors.centerIn: parent
                        width: 12
                        height: 12
                        radius: 6
                        color: "#0a0a0a"
                        visible: partitioningScreen.autoPartition
                    }
                    
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            partitioningScreen.autoPartition = true
                        }
                    }
                }
            }
        }
        
        // Manual partitioning option
        Card {
            Layout.fillWidth: true
            Layout.preferredHeight: 120
            variant: "dashboard"
            title: "Manual Partitioning"
            subtitle: "Advanced users only"
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 16
                
                Text {
                    Layout.fillWidth: true
                    text: "Manually configure partitions, filesystems, and mount points"
                    font.pixelSize: 14
                    color: "#cccccc"
                    wrapMode: Text.WordWrap
                }
                
                // Radio button
                Rectangle {
                    width: 24
                    height: 24
                    radius: 12
                    border.color: "#00d4ff"
                    border.width: 2
                    color: !partitioningScreen.autoPartition ? "#00d4ff" : "transparent"
                    
                    Rectangle {
                        anchors.centerIn: parent
                        width: 12
                        height: 12
                        radius: 6
                        color: "#0a0a0a"
                        visible: !partitioningScreen.autoPartition
                    }
                    
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            partitioningScreen.autoPartition = false
                        }
                    }
                }
            }
        }
        
        // Disk visualization (if manual)
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 16
            visible: !partitioningScreen.autoPartition
            
            Text {
                Layout.fillWidth: true
                text: "Available Disks"
                font.pixelSize: 18
                font.weight: Font.Bold
                color: "#ffffff"
            }
            
            // Disk list
            Repeater {
                model: partitioningScreen.disks
                
                Card {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 100
                    variant: "dashboard"
                    title: modelData.name + " - " + modelData.model
                    subtitle: modelData.size
                    
                    Rectangle {
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        width: 4
                        color: "#00d4ff"
                    }
                }
            }
        }
        
        // Navigation buttons
        RowLayout {
            Layout.fillWidth: true
            spacing: 16
            
            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 48
                variant: "secondary"
                text: "Back"
                onClicked: {
                    partitioningScreen.backClicked()
                }
            }
            
            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 48
                variant: "primary"
                text: "Next"
                onClicked: {
                    // Show warning if manual partitioning
                    if (!partitioningScreen.autoPartition) {
                        // TODO: Show warning dialog
                    }
                    partitioningScreen.nextClicked()
                }
            }
        }
    }
    
    // Accessibility
    Accessible.role: Accessible.Pane
    Accessible.name: "Partitioning Screen"
    Accessible.description: "Configure disk partitioning for MilOS installation"
}

