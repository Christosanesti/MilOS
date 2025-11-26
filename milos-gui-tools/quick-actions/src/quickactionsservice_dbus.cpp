#include "quickactionsservice_dbus.h"
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>

QuickActionsServiceDBus::QuickActionsServiceDBus(QObject* parent)
    : QObject(parent)
    , m_service(new QuickActionsService(this))
    , m_actionDetector(new ActionDetector(this))
    , m_running(false)
    , m_initialized(false)
{
    connect(m_actionDetector, &ActionDetector::activeWindowChanged,
            this, &QuickActionsServiceDBus::ActiveWindowChanged);
}

QuickActionsServiceDBus::~QuickActionsServiceDBus()
{
    stop();
}

bool QuickActionsServiceDBus::initialize()
{
    if (m_initialized) {
        return true;
    }

    m_initialized = true;
    return true;
}

bool QuickActionsServiceDBus::start()
{
    if (!m_initialized) {
        qWarning() << "Quick Actions D-Bus service not initialized";
        return false;
    }

    if (m_running) {
        return true;
    }

    if (!registerInterface()) {
        qWarning() << "Failed to register D-Bus interface";
        return false;
    }

    m_running = true;
    qDebug() << "Quick Actions D-Bus service started";
    return true;
}

void QuickActionsServiceDBus::stop()
{
    if (!m_running) {
        return;
    }

    unregisterInterface();
    m_running = false;
    qDebug() << "Quick Actions D-Bus service stopped";
}

bool QuickActionsServiceDBus::registerInterface()
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    
    if (!connection.isConnected()) {
        qWarning() << "Cannot connect to D-Bus session bus";
        return false;
    }

    if (!connection.registerService("org.milos.QuickActions")) {
        QDBusError error = connection.lastError();
        if (error.isValid()) {
            qWarning() << "Failed to register D-Bus service:" << error.message();
        }
        return false;
    }

    if (!connection.registerObject("/org/milos/QuickActions", this,
                                   QDBusConnection::ExportAllSlots |
                                   QDBusConnection::ExportAllSignals)) {
        QDBusError error = connection.lastError();
        if (error.isValid()) {
            qWarning() << "Failed to register D-Bus object:" << error.message();
            connection.unregisterService("org.milos.QuickActions");
            return false;
        }
    }

    return true;
}

void QuickActionsServiceDBus::unregisterInterface()
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    connection.unregisterObject("/org/milos/QuickActions");
    connection.unregisterService("org.milos.QuickActions");
}

QVariantList QuickActionsServiceDBus::GetAvailableActions()
{
    QVariantMap context = m_actionDetector->getActiveWindowContext();
    return m_actionDetector->getAvailableActions(context);
}

bool QuickActionsServiceDBus::ExecuteAction(const QString& actionId, const QVariantMap& parameters)
{
    Q_UNUSED(parameters)
    m_service->executeAction(actionId);
    return true;
}

bool QuickActionsServiceDBus::GetActionAvailability(const QString& actionId)
{
    QVariantList actions = GetAvailableActions();
    for (const QVariant& actionVar : actions) {
        QVariantMap action = actionVar.toMap();
        if (action.value("action_id").toString() == actionId) {
            return true;
        }
    }
    return false;
}

QString QuickActionsServiceDBus::RegisterAction(const QVariantMap& actionDefinition)
{
    // Register custom action - simplified implementation
    Q_UNUSED(actionDefinition)
    return QString(); // Would return action_id
}


