#include "firewallservice.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QProcess>
#include <QDebug>

FirewallService::FirewallService(QObject *parent)
    : QObject(parent)
    , m_status("not_configured")
{
}

void FirewallService::checkStatus()
{
    // Check firewall status via D-Bus (firewalld) or systemd service status
    QString newStatus = "not_configured";
    
    // Try firewalld first (most common on modern Linux systems)
    QDBusConnection bus = QDBusConnection::systemBus();
    QDBusInterface firewalldInterface("org.fedoraproject.FirewallD1",
                                      "/org/fedoraproject/FirewallD1",
                                      "org.fedoraproject.FirewallD1",
                                      bus);
    
    if (firewalldInterface.isValid()) {
        QDBusReply<QString> reply = firewalldInterface.call("getDefaultZone");
        if (reply.isValid()) {
            // Firewalld is available, check if it's running
            QDBusInterface propertiesInterface("org.fedoraproject.FirewallD1",
                                               "/org/fedoraproject/FirewallD1",
                                               "org.freedesktop.DBus.Properties",
                                               bus);
            if (propertiesInterface.isValid()) {
                QDBusReply<QVariant> stateReply = propertiesInterface.call("Get",
                                                                           "org.fedoraproject.FirewallD1",
                                                                           "state");
                if (stateReply.isValid()) {
                    QString state = stateReply.value().toString();
                    if (state == "running") {
                        newStatus = "active";
                    } else {
                        newStatus = "inactive";
                    }
                } else {
                    newStatus = "active"; // Assume active if we can query it
                }
            }
        }
    }
    
    // Fallback: Check systemd service status for firewalld
    if (newStatus == "not_configured") {
        QProcess process;
        process.start("systemctl", QStringList() << "is-active" << "firewalld");
        process.waitForFinished(2000);
        if (process.exitCode() == 0) {
            newStatus = "active";
        } else {
            // Check for ufw (Ubuntu/Debian)
            process.start("systemctl", QStringList() << "is-active" << "ufw");
            process.waitForFinished(2000);
            if (process.exitCode() == 0) {
                newStatus = "active";
            } else {
                // Check if iptables has rules (basic check)
                process.start("iptables", QStringList() << "-L" << "-n");
                process.waitForFinished(2000);
                if (process.exitCode() == 0) {
                    QString output = process.readAllStandardOutput();
                    // If there are rules beyond default, consider it configured
                    if (output.contains("Chain") && output.split("\n").size() > 5) {
                        newStatus = "active";
                    } else {
                        newStatus = "inactive";
                    }
                } else {
                    newStatus = "not_configured";
                }
            }
        }
    }
    
    if (m_status != newStatus) {
        m_status = newStatus;
        emit statusChanged();
    }
}

bool FirewallService::isConfigured()
{
    return m_status != "not_configured";
}

bool FirewallService::isActive()
{
    return m_status == "active";
}

