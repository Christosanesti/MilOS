#ifndef ENTERPRISE_MANAGEMENT_SYSTEM_H
#define ENTERPRISE_MANAGEMENT_SYSTEM_H

#include "config_manager.h"
#include "deployment_orchestrator.h"
#include "multi_site_manager.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include <QObject>

/**
 * @brief Enterprise Management System
 * 
 * Main system for enterprise management features.
 */
class EnterpriseManagementSystem : public QObject {
    Q_OBJECT

public:
    explicit EnterpriseManagementSystem(QObject* parent = nullptr);
    ~EnterpriseManagementSystem();

    /**
     * @brief Initialize enterprise management system
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get configuration manager
     * @return Configuration manager
     */
    ConfigManager* getConfigManager() const { return m_configManager; }

    /**
     * @brief Get deployment orchestrator
     * @return Deployment orchestrator
     */
    DeploymentOrchestrator* getDeploymentOrchestrator() const { return m_deploymentOrchestrator; }

    /**
     * @brief Get multi-site manager
     * @return Multi-site manager
     */
    MultiSiteManager* getMultiSiteManager() const { return m_multiSiteManager; }

    /**
     * @brief Get D-Bus interface
     * @return D-Bus interface
     */
    EnterpriseManagementDBusInterface* getDBusInterface() const { return m_dbusInterface; }

private:
    ConfigManager* m_configManager;
    DeploymentOrchestrator* m_deploymentOrchestrator;
    MultiSiteManager* m_multiSiteManager;
    EnterpriseManagementDBusInterface* m_dbusInterface;
    AuditLogger* m_auditLogger;
    bool m_initialized;
};

#endif // ENTERPRISE_MANAGEMENT_SYSTEM_H

