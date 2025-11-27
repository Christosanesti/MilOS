import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Milos.Components 1.0

/**
 * Error Dialog Component
 * 
 * Displays user-friendly error messages with suggestions and retry options.
 * Supports graceful degradation and accessibility.
 */
Dialog {
    id: errorDialog
    
    property string errorTitle: ""
    property string errorMessage: ""
    property string detailedMessage: ""
    property int errorSeverity: 0  // 0=Info, 1=Warning, 2=Error, 3=Critical
    property var suggestions: []
    property bool canRetry: false
    property string retryAction: ""
    
    signal retryClicked()
    signal dismissed()
    
    modal: true
    width: Math.min(600, parent.width * 0.9)
    height: Math.min(500, parent.height * 0.8)
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    
    // Accessibility
    Accessible.name: errorTitle || "Error Dialog"
    Accessible.description: errorMessage
    
    // Xenon border effect (fallback to high contrast if unavailable)
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.color: getBorderColor()
        border.width: 2
        
        // Xenon glow effect (if available)
        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: getGlowColor()
            border.width: 1
            opacity: 0.5
            
            SequentialAnimation on opacity {
                running: errorSeverity >= 2  // Only for Error/Critical
                loops: Animation.Infinite
                NumberAnimation { to: 0.8; duration: 1000 }
                NumberAnimation { to: 0.5; duration: 1000 }
            }
        }
    }
    
    function getBorderColor() {
        switch (errorSeverity) {
            case 3: return "#ff0000"  // Critical - Red
            case 2: return "#ff8800"  // Error - Orange
            case 1: return "#ffaa00"  // Warning - Yellow
            default: return "#00d4ff"  // Info - Cyan
        }
    }
    
    function getGlowColor() {
        return getBorderColor()
    }
    
    background: Rectangle {
        color: "#1a1a1a"
        radius: 4
        border.color: getBorderColor()
        border.width: 2
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 16
        
        // Error Icon and Title
        RowLayout {
            Layout.fillWidth: true
            spacing: 12
            
            // Error Icon
            Rectangle {
                width: 48
                height: 48
                radius: 24
                color: getIconBackgroundColor()
                
                Text {
                    anchors.centerIn: parent
                    text: getIconText()
                    font.pixelSize: 24
                    color: "#ffffff"
                }
            }
            
            // Title
            Text {
                Layout.fillWidth: true
                text: errorTitle || "Error"
                font.pixelSize: 20
                font.bold: true
                color: "#ffffff"
                wrapMode: Text.WordWrap
            }
        }
        
        // Error Message
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredHeight: 150
            
            Text {
                width: errorDialog.width - 80
                text: errorMessage
                font.pixelSize: 14
                color: "#e0e0e0"
                wrapMode: Text.WordWrap
                lineHeight: 1.5
            }
        }
        
        // Detailed Message (collapsible)
        ColumnLayout {
            Layout.fillWidth: true
            visible: detailedMessage !== "" && detailedMessage !== errorMessage
            spacing: 8
            
            Button {
                Layout.fillWidth: true
                text: showDetails ? "Hide Details" : "Show Details"
                onClicked: showDetails = !showDetails
                
                property bool showDetails: false
            }
            
            ScrollView {
                Layout.fillWidth: true
                Layout.preferredHeight: 100
                visible: showDetails
                
                Text {
                    width: errorDialog.width - 80
                    text: detailedMessage
                    font.pixelSize: 12
                    font.family: "monospace"
                    color: "#b0b0b0"
                    wrapMode: Text.WordWrap
                }
            }
        }
        
        // Suggestions
        ColumnLayout {
            Layout.fillWidth: true
            visible: suggestions.length > 0
            spacing: 8
            
            Text {
                text: "Suggestions:"
                font.pixelSize: 14
                font.bold: true
                color: "#ffffff"
            }
            
            Repeater {
                model: suggestions
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    
                    Text {
                        text: "•"
                        color: "#00d4ff"
                        font.pixelSize: 14
                    }
                    
                    Text {
                        Layout.fillWidth: true
                        text: modelData
                        font.pixelSize: 13
                        color: "#e0e0e0"
                        wrapMode: Text.WordWrap
                    }
                }
            }
        }
        
        // Buttons
        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight
            spacing: 12
            
            Button {
                text: "Retry"
                visible: canRetry
                onClicked: {
                    retryClicked()
                    errorDialog.close()
                }
            }
            
            Button {
                text: "OK"
                Layout.preferredWidth: 100
                onClicked: {
                    dismissed()
                    errorDialog.close()
                }
            }
        }
    }
    
    function getIconBackgroundColor() {
        switch (errorSeverity) {
            case 3: return "#ff0000"  // Critical
            case 2: return "#ff8800"  // Error
            case 1: return "#ffaa00"  // Warning
            default: return "#00d4ff"  // Info
        }
    }
    
    function getIconText() {
        switch (errorSeverity) {
            case 3: return "!"
            case 2: return "×"
            case 1: return "!"
            default: return "i"
        }
    }
    
    // Keyboard navigation
    Keys.onEscapePressed: {
        dismissed()
        close()
    }
    
    Keys.onEnterPressed: {
        if (canRetry) {
            retryClicked()
        } else {
            dismissed()
        }
        close()
    }
}



