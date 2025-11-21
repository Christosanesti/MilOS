#ifndef ENTERPRISE_MANAGEMENT_DBUS_INTERFACE_H
#define ENTERPRISE_MANAGEMENT_DBUS_INTERFACE_H

#include "enterprise_management_system.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

class EnterpriseManagementSystem;

/**
 * @brief Enterprise Management D-Bus Interface
 */
class EnterpriseManagementDBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.EnterpriseManagement")

public:
    explicit EnterpriseManagementDBusInterface(QObject* parent = nullptr);
    ~EnterpriseManagementDBusInterface();

    bool initialize();
    void setSystem(EnterpriseManagementSystem* system);

public slots:
    QVariant GetConfig(const QString& key);
    bool SetConfig(const QString& key, const QVariant& value);
    QVariantMap GetAllConfigs();
    bool ExportConfig(const QString& filePath);
    bool ImportConfig(const QString& filePath);
    QString CreateDeployment(const QString& name, const QStringList& targets);
    bool ExecuteDeployment(const QString& deploymentId);
    QVariantMap GetDeploymentStatus(const QString& deploymentId);
    QStringList GetSites();
    QVariantMap GetSiteStatus(const QString& siteId);

private:
    EnterpriseManagementSystem* m_system;
};

#endif // ENTERPRISE_MANAGEMENT_DBUS_INTERFACE_H

