import QtQuick 2.15
import QtQuick.Controls 2.15
import org.milos.Components 1.0

Item {
    id: indicator
    
    property string themeName: ""
    property var compatibilityDB: null
    
    width: 100
    height: 30
    
    StatusIndicator {
        id: statusIndicator
        anchors.centerIn: parent
        
        status: {
            if (!compatibilityDB || !themeName) return "unknown"
            var status = compatibilityDB.getThemeStatus(themeName)
            if (status === 0) return "success" // Compatible
            if (status === 1) return "error" // Incompatible
            return "warning" // Unknown
        }
        
        text: {
            if (!compatibilityDB || !themeName) return "?"
            var status = compatibilityDB.getThemeStatus(themeName)
            if (status === 0) return "✓"
            if (status === 1) return "✗"
            return "?"
        }
    }
    
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        
        ToolTip {
            visible: parent.containsMouse
            text: {
                if (!compatibilityDB || !themeName) return "Unknown compatibility"
                var status = compatibilityDB.getThemeStatus(themeName)
                if (status === 0) return "Compatible with MilOS xenon effects"
                if (status === 1) return "Incompatible - may cause rendering issues"
                return "Compatibility unknown - click to validate"
            }
        }
        
        onClicked: {
            if (compatibilityReportDialog) {
                compatibilityReportDialog.themeName = indicator.themeName
                compatibilityReportDialog.open()
            }
        }
    }
}

