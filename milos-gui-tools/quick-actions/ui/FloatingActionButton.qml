import QtQuick 2.15
import QtQuick.Controls 2.15

/**
 * Individual Floating Action Button
 * 
 * Single floating action button with xenon visual feedback.
 */
Rectangle {
    id: button
    
    property var action: null
    property string icon: action ? action.icon : ""
    property string label: action ? action.label : ""
    property bool isPrimary: false
    
    signal clicked()
    
    radius: width / 2
    color: buttonMouseArea.containsMouse ? xenonColor : "#2a2a2a"
    border.color: xenonColor
    border.width: 2
    
    // Xenon color based on state
    property color xenonColor: isPrimary ? "#00d4ff" : "#ffaa00"
    
    // Xenon glow effect
    Rectangle {
        anchors.fill: parent
        anchors.margins: -4
        radius: parent.radius + 4
        color: "transparent"
        border.color: xenonColor
        border.width: 2
        opacity: buttonMouseArea.containsMouse ? 0.8 : 0.3
        
        SequentialAnimation on opacity {
            running: buttonMouseArea.containsMouse
            loops: Animation.Infinite
            NumberAnimation {
                from: 0.3
                to: 0.8
                duration: 1000
                easing.type: Easing.InOutQuad
            }
            NumberAnimation {
                from: 0.8
                to: 0.3
                duration: 1000
                easing.type: Easing.InOutQuad
            }
        }
    }
    
    // Icon
    Text {
        anchors.centerIn: parent
        text: icon !== "" ? "" : (label.length > 0 ? label.charAt(0).toUpperCase() : "?")
        font.pixelSize: parent.width * 0.4
        color: "#ffffff"
        visible: icon === ""
    }
    
    // Label tooltip (shown on hover)
    Rectangle {
        id: tooltip
        visible: buttonMouseArea.containsMouse && label !== ""
        anchors.right: parent.left
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        width: tooltipText.width + 16
        height: tooltipText.height + 8
        radius: 4
        color: "#1a1a1a"
        border.color: xenonColor
        border.width: 1
        
        Text {
            id: tooltipText
            anchors.centerIn: parent
            text: label
            font.pixelSize: 12
            color: "#ffffff"
        }
        
        Behavior on opacity {
            NumberAnimation { duration: 150 }
        }
    }
    
    MouseArea {
        id: buttonMouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            button.clicked()
            if (action) {
                // Execute action via service
                var service = button.parent.parent.service
                if (service) {
                    service.executeAction(action.id)
                }
            }
        }
    }
    
    // Processing state indicator
    Rectangle {
        anchors.fill: parent
        radius: parent.radius
        color: "#1a1a1a"
        opacity: processing ? 0.7 : 0
        visible: processing
        
        Behavior on opacity {
            NumberAnimation { duration: 200 }
        }
    }
    
    property bool processing: false
    
    Connections {
        target: button.parent.parent.service
        function onActionExecuted(actionId, success) {
            if (action && action.id === actionId) {
                processing = false
            }
        }
    }
    
    onActionChanged: {
        if (action) {
            processing = true
        }
    }
}

