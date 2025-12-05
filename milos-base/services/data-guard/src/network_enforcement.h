#ifndef NETWORK_ENFORCEMENT_H
#define NETWORK_ENFORCEMENT_H

#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <QString>

class ConfigParser;
class PolicyManager;
class AuditLogger;

/**
 * @brief Network layer enforcement component
 * 
 * Handles kernel-level network hooks, packet inspection,
 * encryption detection, and packet blocking.
 */
class NetworkEnforcement {
public:
    NetworkEnforcement();
    ~NetworkEnforcement();

    /**
     * @brief Initialize network enforcement
     * @param configParser Configuration parser instance
     * @param policyManager Policy manager instance
     * @param auditLogger Audit logger instance (optional)
     * @return true if initialization successful, false otherwise
     */
    bool initialize(ConfigParser* configParser, PolicyManager* policyManager, class AuditLogger* auditLogger = nullptr);

    /**
     * @brief Start network enforcement
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop network enforcement
     */
    void stop();

    /**
     * @brief Check if network enforcement is running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return m_running; }

    /**
     * @brief Check if network enforcement is healthy
     * @return true if healthy, false otherwise
     */
    bool isHealthy() const;

    /**
     * @brief Get number of blocked transmissions
     * @return Number of blocked transmissions
     */
    size_t getBlockedCount() const { return m_blockedCount; }

    /**
     * @brief Get number of allowed transmissions
     * @return Number of allowed transmissions
     */
    size_t getAllowedCount() const { return m_allowedCount; }

    /**
     * @brief Get list of blocked transmissions (JSON strings)
     * @return List of blocked transmission information
     */
    std::vector<std::string> getBlockedTransmissions() const;

private:
    bool m_running;
    bool m_initialized;
    size_t m_blockedCount;
    size_t m_allowedCount;
    
    ConfigParser* m_configParser;
    PolicyManager* m_policyManager;
    AuditLogger* m_auditLogger;

    // libpcap handle for packet capture
    void* m_pcapHandle;  // pcap_t* handle
    
    // Packet capture thread
    bool m_captureRunning;
    void* m_captureThread;  // Thread handle for packet capture loop

    // Blocked transmission storage
    struct BlockedTransmission {
        std::string source;
        std::string destination;
        std::string protocol;
        int port;
        std::string timestamp;
        std::string reason;
    };
    mutable std::vector<BlockedTransmission> m_blockedTransmissions;
    mutable std::mutex m_blockedMutex;  // Mutex for thread-safe access to blocked transmissions
    static const size_t MAX_BLOCKED_HISTORY = 1000;  // Maximum number of blocked transmissions to store

    /**
     * @brief Initialize network hooks (libpcap)
     * @return true if successful, false otherwise
     */
    bool initializeNetworkHooks();

    /**
     * @brief Cleanup network hooks
     */
    void cleanupNetworkHooks();

    /**
     * @brief Packet capture thread function
     * @param arg Pointer to NetworkEnforcement instance
     * @return Thread return value
     */
    static void* packetCaptureThread(void* arg);

    /**
     * @brief Process captured packet
     * @param header Packet header
     * @param packet Packet data
     */
    void processPacket(const void* header, const unsigned char* packet);

    /**
     * @brief Packet inspection callback
     * @param packetData Packet data
     * @param packetSize Packet size
     * @return true if packet should be allowed, false if blocked
     */
    bool inspectPacket(const void* packetData, size_t packetSize);

    /**
     * @brief Check if packet is encrypted
     * @param packetData Packet data
     * @param packetSize Packet size
     * @return true if encrypted, false otherwise
     */
    bool isPacketEncrypted(const void* packetData, size_t packetSize);

    /**
     * @brief Extract packet information (source, destination, protocol, port)
     * @param packetData Packet data
     * @param packetSize Packet size
     * @param source Output: source IP address
     * @param destination Output: destination IP address
     * @param protocol Output: protocol name
     * @param port Output: port number (-1 if not applicable)
     */
    void extractPacketInfo(const void* packetData, size_t packetSize,
                          QString& source, QString& destination, QString& protocol, int& port);

    /**
     * @brief Match IP address against CIDR notation
     * @param ip IP address
     * @param cidr CIDR notation (e.g., "192.168.1.0/24")
     * @return true if IP matches CIDR, false otherwise
     */
    bool matchCIDR(const std::string& ip, const std::string& cidr);
};

#endif // NETWORK_ENFORCEMENT_H

