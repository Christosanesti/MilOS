#include "network_dashboard.h"
#include "packet_capture.h"
#include "packet_parser.h"
#include "packet_statistics.h"
#include "packet_sanitizer.h"
#include "socket_interface.h"
#include "dbus_interface.h"
#include "config_parser.h"
#include "network_monitor.h"
#include <iostream>

NetworkDashboard::NetworkDashboard(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_running(false)
{
}

NetworkDashboard::~NetworkDashboard() {
    stop();
}

bool NetworkDashboard::initialize() {
    if (m_initialized) {
        return true;
    }

    // Load configuration
    m_configParser = std::make_unique<ConfigParser>();
    if (!m_configParser->load("/etc/milos/network-dashboard/config.yaml")) {
        std::cerr << "Failed to load configuration, using defaults" << std::endl;
    }

    if (!initializeComponents()) {
        std::cerr << "Failed to initialize components" << std::endl;
        return false;
    }

    m_initialized = true;
    return true;
}

bool NetworkDashboard::initializeComponents() {
    // Initialize packet capture
    m_packetCapture = std::make_unique<PacketCapture>();
    std::string interface = m_configParser->getString("packet_capture.default_interface");
    if (!m_packetCapture->initialize(interface)) {
        std::cerr << "Failed to initialize packet capture" << std::endl;
        return false;
    }

    // Initialize packet parser
    m_packetParser = std::make_unique<PacketParser>();
    if (!m_packetParser->initialize()) {
        std::cerr << "Failed to initialize packet parser" << std::endl;
        return false;
    }

    // Initialize packet statistics
    m_packetStatistics = std::make_unique<PacketStatistics>();
    m_packetStatistics->initialize();

    // Initialize packet sanitizer
    m_packetSanitizer = std::make_unique<PacketSanitizer>();
    if (!m_packetSanitizer->initialize()) {
        std::cerr << "Failed to initialize packet sanitizer" << std::endl;
        return false;
    }

    // Initialize socket interface
    m_socketInterface = std::make_unique<SocketInterface>();
    std::string socketPath = m_configParser->getString("integration.socket.socket_path");
    if (socketPath.empty()) {
        socketPath = "/var/run/milos/network-dashboard.sock";
    }
    if (!m_socketInterface->initialize(socketPath)) {
        std::cerr << "Failed to initialize socket interface" << std::endl;
        return false;
    }

    // Initialize Network Monitor
    m_networkMonitor = std::make_unique<NetworkMonitor>();
    if (!m_networkMonitor->initialize(m_packetCapture.get(), m_packetStatistics.get())) {
        std::cerr << "Failed to initialize Network Monitor" << std::endl;
        return false;
    }

    // Initialize D-Bus interface
    m_dbusInterface = std::make_unique<DBusInterface>();
    if (!m_dbusInterface->initialize(m_packetCapture.get(), m_packetStatistics.get())) {
        std::cerr << "Failed to initialize D-Bus interface" << std::endl;
        return false;
    }

    // Setup packet capture callback
    setupPacketCaptureCallback();

    return true;
}

void NetworkDashboard::setupPacketCaptureCallback() {
    PacketCaptureCallback callback = [this](const PacketData& packet) {
        // Parse packet
        PacketData parsedPacket = m_packetParser->parsePacket(packet);

        // Update statistics
        m_packetStatistics->updateStatistics(parsedPacket);

        // Sanitize packet
        PacketData sanitizedPacket = m_packetSanitizer->sanitizePacket(parsedPacket);

        // Send via socket interface (high-throughput)
        if (m_socketInterface->isRunning()) {
            m_socketInterface->sendPacket(sanitizedPacket);
        }

        // TODO: Log to audit service via D-Bus
        // TODO: Check with Data Transmission Guard via D-Bus
    };

    // Store callback for later use when capture starts
    // For now, callback will be set when StartCapture is called
}

bool NetworkDashboard::start() {
    if (!m_initialized) {
        if (!initialize()) {
            return false;
        }
    }

    if (m_running) {
        return true;
    }

    // Start socket interface
    if (m_configParser->getBool("integration.socket.enabled", true)) {
        if (!m_socketInterface->start()) {
            std::cerr << "Failed to start socket interface" << std::endl;
            return false;
        }
    }

    // Start D-Bus interface
    if (m_configParser->getBool("dbus.enabled", true)) {
        if (!m_dbusInterface->start()) {
            std::cerr << "Failed to start D-Bus interface" << std::endl;
            return false;
        }
    }

    m_running = true;
    std::cout << "Network Dashboard started" << std::endl;
    return true;
}

void NetworkDashboard::stop() {
    if (!m_running) {
        return;
    }

    // Stop packet capture
    if (m_packetCapture) {
        m_packetCapture->stopCapture();
    }

    // Stop socket interface
    if (m_socketInterface) {
        m_socketInterface->stop();
    }

    // Stop Network Monitor
    if (m_networkMonitor) {
        m_networkMonitor->stop();
    }

    // Stop D-Bus interface
    if (m_dbusInterface) {
        m_dbusInterface->stop();
    }

    m_running = false;
    std::cout << "Network Dashboard stopped" << std::endl;
}

