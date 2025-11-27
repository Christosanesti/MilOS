import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Milos.Components 1.0
import Milos.UI 1.0

/**
 * Crash Report Dialog Component
 * 
 * Displays crash information and allows user to submit bug report.
 */
Dialog {
    id: crashDialog
    
    property var crashInfo: null
    property bool includeSystemInfo: true
    property bool includeStackTrace: true
    
    modal: true
    width: Math.min(700, parent.width * 0.9)
    height: Math.min(600, parent.height * 0.8)
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    
    // Accessibility
    Accessible.name: "Crash Report Dialog"
    Accessible.description: "Application has crashed. Please submit a bug report."
    
    background: Rectangle {
        color: "#1a1a1a"
        radius: 4
        border.color: "#ff0000"  // Red border for crash
        border.width: 2
        
        // Xenon glow effect for crash
        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: "#ff0000"
            border.width: 1
            opacity: 0.5
            
            SequentialAnimation on opacity {
                running: true
                loops: Animation.Infinite
                NumberAnimation { to: 0.8; duration: 1000 }
                NumberAnimation { to: 0.5; duration: 1000 }
            }
        }
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 16
        
        // Crash Icon and Title
        RowLayout {
            Layout.fillWidth: true
            spacing: 12
            
            // Crash Icon
            Rectangle {
                width: 64
                height: 64
                radius: 32
                color: "#ff0000"
                
                Text {
                    anchors.centerIn: parent
                    text: "!"
                    font.pixelSize: 36
                    font.bold: true
                    color: "#ffffff"
                }
            }
            
            // Title
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4
                
                Text {
                    text: "Application Crashed"
                    font.pixelSize: 24
                    font.bold: true
                    color: "#ffffff"
                }
                
                Text {
                    text: crashInfo ? crashInfo.errorMessage : "An unexpected error occurred"
                    font.pixelSize: 14
                    color: "#e0e0e0"
                    wrapMode: Text.WordWrap
                }
            }
        }
        
        // Crash Information
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredHeight: 200
            
            ColumnLayout {
                width: crashDialog.width - 80
                spacing: 12
                
                // Basic Information
                GroupBox {
                    Layout.fillWidth: true
                    title: "Crash Information"
                    
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 8
                        
                        InfoRow {
                            label: "Crash ID:"
                            value: crashInfo ? crashInfo.crashId : "N/A"
                        }
                        
                        InfoRow {
                            label: "Time:"
                            value: crashInfo ? crashInfo.timestamp : "N/A"
                        }
                        
                        InfoRow {
                            label: "Signal:"
                            value: crashInfo ? crashInfo.signal : "N/A"
                        }
                    }
                }
                
                // System Information (collapsible)
                GroupBox {
                    Layout.fillWidth: true
                    title: "System Information"
                    visible: includeSystemInfo
                    
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 8
                        
                        InfoRow {
                            label: "OS:"
                            value: crashInfo && crashInfo.systemInfo ? crashInfo.systemInfo.os_name : "N/A"
                        }
                        
                        InfoRow {
                            label: "Kernel:"
                            value: crashInfo && crashInfo.systemInfo ? crashInfo.systemInfo.kernel_version : "N/A"
                        }
                        
                        InfoRow {
                            label: "Architecture:"
                            value: crashInfo && crashInfo.systemInfo ? crashInfo.systemInfo.architecture : "N/A"
                        }
                    }
                }
                
                // Stack Trace (collapsible)
                GroupBox {
                    Layout.fillWidth: true
                    title: "Stack Trace"
                    visible: includeStackTrace && crashInfo && crashInfo.stackTrace
                    
                    ScrollView {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 150
                        
                        Text {
                            width: crashDialog.width - 120
                            text: crashInfo ? crashInfo.stackTrace : ""
                            font.pixelSize: 11
                            font.family: "monospace"
                            color: "#b0b0b0"
                            wrapMode: Text.Wrap
                        }
                    }
                }
            }
        }
        
        // User Message
        GroupBox {
            Layout.fillWidth: true
            title: "Additional Information (Optional)"
            
            ScrollView {
                Layout.fillWidth: true
                Layout.preferredHeight: 80
                
                TextArea {
                    id: userMessage
                    placeholderText: "Describe what you were doing when the crash occurred..."
                    wrapMode: TextArea.Wrap
                    background: Rectangle {
                        color: "#2a2a2a"
                        border.color: "#00d4ff"
                        border.width: 1
                        radius: 2
                    }
                }
            }
        }
        
        // Privacy Notice
        Text {
            Layout.fillWidth: true
            text: "Crash reports help us improve MilOS. System information and stack traces are included to help diagnose the issue. No personal data is collected."
            font.pixelSize: 11
            color: "#888888"
            wrapMode: Text.WordWrap
        }
        
        // Buttons
        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight
            spacing: 12
            
            Button {
                text: "Don't Send"
                onClicked: {
                    crashDialog.close()
                }
            }
            
            Button {
                text: "Save Report"
                onClicked: {
                    // Report already saved, just close
                    crashDialog.close()
                }
            }
            
            Button {
                text: "Send Report"
                Layout.preferredWidth: 120
                onClicked: {
                    if (crashInfo) {
                        crashHandler.submitCrashReport(crashInfo.logFile, includeSystemInfo)
                    }
                    crashDialog.close()
                }
            }
        }
    }
    
    // Component for info rows
    component InfoRow: RowLayout {
        property string label
        property string value
        
        Layout.fillWidth: true
        
        Text {
            text: label
            font.pixelSize: 12
            font.bold: true
            color: "#b0b0b0"
            Layout.preferredWidth: 120
        }
        
        Text {
            Layout.fillWidth: true
            text: value
            font.pixelSize: 12
            color: "#e0e0e0"
            elide: Text.ElideRight
        }
    }
    
    // Keyboard navigation
    Keys.onEscapePressed: {
        close()
    }
}



