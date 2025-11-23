#ifndef AUDIT_LOGGER_H
#define AUDIT_LOGGER_H

#include <string>

/**
 * @brief Audit Logger
 * 
 * Logs events to audit service.
 */
class AuditLogger {
public:
    AuditLogger();
    ~AuditLogger();

    /**
     * @brief Initialize audit logger
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Log event
     * @param eventType Event type
     * @param eventData Event data/description
     */
    void logEvent(const std::string& eventType, const std::string& eventData);

private:
    bool m_initialized;
    std::string m_serviceName;
};

#endif // AUDIT_LOGGER_H

