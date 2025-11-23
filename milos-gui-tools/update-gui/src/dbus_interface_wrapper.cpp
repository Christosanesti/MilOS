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
        "org.milos.UpdateService",
        "/org/milos/UpdateService",
        "org.milos.UpdateService",
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
        "org.milos.UpdateService",
        "/org/milos/UpdateService",
        "org.milos.UpdateService",
        "UpdateAvailable",
        this,
        SLOT(onUpdateAvailable(QString))
    );
    
    m_connection.connect(
        "org.milos.UpdateService",
        "/org/milos/UpdateService",
        "org.milos.UpdateService",
        "UpdateStarted",
        this,
        SLOT(onUpdateStarted(QString))
    );
    
    m_connection.connect(
        "org.milos.UpdateService",
        "/org/milos/UpdateService",
        "org.milos.UpdateService",
        "UpdateCompleted",
        this,
        SLOT(onUpdateCompleted(QString, QString))
    );
    
    m_connection.connect(
        "org.milos.UpdateService",
        "/org/milos/UpdateService",
        "org.milos.UpdateService",
        "UpdateError",
        this,
        SLOT(onUpdateError(QString, QString))
    );
    
    m_serviceAvailable = true;
    emit serviceAvailableChanged(m_serviceAvailable);
}

void DBusInterfaceWrapper::disconnectFromService() {
    if (m_dbusInterface) {
        m_connection.disconnect(
            "org.milos.UpdateService",
            "/org/milos/UpdateService",
            "org.milos.UpdateService",
            "UpdateAvailable",
            this,
            SLOT(onUpdateAvailable(QString))
        );
        
        m_connection.disconnect(
            "org.milos.UpdateService",
            "/org/milos/UpdateService",
            "org.milos.UpdateService",
            "UpdateStarted",
            this,
            SLOT(onUpdateStarted(QString))
        );
        
        m_connection.disconnect(
            "org.milos.UpdateService",
            "/org/milos/UpdateService",
            "org.milos.UpdateService",
            "UpdateCompleted",
            this,
            SLOT(onUpdateCompleted(QString, QString))
        );
        
        m_connection.disconnect(
            "org.milos.UpdateService",
            "/org/milos/UpdateService",
            "org.milos.UpdateService",
            "UpdateError",
            this,
            SLOT(onUpdateError(QString, QString))
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
    QDBusReply<QString> reply = m_dbusInterface->call("GetUpdateStatus");
    m_serviceAvailable = reply.isValid();
    emit serviceAvailableChanged(m_serviceAvailable);
}

bool DBusInterfaceWrapper::checkUpdates() {
    if (!m_serviceAvailable || !m_dbusInterface) {
        emit updateCheckFailed("Service not available");
        return false;
    }
    
    QDBusPendingCall call = m_dbusInterface->asyncCall("CheckUpdates");
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &DBusInterfaceWrapper::onCheckUpdatesFinished);
    
    return true;
}

QString DBusInterfaceWrapper::applyUpdates(const QStringList& packageList) {
    if (!m_serviceAvailable || !m_dbusInterface) {
        emit updateError("", "Service not available");
        return QString();
    }
    
    // Convert package list to JSON
    QJsonArray jsonArray;
    for (const QString& package : packageList) {
        jsonArray.append(package);
    }
    QJsonDocument doc(jsonArray);
    QString packageListJson = doc.toJson(QJsonDocument::Compact);
    
    QDBusPendingCall call = m_dbusInterface->asyncCall("ApplyUpdates", packageListJson);
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &DBusInterfaceWrapper::onApplyUpdatesFinished);
    
    return QString(); // Will be set when call completes
}

bool DBusInterfaceWrapper::rollbackUpdate(const QString& updateId) {
    if (!m_serviceAvailable || !m_dbusInterface) {
        emit rollbackFailed(updateId, "Service not available");
        return false;
    }
    
    QDBusPendingCall call = m_dbusInterface->asyncCall("RollbackUpdate", updateId);
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &DBusInterfaceWrapper::onRollbackUpdateFinished);
    
    return true;
}

QString DBusInterfaceWrapper::getUpdateStatus(const QString& updateId) {
    if (!m_serviceAvailable || !m_dbusInterface) {
        return QString();
    }
    
    QDBusPendingCall call = m_dbusInterface->asyncCall("GetUpdateStatus", updateId);
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &DBusInterfaceWrapper::onGetUpdateStatusFinished);
    
    return QString(); // Will be returned via signal
}

