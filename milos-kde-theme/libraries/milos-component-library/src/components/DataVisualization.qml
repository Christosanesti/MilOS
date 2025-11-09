import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Shapes 1.15

Rectangle {
    id: dataViz
    
    // Properties
    property string variant: "line" // line, bar, area, network, stream
    property string state: "default" // default, hover, active, animated, error
    property var data: [] // Array of data points
    property var networkNodes: [] // For network graph
    property var networkConnections: [] // For network graph
    property color xenonColor: "#00d4ff"
    property color xenonHoverColor: "#00ffff"
    property color xenonErrorColor: "#ff4444"
    property int hoveredIndex: -1
    property int selectedIndex: -1
    
    // Dimensions
    width: 400
    height: 300
    implicitWidth: 400
    implicitHeight: 300
    
    // Base styling
    color: "#0a0a0a"
    radius: 8
    border.width: 1
    border.color: xenonColor
    opacity: state === "error" ? 0.7 : 1.0
    
    // Error state indicator
    Rectangle {
        anchors.fill: parent
        color: xenonErrorColor
        opacity: state === "error" ? 0.1 : 0.0
        radius: parent.radius
        
        SequentialAnimation on opacity {
            running: state === "error"
            loops: Animation.Infinite
            NumberAnimation {
                from: 0.05
                to: 0.15
                duration: 1000
                easing.type: Easing.InOutSine
            }
            NumberAnimation {
                from: 0.15
                to: 0.05
                duration: 1000
                easing.type: Easing.InOutSine
            }
        }
        
        Text {
            anchors.centerIn: parent
            text: "Error loading data"
            font.pixelSize: 14
            color: xenonErrorColor
            visible: state === "error"
        }
    }
    
    // Chart area
    Item {
        id: chartArea
        anchors.fill: parent
        anchors.margins: 40
        visible: state !== "error"
        
        // Line Chart
        Canvas {
            id: lineChart
            anchors.fill: parent
            visible: variant === "line" || variant === "area"
            
            onPaint: {
                var ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)
                
                if (data.length < 2) return
                
                // Calculate scale
                var maxValue = Math.max.apply(null, data.map(d => d.y))
                var minValue = Math.min.apply(null, data.map(d => d.y))
                var range = maxValue - minValue || 1
                var xScale = width / (data.length - 1)
                var yScale = height / range
                
                // Draw line/area
                ctx.strokeStyle = xenonColor
                ctx.lineWidth = 2
                ctx.beginPath()
                
                for (var i = 0; i < data.length; i++) {
                    var x = i * xScale
                    var y = height - (data[i].y - minValue) * yScale
                    
                    if (i === 0) {
                        ctx.moveTo(x, y)
                    } else {
                        ctx.lineTo(x, y)
                    }
                }
                
                if (variant === "area") {
                    ctx.lineTo(width, height)
                    ctx.lineTo(0, height)
                    ctx.closePath()
                    ctx.fillStyle = Qt.rgba(0, 0.83, 1, 0.2) // Xenon color with opacity
                    ctx.fill()
                }
                
                ctx.stroke()
                
                // Draw data points
                for (i = 0; i < data.length; i++) {
                    x = i * xScale
                    y = height - (data[i].y - minValue) * yScale
                    
                    var isHovered = hoveredIndex === i
                    var isSelected = selectedIndex === i
                    
                    ctx.beginPath()
                    ctx.arc(x, y, isHovered || isSelected ? 8 : 4, 0, 2 * Math.PI)
                    
                    if (isSelected) {
                        ctx.fillStyle = xenonColor
                        ctx.fill()
                        // Pulse animation
                        ctx.strokeStyle = xenonColor
                        ctx.lineWidth = 2
                    } else if (isHovered) {
                        ctx.fillStyle = xenonHoverColor
                        ctx.fill()
                        ctx.strokeStyle = xenonHoverColor
                        ctx.lineWidth = 2
                    } else {
                        ctx.fillStyle = xenonColor
                        ctx.fill()
                    }
                    ctx.stroke()
                }
            }
            
            onDataChanged: requestPaint()
        }
        
        // Bar Chart
        Repeater {
            model: variant === "bar" ? data.length : 0
            
            Rectangle {
                property var itemData: data[index]
                property var maxValue: Math.max.apply(null, data.map(d => d.y))
                property var barHeight: (itemData.y / maxValue) * chartArea.height
                
                x: (index * chartArea.width / data.length) + 4
                y: chartArea.height - barHeight
                width: (chartArea.width / data.length) - 8
                height: barHeight
                color: hoveredIndex === index ? xenonHoverColor : xenonColor
                opacity: hoveredIndex === index ? 1.0 : 0.7
                
                Rectangle {
                    anchors.fill: parent
                    color: xenonColor
                    opacity: 0.3
                    visible: hoveredIndex === index
                }
                
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: hoveredIndex = index
                    onExited: hoveredIndex = -1
                    onClicked: {
                        selectedIndex = index
                        pointSelected(index)
                    }
                }
            }
        }
        
        // Network Graph
        Repeater {
            model: variant === "network" ? networkConnections : 0
            
            Shape {
                property var connection: networkConnections[index]
                property var fromNode: networkNodes.find(n => n.id === connection.from)
                property var toNode: networkNodes.find(n => n.id === connection.to)
                
                ShapePath {
                    strokeWidth: 2 + (connection.traffic || 0) / 10
                    strokeColor: xenonColor
                    opacity: 0.3 + (connection.traffic || 0) / 100
                    
                    startX: fromNode ? (fromNode.x * chartArea.width) : 0
                    startY: fromNode ? (fromNode.y * chartArea.height) : 0
                    PathLine {
                        x: toNode ? (toNode.x * chartArea.width) : 0
                        y: toNode ? (toNode.y * chartArea.height) : 0
                    }
                }
            }
        }
        
        // Network nodes
        Repeater {
            model: variant === "network" ? networkNodes : 0
            
            Rectangle {
                property var node: networkNodes[index]
                x: (node.x * chartArea.width) - 8
                y: (node.y * chartArea.height) - 8
                width: 16
                height: 16
                radius: 8
                color: xenonColor
                
                SequentialAnimation on opacity {
                    running: true
                    loops: Animation.Infinite
                    NumberAnimation {
                        from: 0.6
                        to: 1.0
                        duration: 2000
                        easing.type: Easing.InOutSine
                    }
                    NumberAnimation {
                        from: 1.0
                        to: 0.6
                        duration: 2000
                        easing.type: Easing.InOutSine
                    }
                }
            }
        }
        
        // Real-time stream particles (simplified)
        Repeater {
            model: variant === "stream" && state === "animated" ? 20 : 0
            
            Rectangle {
                x: Math.random() * chartArea.width
                y: Math.random() * chartArea.height
                width: 4
                height: 4
                radius: 2
                color: xenonColor
                
                SequentialAnimation on opacity {
                    running: true
                    loops: Animation.Infinite
                    NumberAnimation {
                        from: 0.0
                        to: 1.0
                        duration: 1000
                        easing.type: Easing.EaseOut
                    }
                    NumberAnimation {
                        from: 1.0
                        to: 0.0
                        duration: 1000
                        easing.type: Easing.EaseIn
                    }
                }
            }
        }
    }
    
    // Legend
    Column {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 12
        spacing: 8
        visible: variant !== "network" && variant !== "stream"
        
        Repeater {
            model: ["Data Series"]
            
            Row {
                spacing: 8
                
                Rectangle {
                    width: 16
                    height: 16
                    color: xenonColor
                    radius: 2
                }
                
                Text {
                    text: modelData
                    font.pixelSize: 12
                    color: "#ffffff"
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }
    
    // Update animation
    onDataChanged: {
        if (state === "animated") {
            lineChart.requestPaint()
        }
    }
    
    // Signals
    signal pointSelected(int index)
    
    // Accessibility
    Accessible.role: Accessible.Graphic
    Accessible.name: variant + " chart"
    Accessible.description: "Data visualization with " + data.length + " data points"
}

