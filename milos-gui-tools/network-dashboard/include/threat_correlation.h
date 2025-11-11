#ifndef THREAT_CORRELATION_H
#define THREAT_CORRELATION_H

#include "threat_detector.h"
#include <string>
#include <vector>
#include <map>
#include <cstdint>

/**
 * @brief Correlated Threat
 * 
 * Represents a correlated threat from multiple threat events.
 */
struct CorrelatedThreat {
    std::string id;
    std::vector<std::string> threat_ids;  // IDs of correlated threats
    std::string pattern;  // Detected pattern
    std::string severity;  // Aggregated severity
    std::string description;
    uint64_t start_time;
    uint64_t end_time;
    uint32_t threat_count;
    std::map<std::string, std::string> metadata;
};

/**
 * @brief Threat Correlation Engine
 * 
 * Correlates multiple threat events to identify attack patterns.
 */
class ThreatCorrelationEngine {
public:
    ThreatCorrelationEngine();
    ~ThreatCorrelationEngine();

    /**
     * @brief Initialize correlation engine
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Add threat event for correlation
     * @param threat Threat event to add
     * @return Vector of correlated threats (if any)
     */
    std::vector<CorrelatedThreat> addThreat(const ThreatInfo& threat);

    /**
     * @brief Get correlated threats
     * @return Vector of correlated threats
     */
    std::vector<CorrelatedThreat> getCorrelatedThreats() const;

    /**
     * @brief Clear correlation history
     */
    void clear();

private:
    struct ThreatEvent {
        ThreatInfo threat;
        uint64_t timestamp;
    };

    std::vector<ThreatEvent> m_threatHistory;
    std::vector<CorrelatedThreat> m_correlatedThreats;
    uint64_t m_correlationWindow;  // Time window for correlation (ms)

    /**
     * @brief Correlate threats in time window
     */
    std::vector<CorrelatedThreat> correlateThreats(const ThreatEvent& newThreat);

    /**
     * @brief Check if threats match correlation pattern
     */
    bool matchCorrelationPattern(const std::vector<ThreatEvent>& threats) const;
};

#endif // THREAT_CORRELATION_H

