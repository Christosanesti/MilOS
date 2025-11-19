import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Dialog {
    id: ruleBuilder
    title: "Firewall Rule Builder"
    width: 700
    height: 600
    modal: true
    
    signal ruleCreated(string name, string source, string destination, string action, string protocol)
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15
        
        TextField {
            id: nameField
            Layout.fillWidth: true
            placeholderText: "Rule Name"
        }
        
        TextField {
            id: sourceField
            Layout.fillWidth: true
            placeholderText: "Source (IP/Network, e.g., 192.168.1.0/24)"
        }
        
        TextField {
            id: destinationField
            Layout.fillWidth: true
            placeholderText: "Destination (IP/Network)"
        }
        
        ComboBox {
            id: actionCombo
            Layout.fillWidth: true
            model: ["ACCEPT", "DROP", "REJECT"]
            currentIndex: 0
        }
        
        ComboBox {
            id: protocolCombo
            Layout.fillWidth: true
            model: ["TCP", "UDP", "ICMP", "ALL"]
            currentIndex: 0
        }
        
        TextArea {
            id: descriptionField
            Layout.fillWidth: true
            Layout.fillHeight: true
            placeholderText: "Rule Description"
        }
        
        RowLayout {
            Layout.fillWidth: true
            
            Item {
                Layout.fillWidth: true
            }
            
            Button {
                text: "Cancel"
                onClicked: {
                    ruleBuilder.visible = false
                }
            }
            
            Button {
                text: "Create Rule"
                onClicked: {
                    ruleBuilder.ruleCreated(
                        nameField.text,
                        sourceField.text,
                        destinationField.text,
                        actionCombo.currentText,
                        protocolCombo.currentText
                    )
                    ruleBuilder.visible = false
                }
            }
        }
    }
}