import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Milos.Components 1.0

/**
 * Error Dialog Component
 * 
 * Displays user-friendly error messages with suggestions and retry options.
 * Supports graceful degradation and accessibility.
 */
Dialog {
    id: errorDialog
    
    property string errorTitle: ""
    property string errorMessage: ""
    property string detailedMessage: ""
    property int errorSeverity: 0  // 0=Info, 1=Warning, 2=Error, 3=Critical
    property var suggestions: []
    property bool canRetry: false
    property string retryAction: ""
    
    signal retryClicked()
    signal dismissed()
    
    modal: true
    width: Math.min(600, parent.width * 0.9)
    height: Math.min(500, parent.height * 0.8)
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    
    // Accessibility
    Accessible.name: errorTitle || "Error Dialog"
    Accessible.description: errorMessage
    
    // Xenon border effect (fallback to high contrast if unavailable)
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.color: getBorderColor()
        border.width: 2
        
        // Xenon glow effect (if available)
        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: getGlowColor()
            border.width: 1
            opacity: 0.5
            
            SequentialAnimation on opacity {
                running: errorSeverity >= 2  // Only for Error/Critical
                loops: Animation.Infinite
                NumberAnimation { to: 0.8; duration: 1000 }
                NumberAnimation { to: 0.5; duration: 1000 }
            }
        }
    }
    
    function getBorderColor() {
        switch (errorSeverity) {
            case 3: return "#ff0000"  // Critical - Red
            case 2: return "#ff8800"  // Error - Orange
            case 1: return "#ffaa00"  // Warning - Yellow
            default: return "#00d4ff"  // Info - Cyan
        }
    }
    
    function getGlowColor() {
        return getBorderColor()
    }
    
    background: Rectangle {
        color: "#1a1a1a"
        radius: 4
        border.color: getBorderColor()
        border.width: 2
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 16
        
        // Error Icon and Title
        RowLayout {
            Layout.fillWidth: true
            spacing: 12
            
            // Error Icon
            Rectangle {
                width: 48
                height: 48
                radius: 24
                color: getIconBackgroundColor()
                
                Text {
                    anchors.centerIn: parent
                    text: getIconText()
                    font.pixelSize: 24
                    color: "#ffffff"
                }
            }
            
            // Title
            Text {
                Layout.fillWidth: true
                text: errorTitle || "Error"
                font.pixelSize: 20
                font.bold: true
                color: "#ffffff"
                wrapMode: Text.WordWrap
            }
        }
        
        // Error Message
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredHeight: 150
            
            Text {
                width: errorDialog.width - 80
                text: errorMessage
                font.pixelSize: 14
                color: "#e0e0e0"
                wrapMode: Text.WordWrap
                lineHeight: 1.5
            }
        }
        
        // Detailed Message (collapsible)
        ColumnLayout {
            Layout.fillWidth: true
            visible: detailedMessage !== "" && detailedMessage !== errorMessage
            spacing: 8
            
            Button {
                Layout.fillWidth: true
                text: showDetails ? "Hide Details" : "Show Details"
                onClicked: showDetails = !showDetails
                
                property bool showDetails: false
            }
            
            ScrollView {
                Layout.fillWidth: true
                Layout.preferredHeight: 100
                visible: showDetails
                
                Text {
                    width: errorDialog.width - 80
                    text: detailedMessage
                    font.pixelSize: 12
                    font.family: "monospace"
                    color: "#b0b0b0"
                    wrapMode: Text.WordWrap
                }
            }
        }
        
        // Suggestions
        ColumnLayout {
            Layout.fillWidth: true
            visible: suggestions.length > 0
            spacing: 8
            
            Text {
                text: "Suggestions:"
                font.pixelSize: 14
                font.bold: true
                color: "#ffffff"
            }
            
            Repeater {
                model: suggestions
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    
                    Text {
                        text: "•"
                        color: "#00d4ff"
                        font.pixelSize: 14
                    }
                    
                    Text {
                        Layout.fillWidth: true
                        text: modelData
                        font.pixelSize: 13
                        color: "#e0e0e0"
                        wrapMode: Text.WordWrap
                    }
                }
            }
        }
        
        // Buttons
        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight
            spacing: 12
            
            Button {
                text: "Retry"
                visible: canRetry
                onClicked: {
                    retryClicked()
                    errorDialog.close()
                }
            }
            
            Button {
                text: "OK"
                Layout.preferredWidth: 100
                onClicked: {
                    dismissed()
                    errorDialog.close()
                }
            }
        }
    }
    
    function getIconBackgroundColor() {
        switch (errorSeverity) {
            case 3: return "#ff0000"  // Critical
            case 2: return "#ff8800"  // Error
            case 1: return "#ffaa00"  // Warning
            default: return "#00d4ff"  // Info
        }
    }
    
    function getIconText() {
        switch (errorSeverity) {
            case 3: return "!"
            case 2: return "×"
            case 1: return "!"
            default: return "i"
        }
    }
    
    // Keyboard navigation
    Keys.onEscapePressed: {
        dismissed()
        close()
    }
    
    Keys.onEnterPressed: {
        if (canRetry) {
            retryClicked()
        } else {
            dismissed()
        }
        close()
    }
}


