#include "dbus_interface_wrapper.h"
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

DBusInterfaceWrapper::DBusInterfaceWrapper(QObject* parent)
    : QObject(parent)
    , m_dbusInterface(nullptr)
    , m_connection(QDBusConnection::systemBus())
    , m_serviceAvailable(false)
    , m_currentStatus("Unknown")
{
    connectToService();
    
    // Monitor service registration
    QDBusConnection::systemBus().connect(
        "org.freedesktop.DBus",
        "/org/freedesktop/DBus",
        "org.freedesktop.DBus",
        "NameOwnerChanged",
        this,
        SLOT(onServiceRegistered(QString))
    );
    
    checkServiceAvailability();
}

DBusInterfaceWrapper::~DBusInterfaceWrapper() {
    disconnectFromService();
}

void DBusInterfaceWrapper::connectToService() {
    if (m_dbusInterface) {
        delete m_dbusInterface;
    }
    
    m_dbusInterface = new QDBusInterface(
        "org.milos.AuditService",
        "/org/milos/AuditService",
        "org.milos.AuditService",
        m_connection,
        this
    );
    
    if (!m_dbusInterface->isValid()) {
        qWarning() << "D-Bus interface not valid:" << m_dbusInterface->lastError().message();
        m_serviceAvailable = false;
        emit serviceAvailableChanged(m_serviceAvailable);
        return;
    }
    
    // Connect to signals
    m_connection.connect(
        "org.milos.AuditService",
        "/org/milos/AuditService",
        "org.milos.AuditService",
        "EventLogged",
        this,
        SLOT(onEventLogged(QString))
    );
    
    m_connection.connect(
        "org.milos.AuditService",
        "/org/milos/AuditService",
        "org.milos.AuditService",
        "IntegrityViolationDetected",
        this,
        SLOT(onIntegrityViolationDetected())
    );
    
    m_serviceAvailable = true;
    emit serviceAvailableChanged(m_serviceAvailable);
}

void DBusInterfaceWrapper::disconnectFromService() {
    if (m_dbusInterface) {
        m_connection.disconnect(
            "org.milos.AuditService",
            "/org/milos/AuditService",
            "org.milos.AuditService",
            "EventLogged",
            this,
            SLOT(onEventLogged(QString))
        );
        
        m_connection.disconnect(
            "org.milos.AuditService",
            "/org/milos/AuditService",
            "org.milos.AuditService",
            "IntegrityViolationDetected",
            this,
            SLOT(onIntegrityViolationDetected())
        );
    }
}

void DBusInterfaceWrapper::checkServiceAvailability() {
    if (!m_dbusInterface || !m_dbusInterface->isValid()) {
        m_serviceAvailable = false;
        emit serviceAvailableChanged(m_serviceAvailable);
        return;
    }
    
    // Try to call a simple method to check availability
    QDBusReply<QString> reply = m_dbusInterface->call("GetHealthStatus");
    m_serviceAvailable = reply.isValid();
    emit serviceAvailableChanged(m_serviceAvailable);
}

QString DBusInterfaceWrapper::logEvent(const QString& eventData) {
    if (!m_serviceAvailable || !m_dbusInterface) {
        emit eventLogFailed("Service not available");
        return QString();
    }
    
    QDBusPendingCall call = m_dbusInterface->asyncCall("LogEvent", eventData);
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &DBusInterfaceWrapper::onLogEventFinished);
    
    return QString(); // Will be returned via signal
}

QString DBusInterfaceWrapper::queryEvents(const QString& queryParams) {
    if (!m_serviceAvailable || !m_dbusInterface) {
        emit queryFailed("Service not available");
        return QString();
    }
    
    QDBusPendingCall call = m_dbusInterface->asyncCall("QueryEvents", queryParams);
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &DBusInterfaceWrapper::onQueryEventsFinished);
    
    return QString(); // Will be returned via signal
}

QString DBusInterfaceWrapper::getEventStatistics(const QString& queryParams) {
    if (!m_serviceAvailable || !m_dbusInterface) {
        emit statisticsFailed("Service not available");
        return QString();
    }
    
    QDBusPendingCall call = m_dbusInterface->asyncCall("GetEventStatistics", queryParams);
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &DBusInterfaceWrapper::onGetEventStatisticsFinished);
    
    return QString(); // Will be returned via signal
}

