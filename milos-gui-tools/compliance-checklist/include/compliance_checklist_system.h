#ifndef COMPLIANCE_CHECKLIST_SYSTEM_H
#define COMPLIANCE_CHECKLIST_SYSTEM_H

#include "compliance_checklist.h"
#include "compliance_reporter.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include <QObject>

/**
 * @brief Compliance Checklist System
 * 
 * Main system for compliance checklist management.
 */
class ComplianceChecklistSystem : public QObject {
    Q_OBJECT

public:
    explicit ComplianceChecklistSystem(QObject* parent = nullptr);
    ~ComplianceChecklistSystem();

    /**
     * @brief Initialize compliance checklist system
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get checklist manager
     * @return Checklist manager
     */
    ComplianceChecklistManager* getChecklistManager() const { return m_checklistManager; }

    /**
     * @brief Get compliance reporter
     * @return Compliance reporter
     */
    ComplianceReporter* getReporter() const { return m_reporter; }

    /**
     * @brief Get D-Bus interface
     * @return D-Bus interface
     */
    ComplianceChecklistDBusInterface* getDBusInterface() const { return m_dbusInterface; }

private:
    ComplianceChecklistManager* m_checklistManager;
    ComplianceReporter* m_reporter;
    ComplianceChecklistDBusInterface* m_dbusInterface;
    AuditLogger* m_auditLogger;
    bool m_initialized;
};

#endif // COMPLIANCE_CHECKLIST_SYSTEM_H

