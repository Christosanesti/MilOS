import QtQuick 2.15
import QtQuick.Controls 2.15
import MilosComponents 1.0

Rectangle {
    id: segmentList
    color: "#1a1a1a"
    
    signal segmentSelected(string segmentId)
    signal segmentDeleted(string segmentId)
    
    property var segments: []
    
    ScrollView {
        anchors.fill: parent
        
        ListView {
            id: listView
            model: segmentList.segments
            
            delegate: SegmentCard {
                width: listView.width
                segment: modelData
                onSelected: {
                    segmentList.segmentSelected(segmentId)
                }
                onDeleted: {
                    segmentList.segmentDeleted(segmentId)
                }
            }
        }
    }
}

