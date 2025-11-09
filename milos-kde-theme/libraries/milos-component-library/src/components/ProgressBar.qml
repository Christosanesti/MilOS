import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: progressBar
    
    // Properties
    property string variant: "linear" // linear, circular, step, indeterminate, details
    property string state: "inprogress" // inprogress, paused, complete, error, indeterminate
    property real value: 0.0 // 0.0 to 1.0
    property real percentage: value * 100
    property string text: ""
    property string details: ""
    property int stepCount: 5
    property int currentStep: 0
    property string estimatedTime: ""
    
    // Xenon color properties
    property color xenonColor: "#00d4ff"
    property color xenonSuccessColor: "#00cc66"
    property color xenonErrorColor: "#ff4444"
    property color xenonWarningColor: "#ffaa00"
    
    // Dimensions
    width: variant === "circular" ? 80 : 300
    height: {
        if (variant === "circular") return 80
        if (variant === "step") return 40
        if (variant === "details") return 60
        return 20 // Linear and indeterminate
    }
    implicitWidth: width
    implicitHeight: height
    
    // Base styling
    color: "transparent"
    
    // Linear Progress Bar
    Rectangle {
        id: linearTrack
        anchors.fill: parent
        visible: variant === "linear" || variant === "indeterminate" || variant === "details"
        color: "#2a2a2a"
        radius: height / 2
        
        Rectangle {
            id: linearFill
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: parent.width * (progressBar.state === "indeterminate" ? 0.3 : progressBar.value)
            radius: parent.radius
            color: {
                if (progressBar.state === "error") return progressBar.xenonErrorColor
                if (progressBar.state === "complete") return progressBar.xenonSuccessColor
                if (progressBar.state === "paused") return progressBar.xenonWarningColor
                return progressBar.xenonColor
            }
            
            // Progress fill pulse animation (Animation 2: Xenon Border Scan)
            Rectangle {
                id: progressPulse
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: parent.width * 0.1
                color: parent.color
                opacity: 0.6
                visible: progressBar.state === "inprogress" || progressBar.state === "indeterminate"
                
                SequentialAnimation on opacity {
                    running: progressBar.state === "inprogress" || progressBar.state === "indeterminate"
                    loops: Animation.Infinite
                    NumberAnimation {
                        from: 0.3
                        to: 0.9
                        duration: 1000
                        easing.type: Easing.InOutSine
                    }
                    NumberAnimation {
                        from: 0.9
                        to: 0.3
                        duration: 1000
                        easing.type: Easing.InOutSine
                    }
                }
            }
            
            // Complete glow celebration (Animation 5: Xenon Success Celebration)
            SequentialAnimation on opacity {
                id: completeCelebration
                running: progressBar.state === "complete"
                NumberAnimation {
                    from: 0.8
                    to: 1.0
                    duration: 200
                    easing.type: Easing.EaseOut
                }
                NumberAnimation {
                    from: 1.0
                    to: 0.6
                    duration: 200
                    easing.type: Easing.EaseIn
                }
                NumberAnimation {
                    from: 0.6
                    to: 1.0
                    duration: 200
                    easing.type: Easing.EaseOut
                }
                NumberAnimation {
                    from: 1.0
                    to: 0.6
                    duration: 200
                    easing.type: Easing.EaseIn
                }
                NumberAnimation {
                    from: 0.6
                    to: 1.0
                    duration: 200
                    easing.type: Easing.EaseOut
                }
                NumberAnimation {
                    from: 1.0
                    to: 0.8
                    duration: 300
                    easing.type: Easing.EaseIn
                }
            }
            
            // Error alert pulse (Animation 4: Xenon Alert Pulse)
            SequentialAnimation on opacity {
                id: errorPulse
                running: progressBar.state === "error"
                loops: Animation.Infinite
                NumberAnimation {
                    from: 0.8
                    to: 1.0
                    duration: 500
                    easing.type: Easing.InOutSine
                }
                NumberAnimation {
                    from: 1.0
                    to: 0.8
                    duration: 500
                    easing.type: Easing.InOutSine
                }
            }
        }
        
        // Indeterminate animation
        Rectangle {
            id: indeterminateBar
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: parent.width * 0.3
            x: progressBar.state === "indeterminate" ? -width : parent.width
            color: progressBar.xenonColor
            radius: parent.radius
            visible: progressBar.state === "indeterminate"
            
            SequentialAnimation on x {
                running: progressBar.state === "indeterminate"
                loops: Animation.Infinite
                NumberAnimation {
                    from: -indeterminateBar.width
                    to: progressBar.width + indeterminateBar.width
                    duration: 2000
                    easing.type: Easing.Linear
                }
            }
        }
    }
    
    // Circular Progress
    Canvas {
        id: circularCanvas
        anchors.fill: parent
        visible: variant === "circular"
        
        onPaint: {
            var ctx = getContext("2d")
            var centerX = width / 2
            var centerY = height / 2
            var radius = Math.min(width, height) / 2 - 4
            var startAngle = -Math.PI / 2
            var endAngle = startAngle + (2 * Math.PI * progressBar.value)
            
            // Background circle
            ctx.beginPath()
            ctx.arc(centerX, centerY, radius, 0, 2 * Math.PI)
            ctx.strokeStyle = "#2a2a2a"
            ctx.lineWidth = 4
            ctx.stroke()
            
            // Progress arc
            ctx.beginPath()
            ctx.arc(centerX, centerY, radius, startAngle, endAngle)
            ctx.strokeStyle = progressBar.state === "complete" ? progressBar.xenonSuccessColor : progressBar.xenonColor
            ctx.lineWidth = 4
            ctx.stroke()
        }
        
        onValueChanged: requestPaint()
        onStateChanged: requestPaint()
    }
    
    // Step Progress
    Row {
        anchors.fill: parent
        spacing: 8
        visible: variant === "step"
        
        Repeater {
            model: progressBar.stepCount
            Rectangle {
                width: (parent.width - (progressBar.stepCount - 1) * 8) / progressBar.stepCount
                height: parent.height
                radius: 4
                color: {
                    if (index < progressBar.currentStep) return progressBar.xenonSuccessColor
                    if (index === progressBar.currentStep) return progressBar.xenonColor
                    return "#2a2a2a"
                }
                
                // Active step pulse
                SequentialAnimation on opacity {
                    running: index === progressBar.currentStep && progressBar.state === "inprogress"
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
        }
    }
    
    // Details variant - shows text and percentage
    Column {
        anchors.fill: parent
        spacing: 4
        visible: variant === "details"
        
        Row {
            width: parent.width
            spacing: 8
            
            Text {
                width: parent.width - percentageText.width - 8
                text: progressBar.text || "Progress"
                font.pixelSize: 14
                color: "#ffffff"
                elide: Text.ElideRight
            }
            
            Text {
                id: percentageText
                text: Math.round(progressBar.percentage) + "%"
                font.pixelSize: 14
                font.weight: Font.Bold
                color: progressBar.xenonColor
            }
        }
        
        // Progress bar
        Rectangle {
            width: parent.width
            height: 8
            color: "#2a2a2a"
            radius: 4
            
            Rectangle {
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: parent.width * progressBar.value
                radius: parent.radius
                color: progressBar.xenonColor
            }
        }
        
        // Details text
        Text {
            width: parent.width
            text: progressBar.details || progressBar.estimatedTime
            font.pixelSize: 12
            color: "#cccccc"
            visible: progressBar.details.length > 0 || progressBar.estimatedTime.length > 0
        }
    }
    
    // Percentage text for linear variant
    Text {
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        text: Math.round(progressBar.percentage) + "%"
        font.pixelSize: 12
        color: progressBar.xenonColor
        visible: variant === "linear" && progressBar.text.length === 0
    }
    
    // Text label for linear variant
    Text {
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        text: progressBar.text
        font.pixelSize: 12
        color: "#ffffff"
        visible: variant === "linear" && progressBar.text.length > 0
    }
    
    // Accessibility
    Accessible.role: Accessible.ProgressBar
    Accessible.name: progressBar.text || "Progress"
    Accessible.value: progressBar.percentage
    Accessible.maximumValue: 100
    Accessible.minimumValue: 0
    Accessible.description: progressBar.details || progressBar.estimatedTime || ""
}

