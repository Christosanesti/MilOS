import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

ModalDialog {
    id: confirmationDialog
    
    property string title: "Confirm Action"
    property string message: "Are you sure you want to proceed?"
    property bool requiresDoubleConfirmation: false
    property bool isSecondConfirmation: false
    
    signal confirmed()
    signal cancelled()
    
    title: confirmationDialog.title
    variant: "warning"
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16
        
        Text {
            Layout.fillWidth: true
            text: confirmationDialog.message
            font.pixelSize: 16
            color: "#ffffff"
            wrapMode: Text.WordWrap
        }
        
        // Warning indicator for destructive actions
        NotificationAlert {
            Layout.fillWidth: true
            variant: "warning"
            title: "Warning"
            message: "This action cannot be easily undone."
            visible: confirmationDialog.requiresDoubleConfirmation && !confirmationDialog.isSecondConfirmation
        }
        
        // Second confirmation message
        Text {
            Layout.fillWidth: true
            text: "Please confirm again to proceed."
            font.pixelSize: 14
            color: "#ffaa00"
            wrapMode: Text.WordWrap
            visible: confirmationDialog.requiresDoubleConfirmation && confirmationDialog.isSecondConfirmation
        }
        
        // Buttons
        RowLayout {
            Layout.fillWidth: true
            spacing: 16
            
            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 48
                variant: "secondary"
                text: "Cancel"
                onClicked: {
                    confirmationDialog.cancelled()
                    confirmationDialog.close()
                }
            }
            
            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 48
                variant: confirmationDialog.requiresDoubleConfirmation ? "secondary" : "primary"
                text: confirmationDialog.requiresDoubleConfirmation && !confirmationDialog.isSecondConfirmation ? "Continue" : "Confirm"
                onClicked: {
                    if (confirmationDialog.requiresDoubleConfirmation && !confirmationDialog.isSecondConfirmation) {
                        confirmationDialog.isSecondConfirmation = true
                    } else {
                        confirmationDialog.confirmed()
                        confirmationDialog.close()
                    }
                }
            }
        }
    }
    
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
            running: true
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
}

