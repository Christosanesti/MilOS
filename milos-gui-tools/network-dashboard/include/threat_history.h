#ifndef THREAT_HISTORY_H
#define THREAT_HISTORY_H

#include "threat_detector.h"
#include "threat_correlation.h"
#include <string>
#include <vector>
#include <cstdint>
#include <sqlite3.h>

/**
 * @brief Threat History Manager
 * 
 * Manages threat history storage and retrieval using SQLite.
 */
class ThreatHistoryManager {
public:
    ThreatHistoryManager();
    ~ThreatHistoryManager();

    /**
     * @brief Initialize history manager
     * @param dbPath Path to SQLite database
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const std::string& dbPath);

    /**
     * @brief Store threat event
     * @param threat Threat to store
     * @return true if store successful, false otherwise
     */
    bool storeThreat(const ThreatInfo& threat);

    /**
     * @brief Store correlated threat
     * @param correlatedThreat Correlated threat to store
     * @return true if store successful, false otherwise
     */
    bool storeCorrelatedThreat(const CorrelatedThreat& correlatedThreat);

    /**
     * @brief Query threats
     * @param startTime Start time (timestamp)
     * @param endTime End time (timestamp)
     * @param severity Filter by severity (empty for all)
     * @return Vector of threats
     */
    std::vector<ThreatInfo> queryThreats(
        uint64_t startTime = 0,
        uint64_t endTime = 0,
        const std::string& severity = ""
    ) const;

    /**
     * @brief Get threat statistics
     * @param startTime Start time (timestamp)
     * @param endTime End time (timestamp)
     * @return Map of statistics (severity -> count)
     */
    std::map<std::string, uint32_t> getThreatStatistics(
        uint64_t startTime = 0,
        uint64_t endTime = 0
    ) const;

    /**
     * @brief Cleanup old threats
     * @param maxAge Maximum age in seconds
     * @return Number of threats removed
     */
    uint32_t cleanupOldThreats(uint64_t maxAge);

private:
    sqlite3* m_db;
    bool m_initialized;

    /**
     * @brief Create database schema
     */
    bool createSchema();

    /**
     * @brief Encrypt threat data
     */
    std::vector<uint8_t> encryptThreatData(const ThreatInfo& threat) const;

    /**
     * @brief Decrypt threat data
     */
    ThreatInfo decryptThreatData(const std::vector<uint8_t>& encrypted) const;
};

#endif // THREAT_HISTORY_H

