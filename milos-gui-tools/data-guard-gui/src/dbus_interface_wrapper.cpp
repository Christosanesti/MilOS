#include "dbus_interface_wrapper.h"
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusReply>
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
        "org.milos.DataGuard",
        "/org/milos/DataGuard",
        "org.milos.DataGuard",
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
        "org.milos.DataGuard",
        "/org/milos/DataGuard",
        "org.milos.DataGuard",
        "TransmissionBlocked",
        this,
        SLOT(onTransmissionBlocked(QString))
    );
    
    m_connection.connect(
        "org.milos.DataGuard",
        "/org/milos/DataGuard",
        "org.milos.DataGuard",
        "PolicyViolationDetected",
        this,
        SLOT(onPolicyViolationDetected(QString))
    );
    
    m_serviceAvailable = true;
    emit serviceAvailableChanged(m_serviceAvailable);
}

void DBusInterfaceWrapper::disconnectFromService() {
    if (m_dbusInterface) {
        m_connection.disconnect(
            "org.milos.DataGuard",
            "/org/milos/DataGuard",
            "org.milos.DataGuard",
            "TransmissionBlocked",
            this,
            SLOT(onTransmissionBlocked(QString))
        );
        
        m_connection.disconnect(
            "org.milos.DataGuard",
            "/org/milos/DataGuard",
            "org.milos.DataGuard",
            "PolicyViolationDetected",
            this,
            SLOT(onPolicyViolationDetected(QString))
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

QString DBusInterfaceWrapper::getTransmissionStatus() {
    if (!m_serviceAvailable || !m_dbusInterface) {
        emit transmissionStatusFailed("Service not available");
        return QString();
    }
    
    QDBusPendingCall call = m_dbusInterface->asyncCall("GetTransmissionStatus");
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &DBusInterfaceWrapper::onGetTransmissionStatusFinished);
    
    return QString(); // Will be returned via signal
}

QString DBusInterfaceWrapper::getBlockedTransmissions() {
    if (!m_serviceAvailable || !m_dbusInterface) {
        emit blockedTransmissionsFailed("Service not available");
        return QString();
    }
    
    QDBusPendingCall call = m_dbusInterface->asyncCall("GetBlockedTransmissions");
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &DBusInterfaceWrapper::onGetBlockedTransmissionsFinished);
    
    return QString(); // Will be returned via signal
}

bool DBusInterfaceWrapper::configurePolicy(const QString& policy) {
    if (!m_serviceAvailable || !m_dbusInterface) {
        emit policyConfigurationFailed("Service not available");
        return false;
    }
    
    QDBusPendingCall call = m_dbusInterface->asyncCall("ConfigurePolicy", policy);
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &DBusInterfaceWrapper::onConfigurePolicyFinished);
    
    return false; // Will be returned via signal
}

QString DBusInterfaceWrapper::getPolicyStatus(const QString& policyId) {
    if (!m_serviceAvailable || !m_dbusInterface) {
        emit policyStatusFailed("Service not available");
        return QString();
    }
    
    QDBusPendingCall call = m_dbusInterface->asyncCall("GetPolicyStatus", policyId);
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &DBusInterfaceWrapper::onGetPolicyStatusFinished);
    
    return QString(); // Will be returned via signal
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
    if (serviceName == "org.milos.DataGuard") {
        checkServiceAvailability();
    }
}

void DBusInterfaceWrapper::onServiceUnregistered(const QString& serviceName) {
    if (serviceName == "org.milos.DataGuard") {
        m_serviceAvailable = false;
        emit serviceAvailableChanged(m_serviceAvailable);
    }
}

void DBusInterfaceWrapper::onTransmissionBlocked(const QString& transmissionInfo) {
    emit transmissionBlocked(transmissionInfo);
}

void DBusInterfaceWrapper::onPolicyViolationDetected(const QString& violationInfo) {
    emit policyViolationDetected(violationInfo);
}

void DBusInterfaceWrapper::onGetTransmissionStatusFinished(QDBusPendingCallWatcher* watcher) {
    QDBusPendingReply<QString> reply = *watcher;
    watcher->deleteLater();
    
    if (reply.isError()) {
        emit transmissionStatusFailed(reply.error().message());
    } else {
        QString status = reply.value();
        emit transmissionStatusUpdated(status);
    }
}

void DBusInterfaceWrapper::onGetBlockedTransmissionsFinished(QDBusPendingCallWatcher* watcher) {
    QDBusPendingReply<QStringList> reply = *watcher;
    watcher->deleteLater();
    
    if (reply.isError()) {
        emit blockedTransmissionsFailed(reply.error().message());
    } else {
        // Convert QStringList to JSON array string
        QStringList blocked = reply.value();
        QString json = "[";
        for (int i = 0; i < blocked.size(); ++i) {
            if (i > 0) json += ",";
            json += blocked[i];
        }
        json += "]";
        emit blockedTransmissionsUpdated(json);
    }
}

void DBusInterfaceWrapper::onConfigurePolicyFinished(QDBusPendingCallWatcher* watcher) {
    QDBusPendingReply<bool> reply = *watcher;
    watcher->deleteLater();
    
    if (reply.isError()) {
        emit policyConfigurationFailed(reply.error().message());
    } else {
        bool success = reply.value();
        emit policyConfigured(success);
        if (!success) {
            emit policyConfigurationFailed("Policy configuration failed");
        }
    }
}

void DBusInterfaceWrapper::onGetPolicyStatusFinished(QDBusPendingCallWatcher* watcher) {
    QDBusPendingReply<QString> reply = *watcher;
    watcher->deleteLater();
    
    if (reply.isError()) {
        emit policyStatusFailed(reply.error().message());
    } else {
        QString status = reply.value();
        emit policyStatusUpdated(status);
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

