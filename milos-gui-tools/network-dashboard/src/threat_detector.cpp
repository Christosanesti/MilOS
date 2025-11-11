#include "threat_detector.h"
#include <algorithm>
#include <cstring>

ThreatDetector::ThreatDetector()
    : m_initialized(false)
    , m_running(false)
    , m_ruleManager(nullptr)
{
}

ThreatDetector::~ThreatDetector() {
    stop();
}

bool ThreatDetector::initialize(IDSRuleManager* ruleManager) {
    if (m_initialized) {
        return true;
    }

    if (!ruleManager) {
        std::cerr << "Rule manager is null" << std::endl;
        return false;
    }

    m_ruleManager = ruleManager;
    m_initialized = true;
    return true;
}

bool ThreatDetector::start(ThreatDetectedCallback callback) {
    if (!m_initialized) {
        return false;
    }

    m_callback = callback;
    m_running = true;
    return true;
}

void ThreatDetector::stop() {
    m_running = false;
    m_callback = nullptr;
}

std::vector<ThreatInfo> ThreatDetector::analyzePacket(const PacketData& packet) {
    std::vector<ThreatInfo> threats;

    if (!m_running || !m_ruleManager) {
        return threats;
    }

    // Get enabled rules
    auto rules = m_ruleManager->getEnabledRules();

    // Check packet against each rule
    for (const auto& rule : rules) {
        if (matchRule(packet, rule)) {
            ThreatInfo threat = createThreatInfo(rule, packet);
            threats.push_back(threat);

            // Call callback if provided
            if (m_callback) {
                m_callback(threat);
            }
        }
    }

    return threats;
}

bool ThreatDetector::matchRule(const PacketData& packet, const IDSRule& rule) const {
    // Check protocol
    if (!checkProtocol(packet, rule)) {
        return false;
    }

    // Check address
    if (!checkAddress(packet, rule)) {
        return false;
    }

    // Check port
    if (!checkPort(packet, rule)) {
        return false;
    }

    // Check pattern
    if (!rule.pattern.empty() && !checkPattern(packet, rule)) {
        return false;
    }

    // Check keywords
    if (!rule.keywords.empty() && !checkKeywords(packet, rule)) {
        return false;
    }

    return true;
}

bool ThreatDetector::checkProtocol(const PacketData& packet, const IDSRule& rule) const {
    if (rule.protocol.empty()) {
        return true;  // No protocol filter
    }
    return packet.protocol == rule.protocol;
}

bool ThreatDetector::checkAddress(const PacketData& packet, const IDSRule& rule) const {
    if (!rule.source_address.empty() && packet.source_address != rule.source_address) {
        return false;
    }
    if (!rule.destination_address.empty() && packet.destination_address != rule.destination_address) {
        return false;
    }
    return true;
}

bool ThreatDetector::checkPort(const PacketData& packet, const IDSRule& rule) const {
    if (rule.source_port > 0 && packet.source_port != rule.source_port) {
        return false;
    }
    if (rule.destination_port > 0 && packet.destination_port != rule.destination_port) {
        return false;
    }
    return true;
}

bool ThreatDetector::checkPattern(const PacketData& packet, const IDSRule& rule) const {
    if (rule.pattern.empty()) {
        return true;
    }

    // Simple pattern matching (in production, use regex or more sophisticated matching)
    const uint8_t* data = packet.data.data();
    size_t dataLen = packet.data.size();
    const char* pattern = rule.pattern.c_str();
    size_t patternLen = rule.pattern.length();

    if (patternLen > dataLen) {
        return false;
    }

    // Simple substring search
    for (size_t i = 0; i <= dataLen - patternLen; i++) {
        if (std::memcmp(data + i, pattern, patternLen) == 0) {
            return true;
        }
    }

    return false;
}

bool ThreatDetector::checkKeywords(const PacketData& packet, const IDSRule& rule) const {
    if (rule.keywords.empty()) {
        return true;
    }

    // Check if any keyword matches
    for (const auto& keyword : rule.keywords) {
        if (checkPattern(packet, { "", "", "", "", "", "", "", 0, 0, keyword, {}, false, 0, {} })) {
            return true;
        }
    }

    return false;
}

ThreatInfo ThreatDetector::createThreatInfo(const IDSRule& rule, const PacketData& packet) const {
    ThreatInfo threat;
    threat.id = rule.id + "_" + std::to_string(packet.timestamp);
    threat.rule_id = rule.id;
    threat.rule_name = rule.name;
    threat.severity = rule.severity;
    threat.source_address = packet.source_address;
    threat.destination_address = packet.destination_address;
    threat.source_port = packet.source_port;
    threat.destination_port = packet.destination_port;
    threat.protocol = packet.protocol;
    threat.description = rule.description;
    threat.timestamp = packet.timestamp;
    
    // Sanitize packet data (keep only headers)
    size_t headerSize = std::min(packet.data.size(), static_cast<size_t>(128));
    threat.packet_data.assign(packet.data.begin(), packet.data.begin() + headerSize);

    threat.metadata["interface"] = packet.interface;
    threat.metadata["packet_length"] = std::to_string(packet.length);

    return threat;
}