QString DBusInterfaceWrapper::exportAuditLog(const QString& timeRange, const QString& format) {
    if (!m_serviceAvailable || !m_dbusInterface) {
        emit exportFailed("Service not available");
        return QString();
    }
    
    QDBusPendingCall call = m_dbusInterface->asyncCall("ExportAuditLog", timeRange, format);
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &DBusInterfaceWrapper::onExportAuditLogFinished);
    
    return QString(); // Will be returned via signal
}

bool DBusInterfaceWrapper::verifyLogIntegrity() {
    if (!m_serviceAvailable || !m_dbusInterface) {
        emit integrityVerified(false);
        return false;
    }
    
    QDBusPendingCall call = m_dbusInterface->asyncCall("VerifyLogIntegrity");
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &DBusInterfaceWrapper::onVerifyLogIntegrityFinished);
    
    return false; // Will be returned via signal
}

QString DBusInterfaceWrapper::getHealthStatus() {
    if (!m_serviceAvailable || !m_dbusInterface) {
        return QString();
    }
    
    QDBusPendingCall call = m_dbusInterface->asyncCall("GetHealthStatus");
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &DBusInterfaceWrapper::onGetHealthStatusFinished);
    
    return QString(); // Will be returned via signal
}

void DBusInterfaceWrapper::refreshConnection() {
    disconnectFromService();
    connectToService();
    checkServiceAvailability();
}

void DBusInterfaceWrapper::onServiceRegistered(const QString& serviceName) {
    if (serviceName == "org.milos.AuditService") {
        checkServiceAvailability();
    }
}

void DBusInterfaceWrapper::onServiceUnregistered(const QString& serviceName) {
    if (serviceName == "org.milos.AuditService") {
        m_serviceAvailable = false;
        emit serviceAvailableChanged(m_serviceAvailable);
    }
}

void DBusInterfaceWrapper::onEventLogged(const QString& eventId) {
    emit eventLogged(eventId);
}

void DBusInterfaceWrapper::onIntegrityViolationDetected() {
    emit integrityViolationDetected();
}

void DBusInterfaceWrapper::onLogEventFinished(QDBusPendingCallWatcher* watcher) {
    QDBusPendingReply<QString> reply = *watcher;
    watcher->deleteLater();
    
    if (reply.isError()) {
        emit eventLogFailed(reply.error().message());
    } else {
        QString eventId = reply.value();
        emit eventLogged(eventId);
    }
}

void DBusInterfaceWrapper::onQueryEventsFinished(QDBusPendingCallWatcher* watcher) {
    QDBusPendingReply<QString> reply = *watcher;
    watcher->deleteLater();
    
    if (reply.isError()) {
        emit queryFailed(reply.error().message());
    } else {
        QString results = reply.value();
        emit queryCompleted(results);
    }
}

void DBusInterfaceWrapper::onGetEventStatisticsFinished(QDBusPendingCallWatcher* watcher) {
    QDBusPendingReply<QString> reply = *watcher;
    watcher->deleteLater();
    
    if (reply.isError()) {
        emit statisticsFailed(reply.error().message());
    } else {
        QString statistics = reply.value();
        emit statisticsUpdated(statistics);
    }
}

void DBusInterfaceWrapper::onExportAuditLogFinished(QDBusPendingCallWatcher* watcher) {
    QDBusPendingReply<QString> reply = *watcher;
    watcher->deleteLater();
    
    if (reply.isError()) {
        emit exportFailed(reply.error().message());
    } else {
        // Extract format from the call if possible, or use default
        QString data = reply.value();
        QString format = "json"; // Default, could be passed as parameter
        emit exportCompleted(data, format);
    }
}

void DBusInterfaceWrapper::onVerifyLogIntegrityFinished(QDBusPendingCallWatcher* watcher) {
    QDBusPendingReply<bool> reply = *watcher;
    watcher->deleteLater();
    
    if (reply.isError()) {
        emit integrityVerified(false);
    } else {
        bool verified = reply.value();
        emit integrityVerified(verified);
        if (!verified) {
            emit integrityViolationDetected();
        }
    }
}

void DBusInterfaceWrapper::onGetHealthStatusFinished(QDBusPendingCallWatcher* watcher) {
    QDBusPendingReply<QString> reply = *watcher;
    watcher->deleteLater();
    
    if (reply.isError()) {
        m_serviceAvailable = false;
        emit serviceAvailableChanged(m_serviceAvailable);
    } else {
        QString healthStatus = reply.value();
        emit healthStatusUpdated(healthStatus);
    }
}

