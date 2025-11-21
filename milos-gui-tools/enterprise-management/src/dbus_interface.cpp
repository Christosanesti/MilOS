#include "dbus_interface.h"
#include "enterprise_management_system.h"
#include "config_manager.h"
#include "deployment_orchestrator.h"
#include "multi_site_manager.h"
#include <QDebug>
#include <QDBusConnection>

EnterpriseManagementDBusInterface::EnterpriseManagementDBusInterface(QObject* parent)
    : QObject(parent)
    , m_system(nullptr)
{
}

EnterpriseManagementDBusInterface::~EnterpriseManagementDBusInterface() {
}

bool EnterpriseManagementDBusInterface::initialize() {
    if (!QDBusConnection::sessionBus().registerService("org.milos.EnterpriseManagement")) {
        qWarning() << "Failed to register D-Bus service";
        return false;
    }
    
    if (!QDBusConnection::sessionBus().registerObject("/org/milos/EnterpriseManagement", this,
                                                        QDBusConnection::ExportAllSlots |
                                                        QDBusConnection::ExportAllSignals)) {
        qWarning() << "Failed to register D-Bus object";
        return false;
    }
    
    return true;
}

void EnterpriseManagementDBusInterface::setSystem(EnterpriseManagementSystem* system) {
    m_system = system;
}

QVariant EnterpriseManagementDBusInterface::GetConfig(const QString& key) {
    if (!m_system || !m_system->getConfigManager()) {
        return QVariant();
    }
    
    return m_system->getConfigManager()->getConfig(key);
}

bool EnterpriseManagementDBusInterface::SetConfig(const QString& key, const QVariant& value) {
    if (!m_system || !m_system->getConfigManager()) {
        return false;
    }
    
    return m_system->getConfigManager()->setConfig(key, value);
}

QVariantMap EnterpriseManagementDBusInterface::GetAllConfigs() {
    if (!m_system || !m_system->getConfigManager()) {
        return QVariantMap();
    }
    
    return m_system->getConfigManager()->getAllConfigs();
}

bool EnterpriseManagementDBusInterface::ExportConfig(const QString& filePath) {
    if (!m_system || !m_system->getConfigManager()) {
        return false;
    }
    
    return m_system->getConfigManager()->exportConfig(filePath);
}

bool EnterpriseManagementDBusInterface::ImportConfig(const QString& filePath) {
    if (!m_system || !m_system->getConfigManager()) {
        return false;
    }
    
    return m_system->getConfigManager()->importConfig(filePath);
}

QString EnterpriseManagementDBusInterface::CreateDeployment(const QString& name, const QStringList& targets) {
    if (!m_system || !m_system->getDeploymentOrchestrator()) {
        return QString();
    }
    
    // Create simple deployment tasks
    QList<DeploymentTask> tasks;
    for (const QString& target : targets) {
        DeploymentTask task;
        task.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
        task.name = QString("Deploy to %1").arg(target);
        task.target = target;
        task.commands = QStringList() << "deploy" << "verify";
        task.status = DeploymentStatus::Pending;
        tasks.append(task);
    }
    
    return m_system->getDeploymentOrchestrator()->createDeployment(name, targets, tasks);
}

bool EnterpriseManagementDBusInterface::ExecuteDeployment(const QString& deploymentId) {
    if (!m_system || !m_system->getDeploymentOrchestrator()) {
        return false;
    }
    
    return m_system->getDeploymentOrchestrator()->executeDeployment(deploymentId);
}

QVariantMap EnterpriseManagementDBusInterface::GetDeploymentStatus(const QString& deploymentId) {
    if (!m_system || !m_system->getDeploymentOrchestrator()) {
        return QVariantMap();
    }
    
    return m_system->getDeploymentOrchestrator()->getDeploymentStatus(deploymentId);
}

QStringList EnterpriseManagementDBusInterface::GetSites() {
    if (!m_system || !m_system->getMultiSiteManager()) {
        return QStringList();
    }
    
    return m_system->getMultiSiteManager()->getAllSites();
}

QVariantMap EnterpriseManagementDBusInterface::GetSiteStatus(const QString& siteId) {
    if (!m_system || !m_system->getMultiSiteManager()) {
        return QVariantMap();
    }
    
    return m_system->getMultiSiteManager()->getSiteStatistics(siteId);
}

