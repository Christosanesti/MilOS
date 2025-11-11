#ifndef PACKET_SANITIZER_H
#define PACKET_SANITIZER_H

#include "packet_capture.h"
#include <vector>
#include <string>

/**
 * @brief Packet Sanitizer
 * 
 * Sanitizes packet data before storage by removing sensitive information
 * and encrypting data at rest.
 */
class PacketSanitizer {
public:
    PacketSanitizer();
    ~PacketSanitizer();

    /**
     * @brief Initialize sanitizer
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Sanitize packet data
     * @param packet Original packet data
     * @return Sanitized packet data
     */
    PacketData sanitizePacket(const PacketData& packet);

    /**
     * @brief Remove sensitive data from packet
     * @param packet Packet data
     * @return Packet with sensitive data removed
     */
    PacketData removeSensitiveData(const PacketData& packet);

    /**
     * @brief Encrypt packet data
     * @param packet Packet data
     * @return Encrypted packet data
     */
    std::vector<uint8_t> encryptPacketData(const PacketData& packet);

private:
    bool m_initialized;

    /**
     * @brief Check if data contains sensitive information
     */
    bool containsSensitiveData(const std::vector<uint8_t>& data) const;

    /**
     * @brief Mask sensitive data
     */
    std::vector<uint8_t> maskSensitiveData(const std::vector<uint8_t>& data) const;
};

#endif // PACKET_SANITIZER_H

