#ifndef PACKET_PARSER_H
#define PACKET_PARSER_H

#include "packet_capture.h"
#include <string>
#include <map>
#include <vector>

/**
 * @brief Protocol Information
 */
struct ProtocolInfo {
    std::string name;
    std::string description;
    size_t packet_count;
    size_t byte_count;
    std::map<std::string, std::string> metadata;
};

/**
 * @brief Packet Parser
 * 
 * Provides packet parsing and analysis using Wireshark API (when available)
 * or libpcap parsing.
 */
class PacketParser {
public:
    PacketParser();
    ~PacketParser();

    /**
     * @brief Initialize packet parser
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Parse packet data
     * @param packet Packet data
     * @return Parsed packet information
     */
    PacketData parsePacket(const PacketData& packet);

    /**
     * @brief Analyze protocol
     * @param packet Packet data
     * @return Protocol information
     */
    ProtocolInfo analyzeProtocol(const PacketData& packet);

    /**
     * @brief Extract packet metadata
     * @param packet Packet data
     * @return Metadata map
     */
    std::map<std::string, std::string> extractMetadata(const PacketData& packet);

    /**
     * @brief Check if parser is initialized
     * @return true if initialized, false otherwise
     */
    bool isInitialized() const { return m_initialized; }

private:
    bool m_initialized;
    bool m_wiresharkAvailable;

    /**
     * @brief Parse using Wireshark API
     */
    PacketData parseWithWireshark(const PacketData& packet);

    /**
     * @brief Parse using libpcap
     */
    PacketData parseWithLibpcap(const PacketData& packet);
};

#endif // PACKET_PARSER_H

