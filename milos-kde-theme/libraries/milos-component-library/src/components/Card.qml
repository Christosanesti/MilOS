import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: card
    
    // Properties
    property string variant: "dashboard" // status, dashboard, interactive, detail, alert
    property string state: "default" // default, hover, active, expanded, alert
    property string title: ""
    property string subtitle: ""
    property bool expandable: false
    property bool expanded: false
    property color statusColor: "#00cc66" // For status cards
    property color xenonColor: "#00d4ff"
    property color xenonHoverColor: "#00ffff"
    property color xenonAlertColor: "#ffaa00"
    
    // Dimensions
    width: 300
    height: expanded ? expandedHeight : implicitHeight
    implicitHeight: {
        var baseHeight = title.length > 0 ? 60 : 40
        var contentHeight = childrenRect.height
        return baseHeight + contentHeight + 32
    }
    property int expandedHeight: implicitHeight + 200 // Additional space when expanded
    
    // Base styling
    color: "#1a1a1a"
    radius: 12
    border.width: 1
    border.color: getBorderColor()
    
    function getBorderColor() {
        if (state === "alert") return xenonAlertColor
        if (state === "active") return xenonColor
        if (state === "hover") return xenonHoverColor
        return xenonColor
    }
    
    function getBorderOpacity() {
        if (state === "alert") return 0.8
        if (state === "active") return 0.6
        if (state === "hover") return 0.8
        return 0.4
    }
    
    // Xenon border glow
    Rectangle {
        anchors.fill: parent
        radius: parent.radius
        color: "transparent"
        border.color: getBorderColor()
        border.width: {
            if (state === "hover" || state === "active") return 2
            return 1
        }
        opacity: getBorderOpacity()
        
        // Status pulse for status cards (Animation 1: Xenon Glow Pulse)
        SequentialAnimation on opacity {
            id: statusPulse
            running: variant === "status" && state === "active"
            loops: Animation.Infinite
            NumberAnimation {
                from: 0.4
                to: 0.8
                duration: 1000
                easing.type: Easing.InOutSine
            }
            NumberAnimation {
                from: 0.8
                to: 0.4
                duration: 1000
                easing.type: Easing.InOutSine
            }
        }
        
        // Alert pulse (Animation 4: Xenon Alert Pulse)
        SequentialAnimation on opacity {
            id: alertPulse
            running: state === "alert"
            loops: Animation.Infinite
            NumberAnimation {
                from: 0.6
                to: 1.0
                duration: 500
                easing.type: Easing.InOutSine
            }
            NumberAnimation {
                from: 1.0
                to: 0.6
                duration: 500
                easing.type: Easing.InOutSine
            }
        }
        
        Behavior on opacity {
            NumberAnimation { duration: 200; easing.type: Easing.EaseOut }
        }
    }
    
    // Expand transition (Animation 10: Xenon Expansion)
    Behavior on height {
        enabled: expandable
        NumberAnimation {
            duration: 300
            easing.type: Easing.InOutSine
        }
    }
    
    // Content
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 12
        
        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: 12
            
            // Status indicator (for status cards)
            Rectangle {
                width: 12
                height: 12
                radius: 6
                color: statusColor
                visible: variant === "status"
                
                SequentialAnimation on opacity {
                    running: variant === "status" && state === "active"
                    loops: Animation.Infinite
                    NumberAnimation {
                        from: 0.5
                        to: 1.0
                        duration: 1000
                        easing.type: Easing.InOutSine
                    }
                    NumberAnimation {
                        from: 1.0
                        to: 0.5
                        duration: 1000
                        easing.type: Easing.InOutSine
                    }
                }
            }
            
            // Title
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4
                
                Text {
                    Layout.fillWidth: true
                    text: card.title
                    font.pixelSize: 18
                    font.weight: Font.Bold
                    color: "#ffffff"
                    elide: Text.ElideRight
                }
                
                Text {
                    Layout.fillWidth: true
                    text: card.subtitle
                    font.pixelSize: 12
                    color: "#cccccc"
                    visible: subtitle.length > 0
                    elide: Text.ElideRight
                }
            }
            
            // Expand/collapse indicator
            Text {
                text: expanded ? "▼" : "▶"
                font.pixelSize: 12
                color: xenonColor
                visible: expandable
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        
        // Content area
        Item {
            id: contentArea
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: !expandable || expanded
            
            // Default content slot - can be replaced by children
            Text {
                anchors.fill: parent
                text: "Card content"
                font.pixelSize: 14
                color: "#cccccc"
                visible: parent.children.length === 1 // Only show if no custom content
            }
        }
    }
    
    // Mouse area for interactive cards
    MouseArea {
        anchors.fill: parent
        enabled: variant === "interactive" || expandable
        hoverEnabled: true
        cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
        
        onEntered: {
            if (card.state === "default") {
                card.state = "hover"
            }
        }
        onExited: {
            if (card.state === "hover") {
                card.state = "default"
            }
        }
        onClicked: {
            if (expandable) {
                card.expanded = !card.expanded
            }
            if (variant === "interactive") {
                card.state = "active"
                card.clicked()
            }
        }
    }
    
    // Keyboard handling
    Keys.onPressed: {
        if (event.key === Qt.Key_Return || event.key === Qt.Key_Space) {
            if (expandable) {
                expanded = !expanded
            }
            if (variant === "interactive") {
                state = "active"
                clicked()
            }
            event.accepted = true
        }
    }
    
    // Signals
    signal clicked()
    
    // Accessibility
    Accessible.role: variant === "interactive" ? Accessible.Button : Accessible.Pane
    Accessible.name: title || "Card"
    Accessible.description: subtitle
    Accessible.onPressAction: {
        if (expandable) {
            expanded = !expanded
        }
        if (variant === "interactive") {
            clicked()
        }
    }
}

