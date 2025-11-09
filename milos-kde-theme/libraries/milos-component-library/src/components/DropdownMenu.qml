import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: dropdown
    
    // Properties
    property string variant: "standard" // standard, multiselect, searchable, icon, context
    property bool open: false
    property var items: [] // Array of {text: "", icon: "", value: "", enabled: true}
    property var selectedItems: [] // For multiselect
    property string selectedValue: ""
    property string searchText: ""
    property string placeholder: "Select..."
    property bool enabled: true
    property color xenonColor: "#00d4ff"
    property color xenonHoverColor: "#00ffff"
    
    // Dimensions
    width: 200
    height: open ? (Math.min(filteredItems.length, 8) * 40 + 48) : 48
    implicitWidth: 200
    implicitHeight: 48
    
    // Base styling
    color: "#2a2a2a"
    radius: 8
    border.width: 2
    border.color: {
        if (!enabled) return "#666666"
        if (open) return xenonColor
        return xenonColor
    }
    
    // Xenon border glow
    Rectangle {
        anchors.fill: parent
        radius: parent.radius
        color: "transparent"
        border.color: xenonColor
        border.width: 2
        opacity: {
            if (!enabled) return 0.2
            if (open) return 0.8
            return 0.4
        }
        
        Behavior on opacity {
            NumberAnimation { duration: 200; easing.type: Easing.EaseOut }
        }
    }
    
    // Selected value display
    Row {
        anchors.left: parent.left
        anchors.leftMargin: 12
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: arrowIcon.left
        anchors.rightMargin: 8
        spacing: 8
        visible: !open
        
        // Icon (if variant is icon)
        Text {
            text: {
                var selectedItem = items.find(item => item.value === selectedValue)
                return selectedItem && selectedItem.icon ? selectedItem.icon : ""
            }
            font.pixelSize: 16
            color: enabled ? "#ffffff" : "#666666"
            visible: variant === "icon" && text.length > 0
            anchors.verticalCenter: parent.verticalCenter
        }
        
        // Selected text
        Text {
            width: parent.width - (iconText.visible ? iconText.width + 8 : 0)
            text: {
                if (variant === "multiselect" && selectedItems.length > 0) {
                    return selectedItems.length + " selected"
                }
                var selectedItem = items.find(item => item.value === selectedValue)
                return selectedItem ? selectedItem.text : placeholder
            }
            font.pixelSize: 14
            color: enabled ? (selectedValue.length > 0 ? "#ffffff" : "#999999") : "#666666"
            elide: Text.ElideRight
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    
    // Arrow icon
    Text {
        id: arrowIcon
        anchors.right: parent.right
        anchors.rightMargin: 12
        anchors.verticalCenter: parent.verticalCenter
        text: open ? "▲" : "▼"
        font.pixelSize: 12
        color: enabled ? xenonColor : "#666666"
        
        RotationAnimation on rotation {
            running: open
            from: 0
            to: 180
            duration: 200
            easing.type: Easing.EaseOut
        }
    }
    
    // Search input (for searchable variant)
    Rectangle {
        id: searchContainer
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 40
        visible: open && variant === "searchable"
        color: "#1a1a1a"
        
        TextInput {
            id: searchInput
            anchors.fill: parent
            anchors.margins: 8
            text: searchText
            font.pixelSize: 14
            color: "#ffffff"
            focus: open && variant === "searchable"
            
            onTextChanged: {
                dropdown.searchText = text
            }
        }
    }
    
    // Dropdown menu
    Rectangle {
        id: menuContainer
        anchors.top: searchContainer.visible ? searchContainer.bottom : parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: Math.min(filteredItems.length, 8) * 40
        visible: open
        color: "#1a1a1a"
        radius: 8
        border.width: 1
        border.color: xenonColor
        opacity: 0.95
        
        // Slide-down animation (Dropdown Open micro-interaction)
        y: open ? 0 : -height
        
        ParallelAnimation {
            id: openAnimation
            running: open
            NumberAnimation {
                target: menuContainer
                property: "y"
                from: -menuContainer.height
                to: 0
                duration: 200
                easing.type: Easing.EaseOut
            }
            NumberAnimation {
                target: menuContainer
                property: "opacity"
                from: 0.0
                to: 0.95
                duration: 200
                easing.type: Easing.EaseOut
            }
        }
        
        // Menu items
        ListView {
            id: menuList
            anchors.fill: parent
            anchors.margins: 4
            model: filteredItems
            clip: true
            currentIndex: -1
            
            delegate: Rectangle {
                id: menuItem
                width: parent.width
                height: 40
                property bool isSelected: variant === "multiselect" ? 
                    (selectedItems.indexOf(modelData.value) >= 0) : 
                    (modelData.value === selectedValue)
                property bool isHovered: ListView.isCurrentItem
                
                color: {
                    if (!modelData.enabled) return "transparent"
                    if (isHovered) return xenonColor
                    if (isSelected) return "#1a2a2a"
                    return "transparent"
                }
                opacity: modelData.enabled ? 1.0 : 0.5
                
                // Xenon hover glow
                Rectangle {
                    anchors.fill: parent
                    color: xenonHoverColor
                    opacity: isHovered ? 0.2 : 0.0
                    Behavior on opacity {
                        NumberAnimation { duration: 150; easing.type: Easing.EaseOut }
                    }
                }
                
                // Selected indicator
                Rectangle {
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: 3
                    color: xenonColor
                    visible: isSelected
                }
                
                Row {
                    anchors.left: parent.left
                    anchors.leftMargin: 12
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: checkboxIndicator.left
                    anchors.rightMargin: 8
                    spacing: 8
                    
                    // Icon
                    Text {
                        text: modelData.icon || ""
                        font.pixelSize: 16
                        color: "#ffffff"
                        visible: (variant === "icon" || variant === "context") && text.length > 0
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    
                    // Text with search highlight
                    Text {
                        width: parent.width - (iconItem.visible ? iconItem.width + 8 : 0)
                        text: modelData.text
                        font.pixelSize: 14
                        color: modelData.enabled ? "#ffffff" : "#666666"
                        elide: Text.ElideRight
                        anchors.verticalCenter: parent.verticalCenter
                        
                        // Highlight search matches
                        Text {
                            anchors.fill: parent
                            text: parent.text
                            font: parent.font
                            color: xenonColor
                            visible: variant === "searchable" && searchText.length > 0 && 
                                     parent.text.toLowerCase().includes(searchText.toLowerCase())
                            // Simple highlight - can be enhanced with actual highlighting
                        }
                    }
                }
                
                // Checkbox for multiselect
                Rectangle {
                    id: checkboxIndicator
                    anchors.right: parent.right
                    anchors.rightMargin: 12
                    anchors.verticalCenter: parent.verticalCenter
                    width: 18
                    height: 18
                    radius: 4
                    border.width: 2
                    border.color: xenonColor
                    color: isSelected ? xenonColor : "transparent"
                    visible: variant === "multiselect"
                    
                    Text {
                        anchors.centerIn: parent
                        text: "✓"
                        font.pixelSize: 12
                        color: "#0a0a0a"
                        visible: isSelected
                    }
                }
                
                MouseArea {
                    anchors.fill: parent
                    enabled: modelData.enabled
                    hoverEnabled: true
                    onClicked: {
                        if (variant === "multiselect") {
                            var index = selectedItems.indexOf(modelData.value)
                            if (index >= 0) {
                                selectedItems.splice(index, 1)
                            } else {
                                selectedItems.push(modelData.value)
                            }
                            selectedItemsChanged()
                        } else {
                            selectedValue = modelData.value
                            open = false
                        }
                        itemSelected(modelData.value)
                    }
                    onEntered: {
                        menuList.currentIndex = index
                    }
                }
            }
        }
    }
    
    // Filtered items based on search
    property var filteredItems: {
        if (variant === "searchable" && searchText.length > 0) {
            return items.filter(item => 
                item.text.toLowerCase().includes(searchText.toLowerCase())
            )
        }
        return items
    }
    
    // Toggle button
    MouseArea {
        anchors.fill: parent
        enabled: dropdown.enabled && !open
        onClicked: {
            open = !open
            if (open) {
                searchText = ""
                if (variant === "searchable") {
                    searchInput.forceActiveFocus()
                }
            }
        }
    }
    
    // Keyboard handling
    Keys.onPressed: {
        if (event.key === Qt.Key_Space || event.key === Qt.Key_Return) {
            if (!open) {
                open = true
                event.accepted = true
            }
        }
        if (event.key === Qt.Key_Escape && open) {
            open = false
            event.accepted = true
        }
        if (event.key === Qt.Key_Down && open) {
            if (menuList.currentIndex < filteredItems.length - 1) {
                menuList.currentIndex++
            }
            event.accepted = true
        }
        if (event.key === Qt.Key_Up && open) {
            if (menuList.currentIndex > 0) {
                menuList.currentIndex--
            }
            event.accepted = true
        }
        if (event.key === Qt.Key_Return && open && menuList.currentIndex >= 0) {
            var item = filteredItems[menuList.currentIndex]
            if (item && item.enabled) {
                if (variant === "multiselect") {
                    var index = selectedItems.indexOf(item.value)
                    if (index >= 0) {
                        selectedItems.splice(index, 1)
                    } else {
                        selectedItems.push(item.value)
                    }
                    selectedItemsChanged()
                } else {
                    selectedValue = item.value
                    open = false
                }
                itemSelected(item.value)
            }
            event.accepted = true
        }
    }
    
    // Focus management
    onOpenChanged: {
        if (open) {
            forceActiveFocus()
        }
    }
    
    // Signals
    signal itemSelected(string value)
    signal selectedItemsChanged()
    
    // Accessibility
    Accessible.role: Accessible.ComboBox
    Accessible.name: placeholder
    Accessible.description: "Dropdown menu with " + items.length + " items"
    Accessible.onPressAction: {
        if (enabled) {
            open = !open
        }
    }
}