import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Milos.Components 1.0
import Milos.UI 1.0

/**
 * Crash Report Dialog Component
 * 
 * Displays crash information and allows user to submit bug report.
 */
Dialog {
    id: crashDialog
    
    property var crashInfo: null
    property bool includeSystemInfo: true
    property bool includeStackTrace: true
    
    modal: true
    width: Math.min(700, parent.width * 0.9)
    height: Math.min(600, parent.height * 0.8)
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    
    // Accessibility
    Accessible.name: "Crash Report Dialog"
    Accessible.description: "Application has crashed. Please submit a bug report."
    
    background: Rectangle {
        color: "#1a1a1a"
        radius: 4
        border.color: "#ff0000"  // Red border for crash
        border.width: 2
        
        // Xenon glow effect for crash
        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: "#ff0000"
            border.width: 1
            opacity: 0.5
            
            SequentialAnimation on opacity {
                running: true
                loops: Animation.Infinite
                NumberAnimation { to: 0.8; duration: 1000 }
                NumberAnimation { to: 0.5; duration: 1000 }
            }
        }
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 16
        
        // Crash Icon and Title
        RowLayout {
            Layout.fillWidth: true
            spacing: 12
            
            // Crash Icon
            Rectangle {
                width: 64
                height: 64
                radius: 32
                color: "#ff0000"
                
                Text {
                    anchors.centerIn: parent
                    text: "!"
                    font.pixelSize: 36
                    font.bold: true
                    color: "#ffffff"
                }
            }
            
            // Title
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4
                
                Text {
                    text: "Application Crashed"
                    font.pixelSize: 24
                    font.bold: true
                    color: "#ffffff"
                }
                
                Text {
                    text: crashInfo ? crashInfo.errorMessage : "An unexpected error occurred"
                    font.pixelSize: 14
                    color: "#e0e0e0"
                    wrapMode: Text.WordWrap
                }
            }
        }
        
        // Crash Information
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredHeight: 200
            
            ColumnLayout {
                width: crashDialog.width - 80
                spacing: 12
                
                // Basic Information
                GroupBox {
                    Layout.fillWidth: true
                    title: "Crash Information"
                    
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 8
                        
                        InfoRow {
                            label: "Crash ID:"
                            value: crashInfo ? crashInfo.crashId : "N/A"
                        }
                        
                        InfoRow {
                            label: "Time:"
                            value: crashInfo ? crashInfo.timestamp : "N/A"
                        }
                        
                        InfoRow {
                            label: "Signal:"
                            value: crashInfo ? crashInfo.signal : "N/A"
                        }
                    }
                }
                
                // System Information (collapsible)
                GroupBox {
                    Layout.fillWidth: true
                    title: "System Information"
                    visible: includeSystemInfo
                    
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 8
                        
                        InfoRow {
                            label: "OS:"
                            value: crashInfo && crashInfo.systemInfo ? crashInfo.systemInfo.os_name : "N/A"
                        }
                        
                        InfoRow {
                            label: "Kernel:"
                            value: crashInfo && crashInfo.systemInfo ? crashInfo.systemInfo.kernel_version : "N/A"
                        }
                        
                        InfoRow {
                            label: "Architecture:"
                            value: crashInfo && crashInfo.systemInfo ? crashInfo.systemInfo.architecture : "N/A"
                        }
                    }
                }
                
                // Stack Trace (collapsible)
                GroupBox {
                    Layout.fillWidth: true
                    title: "Stack Trace"
                    visible: includeStackTrace && crashInfo && crashInfo.stackTrace
                    
                    ScrollView {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 150
                        
                        Text {
                            width: crashDialog.width - 120
                            text: crashInfo ? crashInfo.stackTrace : ""
                            font.pixelSize: 11
                            font.family: "monospace"
                            color: "#b0b0b0"
                            wrapMode: Text.Wrap
                        }
                    }
                }
            }
        }
        
        // User Message
        GroupBox {
            Layout.fillWidth: true
            title: "Additional Information (Optional)"
            
            ScrollView {
                Layout.fillWidth: true
                Layout.preferredHeight: 80
                
                TextArea {
                    id: userMessage
                    placeholderText: "Describe what you were doing when the crash occurred..."
                    wrapMode: TextArea.Wrap
                    background: Rectangle {
                        color: "#2a2a2a"
                        border.color: "#00d4ff"
                        border.width: 1
                        radius: 2
                    }
                }
            }
        }
        
        // Privacy Notice
        Text {
            Layout.fillWidth: true
            text: "Crash reports help us improve MilOS. System information and stack traces are included to help diagnose the issue. No personal data is collected."
            font.pixelSize: 11
            color: "#888888"
            wrapMode: Text.WordWrap
        }
        
        // Buttons
        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight
            spacing: 12
            
            Button {
                text: "Don't Send"
                onClicked: {
                    crashDialog.close()
                }
            }
            
            Button {
                text: "Save Report"
                onClicked: {
                    // Report already saved, just close
                    crashDialog.close()
                }
            }
            
            Button {
                text: "Send Report"
                Layout.preferredWidth: 120
                onClicked: {
                    if (crashInfo) {
                        crashHandler.submitCrashReport(crashInfo.logFile, includeSystemInfo)
                    }
                    crashDialog.close()
                }
            }
        }
    }
    
    // Component for info rows
    component InfoRow: RowLayout {
        property string label
        property string value
        
        Layout.fillWidth: true
        
        Text {
            text: label
            font.pixelSize: 12
            font.bold: true
            color: "#b0b0b0"
            Layout.preferredWidth: 120
        }
        
        Text {
            Layout.fillWidth: true
            text: value
            font.pixelSize: 12
            color: "#e0e0e0"
            elide: Text.ElideRight
        }
    }
    
    // Keyboard navigation
    Keys.onEscapePressed: {
        close()
    }
}


