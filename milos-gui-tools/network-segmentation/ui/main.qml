import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

ApplicationWindow {
    id: mainWindow
    width: 1400
    height: 900
    visible: true
    title: "MilOS Network Segmentation"
    
    color: "#0a0a0a"
    
    // Main layout
    RowLayout {
        anchors.fill: parent
        spacing: 0
        
        // Sidebar with segment list
        Rectangle {
            Layout.preferredWidth: 350
            Layout.fillHeight: true
            color: "#1a1a1a"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10
                
                Text {
                    text: "Network Segments"
                    color: "#ffffff"
                    font.bold: true
                    font.pixelSize: 18
                }
                
                Button {
                    text: "Create Segment"
                    Layout.fillWidth: true
                    onClicked: {
                        segmentEditor.open()
                    }
                }
                
                SegmentList {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    segments: segmentManager.segments
                    onSegmentSelected: {
                        segmentEditor.loadSegment(segmentId)
                        segmentEditor.open()
                    }
                    onSegmentDeleted: {
                        segmentManager.deleteSegment(segmentId)
                    }
                }
            }
        }
        
        // Main content area with topology
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#0a0a0a"
            
            TopologyView {
                id: topologyView
                anchors.fill: parent
                nodes: topologyDisplay.nodes
                edges: topologyDisplay.edges
            }
        }
    }
    
    // Segment editor dialog
    SegmentEditor {
        id: segmentEditor
        onSegmentSaved: {
            if (segmentId.isEmpty()) {
                segmentManager.createSegment(name, networkAddress, description)
            } else {
                segmentManager.updateSegment(segmentId, name, networkAddress, description)
            }
        }
    }
}

