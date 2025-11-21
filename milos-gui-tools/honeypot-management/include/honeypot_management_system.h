#ifndef HONEYPOT_MANAGEMENT_SYSTEM_H
#define HONEYPOT_MANAGEMENT_SYSTEM_H

#include "honeypot_manager.h"
#include "threat_analyzer.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include <QObject>

/**
 * @brief Honeypot Management System
 * 
 * Main system for honeypot management.
 */
class HoneypotManagementSystem : public QObject {
    Q_OBJECT

public:
    explicit HoneypotManagementSystem(QObject* parent = nullptr);
    ~HoneypotManagementSystem();

    /**
     * @brief Initialize honeypot management system
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get honeypot manager
     * @return Honeypot manager
     */
    HoneypotManager* getHoneypotManager() const { return m_honeypotManager; }

    /**
     * @brief Get threat analyzer
     * @return Threat analyzer
     */
    ThreatAnalyzer* getThreatAnalyzer() const { return m_threatAnalyzer; }

    /**
     * @brief Get D-Bus interface
     * @return D-Bus interface
     */
    HoneypotManagementDBusInterface* getDBusInterface() const { return m_dbusInterface; }

private:
    HoneypotManager* m_honeypotManager;
    ThreatAnalyzer* m_threatAnalyzer;
    HoneypotManagementDBusInterface* m_dbusInterface;
    AuditLogger* m_auditLogger;
    bool m_initialized;
};

#endif // HONEYPOT_MANAGEMENT_SYSTEM_H

