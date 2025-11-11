#ifndef SOCKET_INTERFACE_H
#define SOCKET_INTERFACE_H

#include "packet_capture.h"
#include <string>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>

/**
 * @brief Socket Interface
 * 
 * Provides high-throughput local socket interface for packet data transfer.
 * Uses Unix domain sockets for efficient data transfer (not D-Bus).
 */
class SocketInterface {
public:
    SocketInterface();
    ~SocketInterface();

    /**
     * @brief Initialize socket interface
     * @param socketPath Path to Unix domain socket
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const std::string& socketPath);

    /**
     * @brief Start socket server
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop socket server
     */
    void stop();

    /**
     * @brief Send packet data to connected clients
     * @param packet Packet data
     * @return true if send successful, false otherwise
     */
    bool sendPacket(const PacketData& packet);

    /**
     * @brief Check if socket is running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return m_running.load(); }

private:
    bool m_initialized;
    std::atomic<bool> m_running;
    std::string m_socketPath;
    int m_serverSocket;
    std::thread m_serverThread;
    std::mutex m_mutex;
    std::queue<PacketData> m_packetQueue;
    std::vector<int> m_clientSockets;

    /**
     * @brief Server thread function
     */
    void serverThreadFunction();

    /**
     * @brief Accept client connections
     */
    void acceptConnections();

    /**
     * @brief Send packet to all clients
     */
    void sendToClients(const PacketData& packet);
};

#endif // SOCKET_INTERFACE_H

