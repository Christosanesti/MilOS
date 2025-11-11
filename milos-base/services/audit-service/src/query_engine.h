#ifndef QUERY_ENGINE_H
#define QUERY_ENGINE_H

#include <memory>
#include <string>
#include <vector>
#include <map>

class ConfigParser;
class LogStorage;

/**
 * @brief Query Engine
 * 
 * Provides query and analysis capabilities for audit logs.
 * Handles query parsing, filtering, sorting, pagination, and export.
 */
class QueryEngine {
public:
    QueryEngine();
    ~QueryEngine();

    /**
     * @brief Initialize query engine
     * @param configParser Configuration parser instance
     * @param logStorage Log storage instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(ConfigParser* configParser, LogStorage* logStorage);

    /**
     * @brief Query audit logs
     * @param queryParams Query parameters (JSON string)
     * @return Query results (JSON string)
     */
    std::string queryLogs(const std::string& queryParams);

    /**
     * @brief Get event statistics
     * @param queryParams Query parameters (JSON string)
     * @return Statistics (JSON string)
     */
    std::string getStatistics(const std::string& queryParams);

    /**
     * @brief Export audit logs
     * @param timeRange Time range (JSON string)
     * @param format Export format (json, csv, xml)
     * @return Exported data (string)
     */
    std::string exportLogs(const std::string& timeRange, const std::string& format);

private:
    bool m_initialized;
    ConfigParser* m_configParser;
    LogStorage* m_logStorage;

    /**
     * @brief Parse query parameters
     * @param queryParams Query parameters (JSON string)
     * @return Map of filter parameters
     */
    std::map<std::string, std::string> parseQueryParams(const std::string& queryParams);

    /**
     * @brief Export to JSON format
     * @param entries Log entries
     * @return JSON string
     */
    std::string exportToJSON(const std::vector<struct AuditLogEntry>& entries);

    /**
     * @brief Export to CSV format
     * @param entries Log entries
     * @return CSV string
     */
    std::string exportToCSV(const std::vector<struct AuditLogEntry>& entries);

    /**
     * @brief Export to XML format
     * @param entries Log entries
     * @return XML string
     */
    std::string exportToXML(const std::vector<struct AuditLogEntry>& entries);
};

#endif // QUERY_ENGINE_H

