#include "dbus_interface.h"
#include "packet_capture.h"
#include "packet_statistics.h"
#include <QDBusConnection>
#include <QDBusError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>

DBusInterface::DBusInterface(QObject* parent)
    : QObject(parent)
    , m_running(false)
    , m_initialized(false)
    , m_packetCapture(nullptr)
    , m_packetStatistics(nullptr)
{
}

DBusInterface::~DBusInterface() {
    stop();
}

bool DBusInterface::initialize(PacketCapture* packetCapture, PacketStatistics* packetStatistics) {
    if (m_initialized) {
        return true;
    }

    m_packetCapture = packetCapture;
    m_packetStatistics = packetStatistics;

    m_initialized = true;
    return true;
}

bool DBusInterface::start() {
    if (!m_initialized) {
        std::cerr << "D-Bus interface not initialized" << std::endl;
        return false;
    }

    if (m_running) {
        return true;
    }

    if (!registerInterface()) {
        std::cerr << "Failed to register D-Bus interface" << std::endl;
        return false;
    }

    m_running = true;
    std::cout << "D-Bus interface started" << std::endl;
    return true;
}

void DBusInterface::stop() {
    if (!m_running) {
        return;
    }

    unregisterInterface();
    m_running = false;
    std::cout << "D-Bus interface stopped" << std::endl;
}

bool DBusInterface::StartCapture(const QString& interface, const QString& filter) {
    if (!m_packetCapture) {
        std::cerr << "Packet capture not available" << std::endl;
        return false;
    }

    if (!m_packetCapture->isCapturing()) {
        // Initialize if needed
        if (!m_packetCapture->initialize(interface.toStdString())) {
            std::cerr << "Failed to initialize packet capture" << std::endl;
            return false;
        }

        // Start capture with callback to emit signal
        PacketCaptureCallback callback = [this](const PacketData& packet) {
            // Convert packet to JSON
            QJsonObject packetObj;
            packetObj["timestamp"] = static_cast<qint64>(packet.timestamp);
            packetObj["interface"] = QString::fromStdString(packet.interface);
            packetObj["protocol"] = QString::fromStdString(packet.protocol);
            packetObj["length"] = static_cast<int>(packet.length);
            packetObj["source"] = QString::fromStdString(packet.source_address);
            packetObj["destination"] = QString::fromStdString(packet.destination_address);
            if (packet.source_port > 0) {
                packetObj["source_port"] = packet.source_port;
            }
            if (packet.destination_port > 0) {
                packetObj["destination_port"] = packet.destination_port;
            }

            QJsonDocument doc(packetObj);
            emit PacketCaptured(QString::fromUtf8(doc.toJson()));
        };

        if (!m_packetCapture->startCapture(filter.toStdString(), callback)) {
            std::cerr << "Failed to start packet capture" << std::endl;
            return false;
        }
    }

    return true;
}

bool DBusInterface::StopCapture() {
    if (!m_packetCapture) {
        return false;
    }

    return m_packetCapture->stopCapture();
}

QString DBusInterface::GetPacketStats() {
    if (!m_packetStatistics) {
        return QString("{\"error\":\"Statistics not available\"}");
    }

    uint64_t totalPackets = m_packetStatistics->getTotalPackets();
    uint64_t totalBytes = m_packetStatistics->getTotalBytes();
    auto protocolStats = m_packetStatistics->getProtocolStatistics();

    QJsonObject statsObj;
    statsObj["total_packets"] = static_cast<qint64>(totalPackets);
    statsObj["total_bytes"] = static_cast<qint64>(totalBytes);

    QJsonObject protocolObj;
    for (const auto& pair : protocolStats) {
        QJsonObject protoStats;
        protoStats["packet_count"] = static_cast<qint64>(pair.second.packet_count);
        protoStats["byte_count"] = static_cast<qint64>(pair.second.byte_count);
        protoStats["percentage_packets"] = pair.second.percentage_packets;
        protoStats["percentage_bytes"] = pair.second.percentage_bytes;
        protocolObj[QString::fromStdString(pair.first)] = protoStats;
    }
    statsObj["protocols"] = protocolObj;

    QJsonDocument doc(statsObj);
    return QString::fromUtf8(doc.toJson());
}

QString DBusInterface::GetThreats() {
    // Placeholder for threat information
    // Will be populated by IDS integration (Story 18.3)
    QJsonArray threats;
    QJsonDocument doc(threats);
    return QString::fromUtf8(doc.toJson());
}

bool DBusInterface::registerInterface() {
    QDBusConnection connection = QDBusConnection::systemBus();

    if (!connection.isConnected()) {
        std::cerr << "Cannot connect to D-Bus system bus: "
                  << connection.lastError().message().toStdString() << std::endl;
        return false;
    }

    QString objectPath = "/org/milos/NetworkDashboard";
    if (!connection.registerObject(objectPath, this)) {
        std::cerr << "Failed to register D-Bus object: "
                  << connection.lastError().message().toStdString() << std::endl;
        return false;
    }

    QString serviceName = "org.milos.NetworkDashboard";
    if (!connection.registerService(serviceName)) {
        std::cerr << "Failed to register D-Bus service: "
                  << connection.lastError().message().toStdString() << std::endl;
        connection.unregisterObject(objectPath);
        return false;
    }

    std::cout << "D-Bus interface registered: " << serviceName.toStdString()
              << " at " << objectPath.toStdString() << std::endl;
    return true;
}

void DBusInterface::unregisterInterface() {
    QDBusConnection connection = QDBusConnection::systemBus();

    QString objectPath = "/org/milos/NetworkDashboard";
    QString serviceName = "org.milos.NetworkDashboard";

    connection.unregisterObject(objectPath);
    connection.unregisterService(serviceName);
}