QString DBusInterfaceWrapper::getUpdateHistory() {
    if (!m_serviceAvailable || !m_dbusInterface) {
        return QString();
    }
    
    QDBusPendingCall call = m_dbusInterface->asyncCall("GetUpdateHistory");
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &DBusInterfaceWrapper::onGetUpdateHistoryFinished);
    
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
    if (serviceName == "org.milos.UpdateService") {
        checkServiceAvailability();
    }
}

void DBusInterfaceWrapper::onServiceUnregistered(const QString& serviceName) {
    if (serviceName == "org.milos.UpdateService") {
        m_serviceAvailable = false;
        emit serviceAvailableChanged(m_serviceAvailable);
    }
}

void DBusInterfaceWrapper::onUpdateAvailable(const QString& packageInfo) {
    parseUpdateList(packageInfo);
    emit availableUpdatesChanged(m_availableUpdates);
}

void DBusInterfaceWrapper::onUpdateStarted(const QString& updateId) {
    m_currentUpdateId = updateId;
    m_currentStatus = "In Progress";
    emit currentStatusChanged(m_currentStatus);
    emit updateStarted(updateId);
}

void DBusInterfaceWrapper::onUpdateCompleted(const QString& updateId, const QString& status) {
    m_currentStatus = status;
    emit currentStatusChanged(m_currentStatus);
    emit updateCompleted(updateId, status == "COMPLETED");
}

void DBusInterfaceWrapper::onUpdateError(const QString& updateId, const QString& error) {
    m_currentStatus = "Error: " + error;
    emit currentStatusChanged(m_currentStatus);
    emit updateError(updateId, error);
}

void DBusInterfaceWrapper::onCheckUpdatesFinished(QDBusPendingCallWatcher* watcher) {
    QDBusPendingReply<QString> reply = *watcher;
    watcher->deleteLater();
    
    if (reply.isError()) {
        emit updateCheckFailed(reply.error().message());
    } else {
        QString result = reply.value();
        parseUpdateList(result);
        emit updateCheckCompleted(result);
        emit availableUpdatesChanged(m_availableUpdates);
    }
}

void DBusInterfaceWrapper::onApplyUpdatesFinished(QDBusPendingCallWatcher* watcher) {
    QDBusPendingReply<QString> reply = *watcher;
    watcher->deleteLater();
    
    if (reply.isError()) {
        emit updateError("", reply.error().message());
    } else {
        QString updateId = reply.value();
        m_currentUpdateId = updateId;
        emit updateStarted(updateId);
    }
}

void DBusInterfaceWrapper::onRollbackUpdateFinished(QDBusPendingCallWatcher* watcher) {
    QDBusPendingReply<bool> reply = *watcher;
    watcher->deleteLater();
    
    QString updateId = m_currentUpdateId;
    if (reply.isError()) {
        emit rollbackFailed(updateId, reply.error().message());
    } else {
        bool success = reply.value();
        if (success) {
            emit rollbackCompleted(updateId, true);
        } else {
            emit rollbackFailed(updateId, "Rollback failed");
        }
    }
}

void DBusInterfaceWrapper::onGetUpdateStatusFinished(QDBusPendingCallWatcher* watcher) {
    QDBusPendingReply<QString> reply = *watcher;
    watcher->deleteLater();
    
    if (!reply.isError()) {
        QString status = reply.value();
        m_currentStatus = status;
        emit currentStatusChanged(m_currentStatus);
    }
}

void DBusInterfaceWrapper::onGetUpdateHistoryFinished(QDBusPendingCallWatcher* watcher) {
    QDBusPendingReply<QString> reply = *watcher;
    watcher->deleteLater();
    
    if (!reply.isError()) {
        QString history = reply.value();
        emit historyUpdated(history);
    }
}

void DBusInterfaceWrapper::onGetHealthStatusFinished(QDBusPendingCallWatcher* watcher) {
    QDBusPendingReply<QString> reply = *watcher;
    watcher->deleteLater();
    
    // Health status can be used to update service availability
    if (reply.isError()) {
        m_serviceAvailable = false;
        emit serviceAvailableChanged(m_serviceAvailable);
    }
}

void DBusInterfaceWrapper::parseUpdateList(const QString& jsonResult) {
    m_availableUpdates.clear();
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonResult.toUtf8());
    if (doc.isNull() || !doc.isArray()) {
        return;
    }
    
    QJsonArray array = doc.array();
    for (const QJsonValue& value : array) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();
            if (obj.contains("name")) {
                m_availableUpdates.append(obj["name"].toString());
            }
        } else if (value.isString()) {
            m_availableUpdates.append(value.toString());
        }
    }
}

