import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.core 2.0 as PlasmaCore

/**
 * Traffic Visualization Component
 * 
 * Displays animated network traffic flows with xenon-accented visualizations.
 */
Item {
    id: trafficViz
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
            text: "Real-Time Traffic Flow"
            color: "#00ffff"
            font.pixelSize: 16
            font.bold: true
        }

        // Traffic flow canvas
        Canvas {
            id: trafficCanvas
            anchors.fill: parent
            anchors.topMargin: 40
            anchors.margins: 10

            property var trafficFlows: []

            onPaint: {
                var ctx = getContext("2d");
                ctx.clearRect(0, 0, width, height);

                // Draw traffic flows with xenon glow effect
                for (var i = 0; i < trafficFlows.length; i++) {
                    var flow = trafficFlows[i];
                    
                    // Xenon glow effect
                    ctx.shadowBlur = 15;
                    ctx.shadowColor = "#00ffff";
                    
                    // Draw flow line
                    ctx.strokeStyle = "#00ffff";
                    ctx.lineWidth = 2;
                    ctx.beginPath();
                    ctx.moveTo(flow.x1, flow.y1);
                    ctx.lineTo(flow.x2, flow.y2);
                    ctx.stroke();
                    
                    // Draw flow particle
                    ctx.fillStyle = "#00ffff";
                    ctx.beginPath();
                    ctx.arc(flow.particleX, flow.particleY, 4, 0, 2 * Math.PI);
                    ctx.fill();
                }
            }

            Timer {
                interval: 50  // 20 FPS for smooth animation
                running: true
                repeat: true
                onTriggered: {
                    updateTrafficFlows();
                    trafficCanvas.requestPaint();
                }
            }

            function updateTrafficFlows() {
                // Generate traffic flows based on network activity
                // In production, this would use actual packet data
                trafficFlows = [];
                
                if (networkMonitor && networkMonitor.totalPackets > 0) {
                    // Generate sample flows
                    for (var i = 0; i < 10; i++) {
                        var flow = {
                            x1: Math.random() * width,
                            y1: Math.random() * height,
                            x2: Math.random() * width,
                            y2: Math.random() * height,
                            particleX: Math.random() * width,
                            particleY: Math.random() * height
                        };
                        trafficFlows.push(flow);
                    }
                }
            }
        }

        // Statistics overlay
        Rectangle {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10
            height: 60
            color: "#1a1a1a"
            opacity: 0.9
            radius: 4

            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 20

                Text {
                    text: "Packets: " + (networkMonitor ? networkMonitor.totalPackets : 0)
                    color: "#ffffff"
                    font.pixelSize: 12
                }

                Text {
                    text: "Bytes: " + (networkMonitor ? formatBytes(networkMonitor.totalBytes) : "0 B")
                    color: "#ffffff"
                    font.pixelSize: 12
                }

                Text {
                    text: "Bandwidth: " + (networkMonitor ? networkMonitor.currentBandwidth.toFixed(2) + " MB/s" : "0 MB/s")
                    color: "#00ffff"
                    font.pixelSize: 12
                    font.bold: true
                }
            }
        }
    }

    function formatBytes(bytes) {
        if (bytes < 1024) return bytes + " B";
        if (bytes < 1024 * 1024) return (bytes / 1024).toFixed(2) + " KB";
        if (bytes < 1024 * 1024 * 1024) return (bytes / (1024 * 1024)).toFixed(2) + " MB";
        return (bytes / (1024 * 1024 * 1024)).toFixed(2) + " GB";
    }
}

