#ifndef PACKET_STATISTICS_H
#define PACKET_STATISTICS_H

#include "packet_capture.h"
#include <string>
#include <map>
#include <atomic>
#include <mutex>
#include <cstdint>

/**
 * @brief Protocol Statistics
 */
struct ProtocolStats {
    uint64_t packet_count;
    uint64_t byte_count;
    double percentage_packets;
    double percentage_bytes;
};

/**
 * @brief Packet Statistics
 * 
 * Tracks packet capture statistics including packet counts,
 * byte counts, and protocol distribution.
 */
class PacketStatistics {
public:
    PacketStatistics();
    ~PacketStatistics();

    /**
     * @brief Initialize statistics
     */
    void initialize();

    /**
     * @brief Update statistics with packet
     * @param packet Packet data
     */
    void updateStatistics(const PacketData& packet);

    /**
     * @brief Get total packet count
     * @return Total packet count
     */
    uint64_t getTotalPackets() const { return m_totalPackets.load(); }

    /**
     * @brief Get total byte count
     * @return Total byte count
     */
    uint64_t getTotalBytes() const { return m_totalBytes.load(); }

    /**
     * @brief Get protocol statistics
     * @return Map of protocol name to statistics
     */
    std::map<std::string, ProtocolStats> getProtocolStatistics() const;

    /**
     * @brief Get statistics for specific protocol
     * @param protocol Protocol name
     * @return Protocol statistics
     */
    ProtocolStats getProtocolStats(const std::string& protocol) const;

    /**
     * @brief Reset statistics
     */
    void reset();

private:
    std::atomic<uint64_t> m_totalPackets;
    std::atomic<uint64_t> m_totalBytes;
    std::map<std::string, std::atomic<uint64_t>> m_protocolPackets;
    std::map<std::string, std::atomic<uint64_t>> m_protocolBytes;
    mutable std::mutex m_mutex;

    /**
     * @brief Get or create atomic counter for protocol
     */
    std::atomic<uint64_t>& getProtocolPacketCounter(const std::string& protocol);
    std::atomic<uint64_t>& getProtocolByteCounter(const std::string& protocol);
};

#endif // PACKET_STATISTICS_H

