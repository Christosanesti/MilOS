#ifndef SOCKET_INTERFACE_H
#define SOCKET_INTERFACE_H

#include <memory>
#include <string>

class ConfigParser;
class LogStorage;
class HashChain;
class EventCollector;

/**
 * @brief Socket Interface
 * 
 * Provides local socket interface for high-throughput event logging.
 * Used for bulk operations and high-volume event collection.
 */
class SocketInterface {
public:
    SocketInterface();
    ~SocketInterface();

    /**
     * @brief Initialize socket interface
     * @param configParser Configuration parser instance
     * @param logStorage Log storage instance
     * @param hashChain Hash chain instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(
        ConfigParser* configParser,
        LogStorage* logStorage,
        HashChain* hashChain
    );

    /**
     * @brief Start socket interface
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop socket interface
     */
    void stop();

    /**
     * @brief Check if socket interface is running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return m_running; }

private:
    bool m_running;
    bool m_initialized;
    
    ConfigParser* m_configParser;
    LogStorage* m_logStorage;
    HashChain* m_hashChain;
    
    std::string m_socketPath;
    int m_socketFd;

    /**
     * @brief Create and bind socket
     * @return true if successful, false otherwise
     */
    bool createSocket();

    /**
     * @brief Accept and handle connections
     */
    void acceptConnections();

    /**
     * @brief Handle client connection
     * @param clientFd Client socket file descriptor
     */
    void handleClient(int clientFd);
};

#endif // SOCKET_INTERFACE_H

