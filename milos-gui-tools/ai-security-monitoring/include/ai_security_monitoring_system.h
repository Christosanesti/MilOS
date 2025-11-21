#ifndef AI_SECURITY_MONITORING_SYSTEM_H
#define AI_SECURITY_MONITORING_SYSTEM_H

#include "threat_detector.h"
#include "incident_response.h"
#include "system_monitor.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include <QObject>

/**
 * @brief AI Security Monitoring System
 * 
 * Main system for AI-powered continuous security monitoring.
 */
class AISecurityMonitoringSystem : public QObject {
    Q_OBJECT

public:
    explicit AISecurityMonitoringSystem(QObject* parent = nullptr);
    ~AISecurityMonitoringSystem();

    /**
     * @brief Initialize AI security monitoring system
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Start monitoring
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop monitoring
     */
    void stop();

    /**
     * @brief Get threat detector
     * @return Threat detector
     */
    ThreatDetector* getThreatDetector() const { return m_threatDetector; }

    /**
     * @brief Get incident response
     * @return Incident response
     */
    IncidentResponse* getIncidentResponse() const { return m_incidentResponse; }

    /**
     * @brief Get system monitor
     * @return System monitor
     */
    SystemMonitor* getSystemMonitor() const { return m_systemMonitor; }

    /**
     * @brief Get D-Bus interface
     * @return D-Bus interface
     */
    AISecurityMonitoringDBusInterface* getDBusInterface() const { return m_dbusInterface; }

private:
    ThreatDetector* m_threatDetector;
    IncidentResponse* m_incidentResponse;
    SystemMonitor* m_systemMonitor;
    AISecurityMonitoringDBusInterface* m_dbusInterface;
    AuditLogger* m_auditLogger;
    bool m_initialized;
    bool m_running;
};

#endif // AI_SECURITY_MONITORING_SYSTEM_H

