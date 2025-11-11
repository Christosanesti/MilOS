#include "network_monitor.h"
#include "packet_capture.h"
#include "packet_statistics.h"
#include <QTimer>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <algorithm>
#include <iostream>

NetworkMonitor::NetworkMonitor(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_running(false)
    , m_packetCapture(nullptr)
    , m_packetStatistics(nullptr)
    , m_statisticsTimer(nullptr)
    , m_bandwidthTimer(nullptr)
    , m_totalPackets(0)
    , m_totalBytes(0)
    , m_currentBandwidth(0.0)
    , m_lastUpdateTime(0)
    , m_lastByteCount(0)
{
}

NetworkMonitor::~NetworkMonitor() {
    stop();
}

bool NetworkMonitor::initialize(PacketCapture* packetCapture, PacketStatistics* packetStatistics) {
    if (m_initialized) {
        return true;
    }

    m_packetCapture = packetCapture;
    m_packetStatistics = packetStatistics;

    // Create timers for periodic updates
    m_statisticsTimer = new QTimer(this);
    connect(m_statisticsTimer, &QTimer::timeout, this, &NetworkMonitor::updateStatistics);
    m_statisticsTimer->setInterval(1000);  // Update every second

    m_bandwidthTimer = new QTimer(this);
    connect(m_bandwidthTimer, &QTimer::timeout, this, &NetworkMonitor::updateBandwidth);
    m_bandwidthTimer->setInterval(1000);  // Update every second

    m_initialized = true;
    return true;
}

bool NetworkMonitor::start() {
    if (!m_initialized) {
        return false;
    }

    if (m_running) {
        return true;
    }

    // Start timers
    m_statisticsTimer->start();
    m_bandwidthTimer->start();

    // Setup packet capture callback
    if (m_packetCapture) {
        PacketCaptureCallback callback = [this](const PacketData& packet) {
            processPacket(packet);
        };
        // Note: Capture will be started via D-Bus interface
    }

    m_running = true;
    emit networkStatusChanged("monitoring");
    return true;
}

void NetworkMonitor::stop() {
    if (!m_running) {
        return;
    }

    if (m_statisticsTimer) {
        m_statisticsTimer->stop();
    }

    if (m_bandwidthTimer) {
        m_bandwidthTimer->stop();
    }

    m_running = false;
    emit networkStatusChanged("stopped");
}

QVariantList NetworkMonitor::activeConnections() const {
    QVariantList connections;
    
    for (const auto& pair : m_connections) {
        const ConnectionInfo& conn = pair.second;
        if (conn.is_active) {
            QJsonObject connObj;
            connObj["id"] = pair.first;
            connObj["source"] = conn.source_address;
            connObj["destination"] = conn.destination_address;
            connObj["source_port"] = conn.source_port;
            connObj["destination_port"] = conn.destination_port;
            connObj["protocol"] = conn.protocol;
            connObj["packet_count"] = static_cast<qint64>(conn.packet_count);
            connObj["byte_count"] = static_cast<qint64>(conn.byte_count);
            connObj["start_time"] = static_cast<qint64>(conn.start_time);
            connObj["last_seen"] = static_cast<qint64>(conn.last_seen);
            
            connections.append(QJsonValue(connObj).toVariant());
        }
    }
    
    return connections;
}

QJsonObject NetworkMonitor::getConnectionDetails(const QString& connectionId) const {
    auto it = m_connections.find(connectionId);
    if (it == m_connections.end()) {
        return QJsonObject();
    }

    const ConnectionInfo& conn = it->second;
    QJsonObject details;
    details["id"] = connectionId;
    details["source"] = conn.source_address;
    details["destination"] = conn.destination_address;
    details["source_port"] = conn.source_port;
    details["destination_port"] = conn.destination_port;
    details["protocol"] = conn.protocol;
    details["packet_count"] = static_cast<qint64>(conn.packet_count);
    details["byte_count"] = static_cast<qint64>(conn.byte_count);
    details["start_time"] = static_cast<qint64>(conn.start_time);
    details["last_seen"] = static_cast<qint64>(conn.last_seen);
    details["is_active"] = conn.is_active;

    return details;
}

QJsonArray NetworkMonitor::getThreats() const {
    // Placeholder for threat information
    // Will be populated by IDS integration (Story 18.3)
    QJsonArray threats;
    return threats;
}

void NetworkMonitor::updateStatistics() {
    if (!m_packetStatistics) {
        return;
    }

    m_totalPackets = m_packetStatistics->getTotalPackets();
    m_totalBytes = m_packetStatistics->getTotalBytes();

    // Update protocol statistics
    auto protocolStats = m_packetStatistics->getProtocolStatistics();
    m_protocolStats.clear();
    
    for (const auto& pair : protocolStats) {
        QJsonObject stats;
        stats["packet_count"] = static_cast<qint64>(pair.second.packet_count);
        stats["byte_count"] = static_cast<qint64>(pair.second.byte_count);
        stats["percentage_packets"] = pair.second.percentage_packets;
        stats["percentage_bytes"] = pair.second.percentage_bytes;
        
        m_protocolStats[QString::fromStdString(pair.first)] = QJsonValue(stats).toVariant();
    }

    emit statisticsUpdated();
}

void NetworkMonitor::updateBandwidth() {
    if (!m_packetStatistics) {
        return;
    }

    uint64_t currentByteCount = m_packetStatistics->getTotalBytes();
    uint64_t currentTime = QDateTime::currentMSecsSinceEpoch();

    if (m_lastUpdateTime > 0) {
        uint64_t timeDelta = currentTime - m_lastUpdateTime;
        uint64_t byteDelta = currentByteCount - m_lastByteCount;

        if (timeDelta > 0) {
            // Calculate bandwidth in MB/s
            double bytesPerSecond = (static_cast<double>(byteDelta) / timeDelta) * 1000.0;
            m_currentBandwidth = bytesPerSecond / (1024.0 * 1024.0);  // Convert to MB/s
        }
    }

    m_lastUpdateTime = currentTime;
    m_lastByteCount = currentByteCount;

    emit bandwidthUpdated();
}

void NetworkMonitor::processPacket(const PacketData& packet) {
    updateConnectionTracking(packet);
}

QString NetworkMonitor::generateConnectionId(const PacketData& packet) const {
    // Generate unique connection ID from packet information
    QString id = QString("%1:%2-%3:%4-%5")
        .arg(packet.source_address.c_str())
        .arg(packet.source_port)
        .arg(packet.destination_address.c_str())
        .arg(packet.destination_port)
        .arg(packet.protocol.c_str());
    return id;
}

void NetworkMonitor::updateConnectionTracking(const PacketData& packet) {
    QString connectionId = generateConnectionId(packet);
    
    auto it = m_connections.find(connectionId);
    if (it == m_connections.end()) {
        // New connection
        ConnectionInfo conn;
        conn.source_address = QString::fromStdString(packet.source_address);
        conn.destination_address = QString::fromStdString(packet.destination_address);
        conn.source_port = packet.source_port;
        conn.destination_port = packet.destination_port;
        conn.protocol = QString::fromStdString(packet.protocol);
        conn.packet_count = 1;
        conn.byte_count = packet.length;
        conn.start_time = packet.timestamp;
        conn.last_seen = packet.timestamp;
        conn.is_active = true;
        
        m_connections[connectionId] = conn;
    } else {
        // Update existing connection
        ConnectionInfo& conn = it->second;
        conn.packet_count++;
        conn.byte_count += packet.length;
        conn.last_seen = packet.timestamp;
        conn.is_active = true;
    }

    emit connectionsUpdated();
}

void NetworkMonitor::cleanupInactiveConnections() {
    uint64_t currentTime = QDateTime::currentMSecsSinceEpoch();
    uint64_t timeout = 60000;  // 60 seconds timeout

    auto it = m_connections.begin();
    while (it != m_connections.end()) {
        if (currentTime - it->second.last_seen > timeout) {
            it->second.is_active = false;
            // Optionally remove: it = m_connections.erase(it);
            ++it;
        } else {
            ++it;
        }
    }
}

