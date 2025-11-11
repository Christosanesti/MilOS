#ifndef THREAT_RESPONSE_H
#define THREAT_RESPONSE_H

#include "threat_detector.h"
#include <string>
#include <vector>
#include <map>

/**
 * @brief Response Action
 */
enum class ResponseAction {
    None,
    BlockSource,
    BlockDestination,
    BlockConnection,
    Alert,
    Log,
    Quarantine
};

/**
 * @brief Response Rule
 */
struct ResponseRule {
    std::string id;
    std::string threat_severity;  // Minimum severity to trigger
    std::string threat_pattern;  // Pattern to match
    ResponseAction action;
    std::map<std::string, std::string> parameters;  // Action parameters
    bool enabled;
};

/**
 * @brief Threat Response Manager
 * 
 * Manages automated threat response actions.
 */
class ThreatResponseManager {
public:
    ThreatResponseManager();
    ~ThreatResponseManager();

    /**
     * @brief Initialize response manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Add response rule
     * @param rule Response rule to add
     * @return true if add successful, false otherwise
     */
    bool addResponseRule(const ResponseRule& rule);

    /**
     * @brief Execute response for threat
     * @param threat Threat to respond to
     * @return true if response executed, false otherwise
     */
    bool executeResponse(const ThreatInfo& threat);

    /**
     * @brief Get response rules
     * @return Vector of response rules
     */
    std::vector<ResponseRule> getResponseRules() const;

private:
    std::vector<ResponseRule> m_responseRules;

    /**
     * @brief Find matching response rules
     */
    std::vector<ResponseRule> findMatchingRules(const ThreatInfo& threat) const;

    /**
     * @brief Execute response action
     */
    bool executeAction(const ResponseRule& rule, const ThreatInfo& threat);

    /**
     * @brief Block source address
     */
    bool blockSource(const std::string& address);

    /**
     * @brief Block destination address
     */
    bool blockDestination(const std::string& address);

    /**
     * @brief Block connection
     */
    bool blockConnection(const ThreatInfo& threat);
};

#endif // THREAT_RESPONSE_H

