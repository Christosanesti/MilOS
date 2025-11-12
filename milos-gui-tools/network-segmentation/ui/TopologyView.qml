import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MilosComponents 1.0

Rectangle {
    id: topologyView
    color: "#0a0a0a"
    
    property var nodes: []
    property var edges: []
    
    Canvas {
        id: canvas
        anchors.fill: parent
        
        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            
            // Draw edges
            ctx.strokeStyle = "#666666"
            ctx.lineWidth = 2
            for (var i = 0; i < topologyView.edges.length; i++) {
                var edge = topologyView.edges[i]
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
            for (var j = 0; j < topologyView.nodes.length; j++) {
                var node = topologyView.nodes[j]
                var x = node.x + canvas.width / 2
                var y = node.y + canvas.height / 2
                
                // Draw node circle
                ctx.fillStyle = "#00ff88"
                ctx.beginPath()
                ctx.arc(x, y, 30, 0, 2 * Math.PI)
                ctx.fill()
                
                // Draw node label
                ctx.fillStyle = "#ffffff"
                ctx.font = "12px Arial"
                ctx.textAlign = "center"
                ctx.fillText(node.label || node.nodeId, x, y + 50)
            }
        }
        
        function getNodeById(nodeId) {
            for (var i = 0; i < topologyView.nodes.length; i++) {
                if (topologyView.nodes[i].nodeId === nodeId) {
                    return topologyView.nodes[i]
                }
            }
            return null
        }
        
        onNodesChanged: requestPaint()
        onEdgesChanged: requestPaint()
    }
    
    Connections {
        target: topologyView
        function onNodesChanged() {
            canvas.requestPaint()
        }
        function onEdgesChanged() {
            canvas.requestPaint()
        }
    }
}

