import QtQuick 2.15
import QtQuick.Controls 2.15
import org.kde.plasma.core 2.0 as PlasmaCore

/**
 * Bandwidth Monitor Component
 * 
 * Displays real-time bandwidth usage with animated graphs.
 */
Item {
    id: bandwidthMonitor
    property var networkMonitor: null
    property var bandwidthHistory: []

    Rectangle {
        anchors.fill: parent
        color: "#0a0a0a"
        border.color: "#00ffff"
        border.width: 1
        radius: 4

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 10

            // Title
            Text {
                text: "Bandwidth Usage"
                color: "#00ffff"
                font.pixelSize: 14
                font.bold: true
            }

            // Current bandwidth
            Text {
                text: networkMonitor ? networkMonitor.currentBandwidth.toFixed(2) + " MB/s" : "0.00 MB/s"
                color: "#00ffff"
                font.pixelSize: 24
                font.bold: true
            }

            // Bandwidth graph
            Canvas {
                id: bandwidthCanvas
                Layout.fillWidth: true
                Layout.fillHeight: true

                property int maxHistorySize: 100

                onPaint: {
                    var ctx = getContext("2d");
                    ctx.clearRect(0, 0, width, height);

                    if (bandwidthHistory.length < 2) {
                        return;
                    }

                    // Draw grid
                    ctx.strokeStyle = "#333333";
                    ctx.lineWidth = 1;
                    for (var i = 0; i <= 5; i++) {
                        var y = (height / 5) * i;
                        ctx.beginPath();
                        ctx.moveTo(0, y);
                        ctx.lineTo(width, y);
                        ctx.stroke();
                    }

                    // Draw bandwidth line with xenon glow
                    ctx.shadowBlur = 10;
                    ctx.shadowColor = "#00ffff";
                    ctx.strokeStyle = "#00ffff";
                    ctx.lineWidth = 2;
                    ctx.beginPath();

                    var maxBandwidth = Math.max.apply(null, bandwidthHistory);
                    if (maxBandwidth === 0) maxBandwidth = 1;

                    for (var i = 0; i < bandwidthHistory.length; i++) {
                        var x = (width / (bandwidthHistory.length - 1)) * i;
                        var y = height - (bandwidthHistory[i] / maxBandwidth) * height;
                        
                        if (i === 0) {
                            ctx.moveTo(x, y);
                        } else {
                            ctx.lineTo(x, y);
                        }
                    }

                    ctx.stroke();

                    // Fill area under curve
                    ctx.fillStyle = "#00ffff";
                    ctx.globalAlpha = 0.2;
                    ctx.lineTo(width, height);
                    ctx.lineTo(0, height);
                    ctx.closePath();
                    ctx.fill();
                    ctx.globalAlpha = 1.0;
                }

                Connections {
                    target: networkMonitor
                    function onBandwidthUpdated() {
                        if (networkMonitor) {
                            bandwidthHistory.push(networkMonitor.currentBandwidth);
                            if (bandwidthHistory.length > bandwidthCanvas.maxHistorySize) {
                                bandwidthHistory.shift();
                            }
                            bandwidthCanvas.requestPaint();
                        }
                    }
                }
            }
        }
    }
}

