#include "socket_interface.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>

SocketInterface::SocketInterface()
    : m_initialized(false)
    , m_running(false)
    , m_serverSocket(-1)
{
}

SocketInterface::~SocketInterface() {
    stop();
}

bool SocketInterface::initialize(const std::string& socketPath) {
    if (m_initialized) {
        return true;
    }

    m_socketPath = socketPath;
    m_initialized = true;
    return true;
}

bool SocketInterface::start() {
    if (!m_initialized) {
        return false;
    }

    if (m_running.load()) {
        return true;
    }

    // Create Unix domain socket
    m_serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (m_serverSocket < 0) {
        std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
        return false;
    }

    // Set non-blocking
    int flags = fcntl(m_serverSocket, F_GETFL, 0);
    fcntl(m_serverSocket, F_SETFL, flags | O_NONBLOCK);

    // Bind socket
    struct sockaddr_un addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, m_socketPath.c_str(), sizeof(addr.sun_path) - 1);

    // Remove existing socket file
    unlink(m_socketPath.c_str());

    if (bind(m_serverSocket, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "Failed to bind socket: " << strerror(errno) << std::endl;
        close(m_serverSocket);
        m_serverSocket = -1;
        return false;
    }

    // Listen
    if (listen(m_serverSocket, 10) < 0) {
        std::cerr << "Failed to listen on socket: " << strerror(errno) << std::endl;
        close(m_serverSocket);
        m_serverSocket = -1;
        return false;
    }

    m_running = true;
    m_serverThread = std::thread(&SocketInterface::serverThreadFunction, this);

    return true;
}

void SocketInterface::stop() {
    if (!m_running.load()) {
        return;
    }

    m_running = false;

    // Close server socket
    if (m_serverSocket >= 0) {
        close(m_serverSocket);
        m_serverSocket = -1;
    }

    // Close client sockets
    std::lock_guard<std::mutex> lock(m_mutex);
    for (int clientSocket : m_clientSockets) {
        close(clientSocket);
    }
    m_clientSockets.clear();

    // Wait for server thread
    if (m_serverThread.joinable()) {
        m_serverThread.join();
    }

    // Remove socket file
    unlink(m_socketPath.c_str());
}

bool SocketInterface::sendPacket(const PacketData& packet) {
    if (!m_running.load()) {
        return false;
    }

    sendToClients(packet);
    return true;
}

void SocketInterface::serverThreadFunction() {
    while (m_running.load()) {
        acceptConnections();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void SocketInterface::acceptConnections() {
    if (m_serverSocket < 0) {
        return;
    }

    struct sockaddr_un clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientSocket = accept(m_serverSocket, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientLen);

    if (clientSocket >= 0) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_clientSockets.push_back(clientSocket);
    }
}

void SocketInterface::sendToClients(const PacketData& packet) {
    std::lock_guard<std::mutex> lock(m_mutex);

    // Serialize packet data (simplified)
    // In production, use proper serialization (protobuf, msgpack, etc.)
    std::vector<uint8_t> buffer;
    buffer.insert(buffer.end(), packet.data.begin(), packet.data.end());

    // Send to all connected clients
    auto it = m_clientSockets.begin();
    while (it != m_clientSockets.end()) {
        int clientSocket = *it;
        ssize_t sent = send(clientSocket, buffer.data(), buffer.size(), MSG_NOSIGNAL);
        
        if (sent < 0) {
            // Client disconnected
            close(clientSocket);
            it = m_clientSockets.erase(it);
        } else {
            ++it;
        }
    }
}

