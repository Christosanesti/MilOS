import QtQuick 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.core 2.1 as PlasmaCore

Rectangle {
    id: buttonRoot
    
    property string buttonText: ""
    property string buttonIcon: ""
    property color buttonColor: "#4A90E2"
    property bool isHovered: false
    
    signal clicked()
    
    height: units.gridUnit * 3
    radius: units.smallSpacing
    
    // Xenon glow effect - gradient background
    gradient: Gradient {
        GradientStop { position: 0.0; color: buttonColor }
        GradientStop { position: 1.0; color: Qt.darker(buttonColor, 1.2) }
    }
    
    // Glow effect border (xenon styling)
    Rectangle {
        anchors.fill: parent
        radius: parent.radius
        color: "transparent"
        border.color: buttonRoot.isHovered ? "#00FFFF" : buttonColor
        border.width: buttonRoot.isHovered ? 2 : 1
        opacity: buttonRoot.isHovered ? 0.8 : 0.4
        
        // Pulse animation for hover state (xenon effect)
        SequentialAnimation on opacity {
            running: buttonRoot.isHovered
            loops: Animation.Infinite
            NumberAnimation { from: 0.4; to: 0.8; duration: 1000 }
            NumberAnimation { from: 0.8; to: 0.4; duration: 1000 }
        }
    }
    
    RowLayout {
        anchors.fill: parent
        anchors.margins: units.smallSpacing
        spacing: units.smallSpacing
        
        PlasmaCore.IconItem {
            source: buttonIcon
            width: units.iconSizes.medium
            height: units.iconSizes.medium
        }
        
        Text {
            text: buttonText
            color: "white"
            font.pixelSize: units.fontSizes.medium
            Layout.fillWidth: true
        }
    }
    
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: buttonRoot.isHovered = true
        onExited: buttonRoot.isHovered = false
        onClicked: buttonRoot.clicked()
    }
}

