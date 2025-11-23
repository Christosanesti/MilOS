import QtQuick 2.15
import QtQuick.Controls 2.15
import org.kde.plasma.core 2.1 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents

Rectangle {
    id: emergencyButton
    
    // Properties
    property string actionType: "network" // network, lock, secure, shutdown
    property string label: ""
    property string iconName: "network-disconnect"
    property color buttonColor: "#1a1a1a"
    property color accentColor: "#ff4444" // Red for network/shutdown, cyan for lock, yellow for secure
    property bool compact: false
    
    // Dimensions
    width: compact ? 40 : implicitWidth
    height: compact ? 40 : implicitHeight
    implicitWidth: 200
    implicitHeight: 50
    
    // Base styling
    color: buttonColor
    radius: 6
    border.width: 1
    border.color: mouseArea.containsMouse ? accentColor : "#333333"
    
    // Xenon glow effect
    Rectangle {
        anchors.fill: parent
        radius: parent.radius
        color: "transparent"
        border.width: 2
        border.color: accentColor
        opacity: mouseArea.containsMouse ? 0.6 : 0
        visible: mouseArea.containsMouse
        
        SequentialAnimation on opacity {
            running: mouseArea.containsMouse
            loops: Animation.Infinite
            NumberAnimation {
                from: 0.3
                to: 0.8
                duration: 1000
                easing.type: Easing.InOutSine
            }
            NumberAnimation {
                from: 0.8
                to: 0.3
                duration: 1000
                easing.type: Easing.InOutSine
            }
        }
    }
    
    // Content
    Row {
        anchors.centerIn: parent
        spacing: compact ? 0 : 10
        visible: !compact || mouseArea.containsMouse
        
        PlasmaCore.IconItem {
            id: icon
            source: iconName
            width: compact ? 24 : 32
            height: compact ? 24 : 32
            color: accentColor
            anchors.verticalCenter: parent.verticalCenter
        }
        
        Text {
            id: labelText
            text: label
            color: "#ffffff"
            font.pixelSize: compact ? 12 : 14
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter
            visible: !compact
        }
    }
    
    // Mouse area
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        
        onClicked: {
            parent.clicked()
        }
    }
    
    signal clicked()
    
    // Accessibility
    Accessible.role: Accessible.Button
    Accessible.name: label
    Accessible.description: "Emergency action: " + label
}














