import QtQuick 2.15
import QtQuick.Controls 2.15
import MilosComponents 1.0

Dialog {
    id: rulePreview
    title: "Firewall Rules Preview"
    width: 800
    height: 600
    modal: true
    
    property string previewText: ""
    
    ScrollView {
        anchors.fill: parent
        anchors.margins: 20
        
        TextArea {
            id: previewArea
            readOnly: true
            text: rulePreview.previewText
            font.family: "monospace"
            font.pixelSize: 12
        }
    }
    
    RowLayout {
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 20
        spacing: 10
        
        Button {
            text: "Close"
            onClicked: {
                rulePreview.visible = false
            }
        }
        
        Button {
            text: "Apply Rules"
            onClicked: {
                // Apply rules via firewall manager
                firewallManager.applyRules()
                rulePreview.visible = false
            }
        }
    }
}

