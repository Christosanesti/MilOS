import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.core 2.1 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents

Rectangle {
    id: confirmationDialog
    
    property string actionType: ""
    property string message: "Are you sure?"
    
    signal confirmed()
    signal cancelled()
    
    anchors.fill: parent
    color: "#1a1a1a"
    radius: 8
    border.width: 2
    border.color: "#ffaa00" // Xenon warning color
    
    visible: false
    
    // Xenon warning border glow
    Rectangle {
        anchors.fill: parent
        anchors.margins: -2
        radius: parent.radius + 2
        color: "transparent"
        border.color: "#ffaa00"
        border.width: 2
        opacity: 0.8
        
        SequentialAnimation on opacity {
            running: confirmationDialog.visible
            loops: Animation.Infinite
            NumberAnimation {
                from: 0.4
                to: 1.0
                duration: 1000
                easing.type: Easing.InOutQuad
            }
            NumberAnimation {
                from: 1.0
                to: 0.4
                duration: 1000
                easing.type: Easing.InOutQuad
            }
        }
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 16
        
        Text {
            Layout.fillWidth: true
            text: "Confirm Action"
            font.pixelSize: 18
            font.bold: true
            color: "#ffffff"
        }
        
        Text {
            Layout.fillWidth: true
            text: confirmationDialog.message
            font.pixelSize: 14
            color: "#ffffff"
            wrapMode: Text.WordWrap
        }
        
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            PlasmaComponents.Button {
                Layout.fillWidth: true
                text: "Cancel"
                onClicked: {
                    confirmationDialog.cancelled()
                }
            }
            
            PlasmaComponents.Button {
                Layout.fillWidth: true
                text: "Confirm"
                onClicked: {
                    confirmationDialog.confirmed()
                }
            }
        }
    }
    
    function show() {
        visible = true
    }
}

