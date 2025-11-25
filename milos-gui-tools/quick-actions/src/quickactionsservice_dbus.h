#ifndef QUICKACTIONSSERVICE_DBUS_H
#define QUICKACTIONSSERVICE_DBUS_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QVariantList>
#include "quickactionsservice.h"
#include "actiondetector.h"

/**
 * @brief Quick Actions D-Bus Service
 * 
 * D-Bus interface for contextual quick actions service
 */
class QuickActionsServiceDBus : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.QuickActions")

public:
    explicit QuickActionsServiceDBus(QObject* parent = nullptr);
    ~QuickActionsServiceDBus();

    bool initialize();
    bool start();
    void stop();
    bool isRunning() const { return m_running; }

public Q_SLOTS:
    // D-Bus methods
    QVariantList GetAvailableActions();
    bool ExecuteAction(const QString& actionId, const QVariantMap& parameters);
    bool GetActionAvailability(const QString& actionId);
    QString RegisterAction(const QVariantMap& actionDefinition);

Q_SIGNALS:
    void ActiveWindowChanged(const QString& windowId, const QString& applicationName);
    void ActionAvailabilityChanged(const QString& actionId, bool available);

private:
    bool registerInterface();
    void unregisterInterface();

    QuickActionsService* m_service;
    ActionDetector* m_actionDetector;
    bool m_running;
    bool m_initialized;
};

#endif // QUICKACTIONSSERVICE_DBUS_H

