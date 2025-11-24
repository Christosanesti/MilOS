#ifndef HONEYPOT_MANAGEMENT_DBUS_INTERFACE_H
#define HONEYPOT_MANAGEMENT_DBUS_INTERFACE_H

#include "honeypot_management_system.h"
#include <QObject>
#include <QString>
#include <QVariantMap>

class HoneypotManagementSystem;

/**
 * @brief Honeypot Management D-Bus Interface
 */
class HoneypotManagementDBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.HoneypotManagement")

public:
    explicit HoneypotManagementDBusInterface(QObject* parent = nullptr);
    ~HoneypotManagementDBusInterface();

    bool initialize();
    void setSystem(HoneypotManagementSystem* system);

public slots:
    QString DeployHoneypot(int type, const QString& name, const QString& host, int port);
    bool RemoveHoneypot(const QString& honeypotId);
    bool StartHoneypot(const QString& honeypotId);
    bool StopHoneypot(const QString& honeypotId);
    QStringList GetHoneypots();
    QVariantMap GetHoneypotStatus(const QString& honeypotId);
    QVariantMap GetThreatIntelligence();

private:
    HoneypotManagementSystem* m_system;
};

#endif // HONEYPOT_MANAGEMENT_DBUS_INTERFACE_H






