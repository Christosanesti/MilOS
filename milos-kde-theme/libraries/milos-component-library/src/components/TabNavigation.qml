import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: tabNavigation
    
    // Properties
    property string variant: "standard" // standard, icon, pill, underline, vertical
    property int currentIndex: 0
    property var tabs: [] // Array of {text: "", icon: "", badge: 0, enabled: true}
    property int maxTabs: 10 // Maximum tabs before overflow
    property color xenonColor: "#00d4ff"
    property color xenonHoverColor: "#00ffff"
    
    // Dimensions
    width: variant === "vertical" ? 200 : 600
    height: variant === "vertical" ? 400 : 48
    implicitWidth: width
    implicitHeight: height
    
    // Base styling
    color: "transparent"
    
    // Tab container
    Row {
        id: tabRow
        anchors.fill: parent
        spacing: 0
        visible: variant !== "vertical"
        
        Repeater {
            id: tabRepeater
            model: Math.min(tabs.length, maxTabs)
            
            Rectangle {
                id: tabItem
                property bool isActive: index === tabNavigation.currentIndex
                property bool hasBadge: tabs[index] && tabs[index].badge > 0
                property bool isEnabled: tabs[index] && tabs[index].enabled !== false
                
                width: {
                    if (variant === "pill") return 100
                    if (variant === "underline") return implicitWidth
                    return (tabRow.width - (tabRepeater.count - 1) * 0) / tabRepeater.count
                }
                height: parent.height
                implicitWidth: {
                    if (variant === "pill") return 100
                    var textWidth = tabText.contentWidth
                    var iconWidth = tabIcon.visible ? 24 : 0
                    var badgeWidth = badgeIndicator.visible ? 20 : 0
                    return textWidth + iconWidth + badgeWidth + 24
                }
                
                color: {
                    if (variant === "pill" && isActive) return xenonColor
                    if (variant === "pill") return "#1a1a1a"
                    return "transparent"
                }
                radius: variant === "pill" ? height / 2 : 0
                
                // Xenon border/underline for active tab
                Rectangle {
                    id: activeIndicator
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: variant === "underline" ? 2 : (variant === "vertical" ? 0 : 2)
                    color: xenonColor
                    visible: isActive && variant !== "pill"
                    opacity: isActive ? 1.0 : 0.0
                    
                    // Pulsing indicator dot
                    Rectangle {
                        anchors.centerIn: parent
                        width: 8
                        height: 8
                        radius: 4
                        color: xenonColor
                        visible: isActive && variant !== "underline"
                        
                        SequentialAnimation on opacity {
                            running: isActive && variant !== "underline"
                            loops: Animation.Infinite
                            NumberAnimation {
                                from: 0.6
                                to: 1.0
                                duration: 1000
                                easing.type: Easing.InOutSine
                            }
                            NumberAnimation {
                                from: 1.0
                                to: 0.6
                                duration: 1000
                                easing.type: Easing.InOutSine
                            }
                        }
                    }
                    
                    Behavior on opacity {
                        NumberAnimation { duration: 200; easing.type: Easing.EaseInOut }
                    }
                }
                
                // Xenon glow for hover
                Rectangle {
                    anchors.fill: parent
                    radius: parent.radius
                    color: xenonHoverColor
                    opacity: mouseArea.containsMouse && !isActive ? 0.2 : 0.0
                    
                    Behavior on opacity {
                        NumberAnimation { duration: 200; easing.type: Easing.EaseOut }
                    }
                }
                
                // Tab content
                Row {
                    anchors.centerIn: parent
                    spacing: 8
                    
                    // Icon
                    Text {
                        id: tabIcon
                        text: tabs[index] && tabs[index].icon ? tabs[index].icon : ""
                        font.pixelSize: 16
                        color: isActive ? xenonColor : (isEnabled ? "#ffffff" : "#666666")
                        visible: (variant === "icon" || variant === "pill") && text.length > 0
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    
                    // Text
                    Text {
                        id: tabText
                        text: tabs[index] ? tabs[index].text : ""
                        font.pixelSize: 14
                        font.weight: isActive ? Font.Bold : Font.Normal
                        color: isActive ? xenonColor : (isEnabled ? "#ffffff" : "#666666")
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    
                    // Badge indicator
                    Rectangle {
                        id: badgeIndicator
                        width: badgeText.contentWidth + 8
                        height: 18
                        radius: 9
                        color: hasBadge ? (tabs[index].badgeType === "alert" ? "#ff4444" : "#ffaa00") : "transparent"
                        visible: hasBadge
                        anchors.verticalCenter: parent.verticalCenter
                        
                        Text {
                            id: badgeText
                            anchors.centerIn: parent
                            text: tabs[index] ? tabs[index].badge : ""
                            font.pixelSize: 10
                            font.weight: Font.Bold
                            color: "#ffffff"
                        }
                        
                        // Badge pulse animation
                        SequentialAnimation on opacity {
                            running: hasBadge
                            loops: Animation.Infinite
                            NumberAnimation {
                                from: 0.7
                                to: 1.0
                                duration: 1000
                                easing.type: Easing.InOutSine
                            }
                            NumberAnimation {
                                from: 1.0
                                to: 0.7
                                duration: 1000
                                easing.type: Easing.InOutSine
                            }
                        }
                    }
                }
                
                // Mouse area
                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    enabled: isEnabled
                    hoverEnabled: true
                    cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                    
                    onClicked: {
                        if (isEnabled) {
                            tabNavigation.currentIndex = index
                        }
                    }
                }
                
                // Keyboard focus indicator
                Rectangle {
                    anchors.fill: parent
                    radius: parent.radius
                    color: "transparent"
                    border.color: xenonColor
                    border.width: 2
                    opacity: activeFocus ? 1.0 : 0.0
                    visible: activeFocus
                    
                    // Focus flash animation (Animation 9: Xenon Focus Flash)
                    SequentialAnimation on opacity {
                        running: activeFocus
                        NumberAnimation {
                            from: 0.0
                            to: 1.0
                            duration: 75
                            easing.type: Easing.EaseOut
                        }
                        NumberAnimation {
                            from: 1.0
                            to: 0.6
                            duration: 75
                            easing.type: Easing.EaseIn
                        }
                    }
                }
            }
        }
        
        // Overflow menu button (if more than maxTabs)
        Rectangle {
            id: overflowButton
            width: 40
            height: parent.height
            visible: tabs.length > maxTabs
            color: mouseAreaOverflow.containsMouse ? "#1a1a1a" : "transparent"
            
            Text {
                anchors.centerIn: parent
                text: "⋯"
                font.pixelSize: 20
                color: "#ffffff"
            }
            
            MouseArea {
                id: mouseAreaOverflow
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    // TODO: Show dropdown menu with overflow tabs
                }
            }
        }
    }
    
    // Vertical tabs
    Column {
        id: verticalColumn
        anchors.fill: parent
        spacing: 0
        visible: variant === "vertical"
        
        Repeater {
            model: Math.min(tabs.length, maxTabs)
            
            Rectangle {
                width: parent.width
                height: 48
                property bool isActive: index === tabNavigation.currentIndex
                property bool isEnabled: tabs[index] && tabs[index].enabled !== false
                
                color: isActive ? "#1a1a1a" : "transparent"
                
                // Active indicator (left border)
                Rectangle {
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: 3
                    color: xenonColor
                    visible: isActive
                }
                
                // Content
                Row {
                    anchors.left: parent.left
                    anchors.leftMargin: 12
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 8
                    
                    Text {
                        text: tabs[index] ? tabs[index].text : ""
                        font.pixelSize: 14
                        font.weight: isActive ? Font.Bold : Font.Normal
                        color: isActive ? xenonColor : (isEnabled ? "#ffffff" : "#666666")
                    }
                }
                
                MouseArea {
                    anchors.fill: parent
                    enabled: isEnabled
                    onClicked: {
                        if (isEnabled) {
                            tabNavigation.currentIndex = index
                        }
                    }
                }
            }
        }
    }
    
    // Keyboard handling
    Keys.onPressed: {
        // Ctrl+1-9 for tab switching
        if ((event.modifiers & Qt.ControlModifier) && event.key >= Qt.Key_1 && event.key <= Qt.Key_9) {
            var tabIndex = event.key - Qt.Key_1
            if (tabIndex < tabs.length && tabIndex < maxTabs) {
                currentIndex = tabIndex
                event.accepted = true
            }
        }
        // Arrow keys for navigation
        else if (event.key === Qt.Key_Left && currentIndex > 0) {
            currentIndex--
            event.accepted = true
        }
        else if (event.key === Qt.Key_Right && currentIndex < tabs.length - 1) {
            currentIndex++
            event.accepted = true
        }
    }
    
    // Signal for tab change
    signal tabChanged(int index)
    
    onCurrentIndexChanged: {
        tabChanged(currentIndex)
    }
    
    // Accessibility
    Accessible.role: Accessible.PageTabList
    Accessible.name: "Tab Navigation"
    Accessible.description: "Navigate between " + tabs.length + " tabs"
}

