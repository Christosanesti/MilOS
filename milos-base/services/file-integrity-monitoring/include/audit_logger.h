#ifndef AUDIT_LOGGER_H
#define AUDIT_LOGGER_H

#include <string>

/**
 * @brief Audit Logger
 * 
 * Logs FIM events to the Audit Service.
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
     * @brief Log FIM event
     * @param event Event name
     * @param details Event details
     */
    void logEvent(const std::string& event, const std::string& details);

private:
    bool m_initialized;
    void* m_dbusInterface;  // QDBusInterface pointer
};

#endif // AUDIT_LOGGER_H

