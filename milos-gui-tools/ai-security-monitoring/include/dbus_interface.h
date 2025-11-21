#ifndef AI_SECURITY_MONITORING_DBUS_INTERFACE_H
#define AI_SECURITY_MONITORING_DBUS_INTERFACE_H

#include "ai_security_monitoring_system.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

class AISecurityMonitoringSystem;

/**
 * @brief AI Security Monitoring D-Bus Interface
 */
class AISecurityMonitoringDBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.AISecurityMonitoring")

public:
    explicit AISecurityMonitoringDBusInterface(QObject* parent = nullptr);
    ~AISecurityMonitoringDBusInterface();

    bool initialize();
    void setSystem(AISecurityMonitoringSystem* system);

public slots:
    bool StartMonitoring();
    void StopMonitoring();
    bool IsMonitoring();
    QVariantMap GetThreats();
    QVariantMap GetSystemMetrics();
    QVariantList GetProcessList();
    QVariantList GetNetworkConnections();
    bool ContainThreat(const QString& threatId);
    QVariantMap GetStatistics();

private:
    AISecurityMonitoringSystem* m_system;
};

#endif // AI_SECURITY_MONITORING_DBUS_INTERFACE_H

