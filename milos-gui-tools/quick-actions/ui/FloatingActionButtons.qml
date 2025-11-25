import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.core 2.1 as PlasmaCore

/**
 * Floating Action Buttons Component
 * 
 * Displays context-aware floating action buttons on the desktop.
 * Shows primary and secondary actions based on current application context.
 */
Item {
    id: root
    
    property alias service: quickActionsService
    property string position: "bottom-right" // "bottom-right", "bottom-left", "top-right", "top-left"
    property bool autoHide: false
    property int buttonSize: 56
    property real spacing: 12
    
    // Xenon colors
    readonly property color xenonCyan: "#00d4ff"
    readonly property color xenonAmber: "#ffaa00"
    readonly property color xenonRed: "#ff4444"
    
    QuickActionsService {
        id: quickActionsService
        
        onCurrentContextChanged: {
            updateButtons()
        }
        
        onAvailableActionsChanged: {
            updateButtons()
        }
    }
    
    // Primary action button (always visible when context is active)
    FloatingActionButton {
        id: primaryButton
        visible: primaryAction !== null && (quickActionsService.currentContext !== "" || !root.autoHide)
        x: getButtonX(0)
        y: getButtonY(0)
        width: root.buttonSize
        height: root.buttonSize
        action: primaryAction
        isPrimary: true
        
        Behavior on opacity {
            NumberAnimation { duration: 200 }
        }
    }
    
    // Secondary action buttons (expandable menu)
    Repeater {
        id: secondaryButtonsRepeater
        model: secondaryActions
        
        FloatingActionButton {
            visible: root.expanded && modelData !== null
            x: getButtonX(index + 1)
            y: getButtonY(index + 1)
            width: root.buttonSize
            height: root.buttonSize
            action: modelData
            isPrimary: false
            
            Behavior on opacity {
                NumberAnimation { duration: 200 }
            }
            
            Behavior on x {
                NumberAnimation { duration: 300; easing.type: Easing.OutCubic }
            }
            
            Behavior on y {
                NumberAnimation { duration: 300; easing.type: Easing.OutCubic }
            }
        }
    }
    
    // Expand/collapse button (only shown when secondary actions exist)
    FloatingActionButton {
        id: expandButton
        visible: secondaryActions.length > 0 && quickActionsService.currentContext !== ""
        x: getButtonX(0)
        y: getButtonY(0)
        width: root.buttonSize
        height: root.buttonSize
        icon: root.expanded ? "go-up" : "go-down"
        label: ""
        isPrimary: false
        
        onClicked: {
            root.expanded = !root.expanded
        }
        
        Behavior on rotation {
            NumberAnimation { duration: 200 }
        }
        
        rotation: root.expanded ? 180 : 0
    }
    
    property bool expanded: false
    property var primaryAction: null
    property var secondaryActions: []
    
    function updateButtons() {
        var actions = quickActionsService.availableActions
        primaryAction = null
        secondaryActions = []
        
        // Separate primary and secondary actions
        for (var i = 0; i < actions.length; i++) {
            var action = actions[i]
            if (action.priority === "primary") {
                primaryAction = action
            } else {
                secondaryActions.push(action)
            }
        }
        
        // If no primary action, use first action
        if (primaryAction === null && actions.length > 0) {
            primaryAction = actions[0]
        }
    }
    
    function getButtonX(index) {
        var baseX = 0
        if (position === "bottom-right" || position === "top-right") {
            baseX = parent.width - root.buttonSize - 20
        } else {
            baseX = 20
        }
        
        if (index === 0) {
            return baseX
        }
        
        // Secondary buttons stack vertically
        return baseX
    }
    
    function getButtonY(index) {
        var baseY = 0
        if (position === "bottom-right" || position === "bottom-left") {
            baseY = parent.height - root.buttonSize - 20
        } else {
            baseY = 20
        }
        
        if (index === 0) {
            return baseY
        }
        
        // Secondary buttons stack upward from primary
        var offset = (index) * (root.buttonSize + root.spacing)
        if (position === "bottom-right" || position === "bottom-left") {
            return baseY - offset
        } else {
            return baseY + offset
        }
    }
    
    // Auto-hide timer
    Timer {
        id: autoHideTimer
        interval: 3000
        running: root.autoHide && quickActionsService.currentContext === ""
        onTriggered: {
            primaryButton.opacity = 0
            expandButton.opacity = 0
        }
    }
    
    Component.onCompleted: {
        updateButtons()
    }
}

