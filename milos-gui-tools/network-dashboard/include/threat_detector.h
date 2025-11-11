#ifndef THREAT_DETECTOR_H
#define THREAT_DETECTOR_H

#include "ids_rule.h"
#include "packet_capture.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

// Forward declaration
struct ThreatInfo;

/**
 * @brief Threat Detection Callback
 */
using ThreatDetectedCallback = std::function<void(const ThreatInfo& threat)>;

/**
 * @brief Threat Information
 */
struct ThreatInfo {
    std::string id;
    std::string rule_id;
    std::string rule_name;
    std::string severity;
    std::string source_address;
    std::string destination_address;
    uint16_t source_port;
    uint16_t destination_port;
    std::string protocol;
    std::string description;
    uint64_t timestamp;
    std::vector<uint8_t> packet_data;  // Sanitized packet data
    std::map<std::string, std::string> metadata;
};

/**
 * @brief Threat Detector
 * 
 * Implements real-time threat detection by analyzing packets against IDS rules.
 */
class ThreatDetector {
public:
    ThreatDetector();
    ~ThreatDetector();

    /**
     * @brief Initialize threat detector
     * @param ruleManager IDS rule manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize(IDSRuleManager* ruleManager);

    /**
     * @brief Start threat detection
     * @param callback Callback for detected threats
     * @return true if start successful, false otherwise
     */
    bool start(ThreatDetectedCallback callback = nullptr);

    /**
     * @brief Stop threat detection
     */
    void stop();

    /**
     * @brief Analyze packet for threats
     * @param packet Packet to analyze
     * @return Vector of detected threats
     */
    std::vector<ThreatInfo> analyzePacket(const PacketData& packet);

    /**
     * @brief Check if detector is running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return m_running; }

private:
    bool m_initialized;
    bool m_running;
    IDSRuleManager* m_ruleManager;
    ThreatDetectedCallback m_callback;

    /**
     * @brief Match packet against rule
     */
    bool matchRule(const PacketData& packet, const IDSRule& rule) const;

    /**
     * @brief Check protocol match
     */
    bool checkProtocol(const PacketData& packet, const IDSRule& rule) const;

    /**
     * @brief Check address match
     */
    bool checkAddress(const PacketData& packet, const IDSRule& rule) const;

    /**
     * @brief Check port match
     */
    bool checkPort(const PacketData& packet, const IDSRule& rule) const;

    /**
     * @brief Check pattern match
     */
    bool checkPattern(const PacketData& packet, const IDSRule& rule) const;

    /**
     * @brief Check keyword match
     */
    bool checkKeywords(const PacketData& packet, const IDSRule& rule) const;

    /**
     * @brief Create threat info from rule and packet
     */
    ThreatInfo createThreatInfo(const IDSRule& rule, const PacketData& packet) const;
};

#endif // THREAT_DETECTOR_H

