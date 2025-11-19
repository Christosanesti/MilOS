import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: enhancedTopologyView
    color: "#0a0a0a"
    
    property var nodes: []
    property var edges: []
    property real zoomLevel: 1.0
    property point panOffset: Qt.point(0, 0)
    
    // Interactive canvas with zoom and pan
    Flickable {
        id: flickable
        anchors.fill: parent
        contentWidth: canvas.width * enhancedTopologyView.zoomLevel
        contentHeight: canvas.height * enhancedTopologyView.zoomLevel
        contentX: enhancedTopologyView.panOffset.x
        contentY: enhancedTopologyView.panOffset.y
        
        Canvas {
            id: canvas
            width: enhancedTopologyView.width
            height: enhancedTopologyView.height
            
            onPaint: {
                var ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)
                
                // Apply zoom
                ctx.save()
                ctx.scale(enhancedTopologyView.zoomLevel, enhancedTopologyView.zoomLevel)
                
                // Draw edges
                ctx.strokeStyle = "#666666"
                ctx.lineWidth = 2
                for (var i = 0; i < enhancedTopologyView.edges.length; i++) {
                    var edge = enhancedTopologyView.edges[i]
                    var fromNode = getNodeById(edge.fromNodeId)
                    var toNode = getNodeById(edge.toNodeId)
                    
                    if (fromNode && toNode) {
                        ctx.beginPath()
                        ctx.moveTo(fromNode.x + canvas.width / 2, fromNode.y + canvas.height / 2)
                        ctx.lineTo(toNode.x + canvas.width / 2, toNode.y + canvas.height / 2)
                        ctx.stroke()
                    }
                }
                
                // Draw nodes
                for (var j = 0; j < enhancedTopologyView.nodes.length; j++) {
                    var node = enhancedTopologyView.nodes[j]
                    var x = node.x + canvas.width / 2
                    var y = node.y + canvas.height / 2
                    
                    // Draw security zone (if isolated)
                    if (node.properties && node.properties.isIsolated) {
                        ctx.fillStyle = "rgba(255, 136, 0, 0.2)"
                        ctx.beginPath()
                        ctx.arc(x, y, 50, 0, 2 * Math.PI)
                        ctx.fill()
                    }
                    
                    // Draw node circle
                    ctx.fillStyle = node.properties && node.properties.isIsolated ? "#ff8800" : "#00ff88"
                    ctx.beginPath()
                    ctx.arc(x, y, 30, 0, 2 * Math.PI)
                    ctx.fill()
                    
                    // Draw node border (xenon effect)
                    ctx.strokeStyle = "#00ff88"
                    ctx.lineWidth = 3
                    ctx.beginPath()
                    ctx.arc(x, y, 30, 0, 2 * Math.PI)
                    ctx.stroke()
                    
                    // Draw node label
                    ctx.fillStyle = "#ffffff"
                    ctx.font = "12px Arial"
                    ctx.textAlign = "center"
                    ctx.fillText(node.label || node.nodeId, x, y + 50)
                }
                
                ctx.restore()
            }
            
            function getNodeById(nodeId) {
                for (var i = 0; i < enhancedTopologyView.nodes.length; i++) {
                    if (enhancedTopologyView.nodes[i].nodeId === nodeId) {
                        return enhancedTopologyView.nodes[i]
                    }
                }
                return null
            }
            
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    // Handle node selection
                    var clickedX = mouse.x / enhancedTopologyView.zoomLevel - canvas.width / 2
                    var clickedY = mouse.y / enhancedTopologyView.zoomLevel - canvas.height / 2
                    
                    for (var i = 0; i < enhancedTopologyView.nodes.length; i++) {
                        var node = enhancedTopologyView.nodes[i]
                        var distance = Math.sqrt(Math.pow(clickedX - node.x, 2) + Math.pow(clickedY - node.y, 2))
                        if (distance < 30) {
                            // Node clicked
                            console.log("Node selected:", node.nodeId)
                            break
                        }
                    }
                }
            }
        }
        
        // Zoom controls
        Column {
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 10
            spacing: 5
            
            Button {
                text: "+"
                onClicked: {
                    enhancedTopologyView.zoomLevel = Math.min(enhancedTopologyView.zoomLevel + 0.1, 3.0)
                    canvas.requestPaint()
                }
            }
            
            Button {
                text: "-"
                onClicked: {
                    enhancedTopologyView.zoomLevel = Math.max(enhancedTopologyView.zoomLevel - 0.1, 0.5)
                    canvas.requestPaint()
                }
            }
            
            Button {
                text: "Reset"
                onClicked: {
                    enhancedTopologyView.zoomLevel = 1.0
                    enhancedTopologyView.panOffset = Qt.point(0, 0)
                    canvas.requestPaint()
                }
            }
        }
    }
    
    Connections {
        target: enhancedTopologyView
        function onNodesChanged() {
            canvas.requestPaint()
        }
        function onEdgesChanged() {
            canvas.requestPaint()
        }
    }
}

