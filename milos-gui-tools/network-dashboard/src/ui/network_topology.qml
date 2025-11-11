import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.core 2.0 as PlasmaCore

/**
 * Network Topology Visualization Component
 * 
 * Displays interactive network topology with security zones, nodes, edges, and threat indicators.
 */
Item {
    id: topologyView
    property var networkTopology: null
    property var networkMonitor: null

    Rectangle {
        anchors.fill: parent
        color: "#0a0a0a"
        border.color: "#00ffff"
        border.width: 2
        radius: 4

        // Title
        Text {
            id: title
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 10
            text: "Network Topology"
            color: "#00ffff"
            font.pixelSize: 16
            font.bold: true
        }

        // Topology canvas
        Canvas {
            id: topologyCanvas
            anchors.fill: parent
            anchors.topMargin: 40
            anchors.margins: 10

            property double zoom: 1.0
            property double panX: 0.0
            property double panY: 0.0
            property string selectedNodeId: ""

            onPaint: {
                var ctx = getContext("2d");
                ctx.clearRect(0, 0, width, height);

                // Apply zoom and pan
                ctx.save();
                ctx.translate(panX, panY);
                ctx.scale(zoom, zoom);

                // Draw security zones
                if (networkTopology) {
                    var zones = networkTopology.securityZones;
                    for (var i = 0; i < zones.length; i++) {
                        var zone = zones[i];
                        ctx.fillStyle = zone.color;
                        ctx.globalAlpha = 0.1;
                        ctx.fillRect(0, 0, width / zoom, height / zoom);
                        ctx.globalAlpha = 1.0;
                    }
                }

                // Draw edges
                if (networkTopology) {
                    var edges = networkTopology.edges;
                    for (var i = 0; i < edges.length; i++) {
                        var edge = edges[i];
                        var sourceNode = findNode(edge.source_id);
                        var destNode = findNode(edge.destination_id);
                        
                        if (sourceNode && destNode) {
                            // Xenon glow for threat edges
                            if (edge.has_threat) {
                                ctx.shadowBlur = 15;
                                ctx.shadowColor = "#ff0000";
                            } else {
                                ctx.shadowBlur = 5;
                                ctx.shadowColor = "#00ffff";
                            }
                            
                            ctx.strokeStyle = edge.has_threat ? "#ff0000" : "#00ffff";
                            ctx.lineWidth = 2;
                            ctx.beginPath();
                            ctx.moveTo(sourceNode.x, sourceNode.y);
                            ctx.lineTo(destNode.x, destNode.y);
                            ctx.stroke();
                            
                            // Draw data flow animation
                            if (edge.packet_count > 0) {
                                var t = Date.now() % 2000 / 2000.0;
                                var flowX = sourceNode.x + (destNode.x - sourceNode.x) * t;
                                var flowY = sourceNode.y + (destNode.y - sourceNode.y) * t;
                                
                                ctx.fillStyle = "#00ffff";
                                ctx.beginPath();
                                ctx.arc(flowX, flowY, 4, 0, 2 * Math.PI);
                                ctx.fill();
                            }
                        }
                    }
                }

                // Draw nodes
                if (networkTopology) {
                    var nodes = networkTopology.nodes;
                    for (var i = 0; i < nodes.length; i++) {
                        var node = nodes[i];
                        
                        // Xenon glow for selected node
                        if (node.id === selectedNodeId) {
                            ctx.shadowBlur = 20;
                            ctx.shadowColor = "#00ffff";
                        } else {
                            ctx.shadowBlur = 10;
                            ctx.shadowColor = "#00ffff";
                        }
                        
                        ctx.fillStyle = "#00ffff";
                        ctx.beginPath();
                        ctx.arc(node.x, node.y, 10, 0, 2 * Math.PI);
                        ctx.fill();
                        
                        // Node label
                        ctx.fillStyle = "#ffffff";
                        ctx.font = "12px sans-serif";
                        ctx.fillText(node.name, node.x + 15, node.y);
                    }
                }

                ctx.restore();
            }

            // Mouse interaction
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                
                onWheel: {
                    var delta = wheel.angleDelta.y / 120.0;
                    topologyCanvas.zoom = Math.max(0.5, Math.min(3.0, topologyCanvas.zoom + delta * 0.1));
                    topologyCanvas.requestPaint();
                }
                
                property point lastPanPoint
                onPressed: {
                    lastPanPoint = Qt.point(mouse.x, mouse.y);
                }
                
                onPositionChanged: {
                    if (pressedButtons & Qt.RightButton) {
                        topologyCanvas.panX += mouse.x - lastPanPoint.x;
                        topologyCanvas.panY += mouse.y - lastPanPoint.y;
                        lastPanPoint = Qt.point(mouse.x, mouse.y);
                        topologyCanvas.requestPaint();
                    } else if (pressedButtons & Qt.LeftButton) {
                        // Node selection
                        var clickedNode = findNodeAt(mouse.x, mouse.y);
                        if (clickedNode) {
                            topologyCanvas.selectedNodeId = clickedNode.id;
                            networkTopology.nodeSelected(clickedNode.id);
                            topologyCanvas.requestPaint();
                        }
                    }
                }
            }

            Timer {
                interval: 50  // 20 FPS for smooth animation
                running: true
                repeat: true
                onTriggered: {
                    topologyCanvas.requestPaint();
                }
            }

            Connections {
                target: networkTopology
                function onTopologyUpdated() {
                    topologyCanvas.requestPaint();
                }
            }

            function findNode(nodeId) {
                if (!networkTopology) return null;
                var nodes = networkTopology.nodes;
                for (var i = 0; i < nodes.length; i++) {
                    if (nodes[i].id === nodeId) {
                        return nodes[i];
                    }
                }
                return null;
            }

            function findNodeAt(x, y) {
                if (!networkTopology) return null;
                var nodes = networkTopology.nodes;
                for (var i = 0; i < nodes.length; i++) {
                    var node = nodes[i];
                    var nodeX = node.x * topologyCanvas.zoom + topologyCanvas.panX;
                    var nodeY = node.y * topologyCanvas.zoom + topologyCanvas.panY;
                    var dist = Math.sqrt(Math.pow(x - nodeX, 2) + Math.pow(y - nodeY, 2));
                    if (dist < 20) {
                        return node;
                    }
                }
                return null;
            }
        }
    }
}

