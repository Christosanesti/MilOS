import QtQuick 2.15
import QtQuick.Window 2.15
import QuickActions 1.0

Window {
    id: root
    visible: true
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool
    color: "transparent"
    
    // Make window cover entire screen
    x: 0
    y: 0
    width: Screen.width
    height: Screen.height
    
    // Floating action buttons
    FloatingActionButtons {
        id: floatingButtons
        anchors.fill: parent
        service: quickActionsService
    }
    
    // Make window not capture mouse events (click-through)
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        hoverEnabled: false
    }
}

