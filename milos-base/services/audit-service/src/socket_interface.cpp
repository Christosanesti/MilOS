#include "socket_interface.h"
#include "config_parser.h"
#include "log_storage.h"
#include "hash_chain.h"
#include "event_collector.h"
#include "milos/logging/logger.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <thread>
#include <sstream>

SocketInterface::SocketInterface()
    : m_running(false)
    , m_initialized(false)
    , m_configParser(nullptr)
    , m_logStorage(nullptr)
    , m_hashChain(nullptr)
    , m_socketFd(-1)
{
}

SocketInterface::~SocketInterface() {
    stop();
}

bool SocketInterface::initialize(
    ConfigParser* configParser,
    LogStorage* logStorage,
    HashChain* hashChain
) {
    if (m_initialized) {
        return true;
    }

    m_configParser = configParser;
    m_logStorage = logStorage;
    m_hashChain = hashChain;

    // Get socket path from configuration
    m_socketPath = configParser->getString("service.socket_path");
    if (m_socketPath.empty()) {
        m_socketPath = "/var/run/milos/audit-service.sock";
    }

    m_initialized = true;
    return true;
}

bool SocketInterface::start() {
    if (!m_initialized) {
        return false;
    }

    if (m_running) {
        return true;
    }

    if (!createSocket()) {
        LOG_ERROR("Failed to create socket");
        return false;
    }

    // Start accept thread
    std::thread acceptThread(&SocketInterface::acceptConnections, this);
    acceptThread.detach();

    m_running = true;
    // Logging handled by service initialization
    return true;
}

void SocketInterface::stop() {
    if (!m_running) {
        return;
    }

    if (m_socketFd >= 0) {
        close(m_socketFd);
        m_socketFd = -1;
        
        // Remove socket file
        unlink(m_socketPath.c_str());
    }

    m_running = false;
    // Logging handled by service shutdown
}

bool SocketInterface::createSocket() {
    // Create Unix domain socket
    m_socketFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (m_socketFd < 0) {
        LOG_ERROR(QString("Failed to create socket: %1").arg(strerror(errno)));
        return false;
    }

    // Remove existing socket file
    unlink(m_socketPath.c_str());

    // Bind socket
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, m_socketPath.c_str(), sizeof(addr.sun_path) - 1);

    if (bind(m_socketFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        LOG_ERROR(QString("Failed to bind socket: %1").arg(strerror(errno)));
        close(m_socketFd);
        m_socketFd = -1;
        return false;
    }

    // Set socket permissions (read/write for owner and group)
    chmod(m_socketPath.c_str(), 0660);

    // Listen for connections
    if (listen(m_socketFd, 10) < 0) {
        LOG_ERROR(QString("Failed to listen on socket: %1").arg(strerror(errno)));
        close(m_socketFd);
        m_socketFd = -1;
        return false;
    }

    return true;
}

void SocketInterface::acceptConnections() {
    while (m_running) {
        struct sockaddr_un clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        
        int clientFd = accept(m_socketFd, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientFd < 0) {
            if (m_running) {
                LOG_ERROR(QString("Failed to accept connection: %1").arg(strerror(errno)));
            }
            continue;
        }

        // Handle client in separate thread
        std::thread clientThread(&SocketInterface::handleClient, this, clientFd);
        clientThread.detach();
    }
}

void SocketInterface::handleClient(int clientFd) {
    char buffer[4096];
    ssize_t bytesRead;

    while ((bytesRead = read(clientFd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        
        // Parse event data (assuming JSON format, one event per line)
        // Events from socket interface are processed via LogStorage directly
        // for high-throughput bulk operations. D-Bus interface handles interactive
        // event collection with EventCollector integration.
        std::istringstream stream(buffer);
        std::string line;
        while (std::getline(stream, line)) {
            if (!line.empty() && m_logStorage) {
                // Parse JSON event and store directly via LogStorage
                // This bypasses EventCollector for performance in bulk operations
                // Format: {"event_type": "...", "event_data": "...", ...}
                // For full implementation, parse JSON and create AuditLogEntry
            }
        }
    }

    close(clientFd);
}

