import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import MilosComponents 1.0

Dialog {
    id: segmentEditor
    title: "Network Segment Editor"
    width: 600
    height: 500
    modal: true
    
    signal segmentSaved(string segmentId, string name, string networkAddress, string description)
    
    property string segmentId: ""
    property string segmentName: ""
    property string segmentNetwork: ""
    property string segmentDescription: ""
    
    function loadSegment(segId) {
        var segment = segmentManager.getSegment(segId)
        if (segment) {
            segmentEditor.segmentId = segment.segmentId
            segmentEditor.segmentName = segment.name
            segmentEditor.segmentNetwork = segment.networkAddress
            segmentEditor.segmentDescription = segment.description
        }
    }
    
    function open() {
        if (segmentEditor.segmentId.isEmpty()) {
            // New segment
            segmentEditor.segmentName = ""
            segmentEditor.segmentNetwork = ""
            segmentEditor.segmentDescription = ""
        }
        segmentEditor.visible = true
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15
        
        TextField {
            id: nameField
            Layout.fillWidth: true
            placeholderText: "Segment Name"
            text: segmentEditor.segmentName
            onTextChanged: {
                segmentEditor.segmentName = text
            }
        }
        
        TextField {
            id: networkField
            Layout.fillWidth: true
            placeholderText: "Network Address (CIDR, e.g., 192.168.1.0/24)"
            text: segmentEditor.segmentNetwork
            onTextChanged: {
                segmentEditor.segmentNetwork = text
            }
        }
        
        TextArea {
            id: descriptionField
            Layout.fillWidth: true
            Layout.fillHeight: true
            placeholderText: "Segment Description"
            text: segmentEditor.segmentDescription
            onTextChanged: {
                segmentEditor.segmentDescription = text
            }
        }
        
        RowLayout {
            Layout.fillWidth: true
            
            Item {
                Layout.fillWidth: true
            }
            
            Button {
                text: "Cancel"
                onClicked: {
                    segmentEditor.visible = false
                }
            }
            
            Button {
                text: "Save"
                onClicked: {
                    segmentEditor.segmentSaved(segmentEditor.segmentId,
                                             segmentEditor.segmentName,
                                             segmentEditor.segmentNetwork,
                                             segmentEditor.segmentDescription)
                    segmentEditor.visible = false
                }
            }
        }
    }
}

