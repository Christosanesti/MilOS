#include "update_manager.h"
#include "dbus_interface_wrapper.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

UpdateManager::UpdateManager(QObject* parent)
    : QObject(parent)
    , m_dbusInterface(nullptr)
    , m_checkingUpdates(false)
    , m_applyingUpdates(false)
    , m_updateProgress(0)
    , m_statusPollTimer(new QTimer(this))
{
    m_statusPollTimer->setInterval(1000); // Poll every second during updates
    m_statusPollTimer->setSingleShot(false);
    connect(m_statusPollTimer, &QTimer::timeout, this, &UpdateManager::onStatusPollTimeout);
}

UpdateManager::~UpdateManager() {
    stopStatusPolling();
}

void UpdateManager::setDBusInterface(DBusInterfaceWrapper* dbusInterface) {
    if (m_dbusInterface == dbusInterface) {
        return;
    }
    
    // Disconnect old interface
    if (m_dbusInterface) {
        disconnect(m_dbusInterface, nullptr, this, nullptr);
    }
    
    m_dbusInterface = dbusInterface;
    
    // Connect to new interface
    if (m_dbusInterface) {
        connect(m_dbusInterface, &DBusInterfaceWrapper::updateCheckCompleted,
                this, &UpdateManager::onUpdateCheckCompleted);
        connect(m_dbusInterface, &DBusInterfaceWrapper::updateCheckFailed,
                this, &UpdateManager::onUpdateCheckFailed);
        connect(m_dbusInterface, &DBusInterfaceWrapper::updateStarted,
                this, &UpdateManager::onUpdateStarted);
        connect(m_dbusInterface, &DBusInterfaceWrapper::updateProgress,
                this, &UpdateManager::onUpdateProgress);
        connect(m_dbusInterface, &DBusInterfaceWrapper::updateCompleted,
                this, &UpdateManager::onUpdateCompleted);
        connect(m_dbusInterface, &DBusInterfaceWrapper::updateError,
                this, &UpdateManager::onUpdateError);
        connect(m_dbusInterface, &DBusInterfaceWrapper::rollbackCompleted,
                this, &UpdateManager::onRollbackCompleted);
        connect(m_dbusInterface, &DBusInterfaceWrapper::rollbackFailed,
                this, &UpdateManager::onRollbackFailed);
        connect(m_dbusInterface, &DBusInterfaceWrapper::historyUpdated,
                this, &UpdateManager::onHistoryUpdated);
    }
}

void UpdateManager::checkForUpdates() {
    if (!m_dbusInterface || m_checkingUpdates) {
        return;
    }
    
    m_checkingUpdates = true;
    emit checkingUpdatesChanged(m_checkingUpdates);
    
    m_dbusInterface->checkUpdates();
}

void UpdateManager::applySelectedUpdates(const QStringList& packageList) {
    if (!m_dbusInterface || m_applyingUpdates || packageList.isEmpty()) {
        return;
    }
    
    m_applyingUpdates = true;
    m_updateProgress = 0;
    emit applyingUpdatesChanged(m_applyingUpdates);
    emit updateProgressChanged(m_updateProgress);
    
    QString updateId = m_dbusInterface->applyUpdates(packageList);
    if (!updateId.isEmpty()) {
        m_currentUpdateId = updateId;
        emit currentUpdateIdChanged(m_currentUpdateId);
        startStatusPolling();
    }
}

void UpdateManager::performRollback(const QString& updateId) {
    if (!m_dbusInterface || updateId.isEmpty()) {
        return;
    }
    
    m_dbusInterface->rollbackUpdate(updateId);
}

void UpdateManager::refreshHistory() {
    if (!m_dbusInterface) {
        return;
    }
    
    m_dbusInterface->getUpdateHistory();
}

void UpdateManager::cancelUpdate() {
    // Note: Update service may not support cancellation
    // This is a placeholder for future implementation
    stopStatusPolling();
    m_applyingUpdates = false;
    m_updateProgress = 0;
    emit applyingUpdatesChanged(m_applyingUpdates);
    emit updateProgressChanged(m_updateProgress);
}

void UpdateManager::onUpdateCheckCompleted(const QString& result) {
    m_checkingUpdates = false;
    emit checkingUpdatesChanged(m_checkingUpdates);
    
    parseUpdateList(result);
    emit updateCheckCompleted(m_availableUpdates);
}

void UpdateManager::onUpdateCheckFailed(const QString& error) {
    m_checkingUpdates = false;
    emit checkingUpdatesChanged(m_checkingUpdates);
    emit updateCheckFailed(error);
}

void UpdateManager::onUpdateStarted(const QString& updateId) {
    m_currentUpdateId = updateId;
    emit currentUpdateIdChanged(m_currentUpdateId);
    emit updateStarted(updateId);
    startStatusPolling();
}

void UpdateManager::onUpdateProgress(const QString& updateId, int progress) {
    if (updateId == m_currentUpdateId) {
        m_updateProgress = progress;
        emit updateProgressChanged(m_updateProgress);
        emit updateProgressUpdate(updateId, progress);
    }
}

void UpdateManager::onUpdateCompleted(const QString& updateId, bool success) {
    stopStatusPolling();
    m_applyingUpdates = false;
    m_updateProgress = success ? 100 : 0;
    emit applyingUpdatesChanged(m_applyingUpdates);
    emit updateProgressChanged(m_updateProgress);
    emit updateCompleted(updateId, success);
    
    // Refresh history after update completes
    refreshHistory();
}

void UpdateManager::onUpdateError(const QString& updateId, const QString& error) {
    stopStatusPolling();
    m_applyingUpdates = false;
    m_updateProgress = 0;
    emit applyingUpdatesChanged(m_applyingUpdates);
    emit updateProgressChanged(m_updateProgress);
    emit updateFailed(updateId, error);
}

void UpdateManager::onRollbackCompleted(const QString& updateId, bool success) {
    emit rollbackCompleted(updateId, success);
    
    // Refresh history after rollback
    refreshHistory();
}

void UpdateManager::onRollbackFailed(const QString& updateId, const QString& error) {
    emit rollbackFailed(updateId, error);
}

void UpdateManager::onHistoryUpdated(const QString& history) {
    parseHistory(history);
    emit updateHistoryChanged(m_updateHistory);
}

void UpdateManager::onStatusPollTimeout() {
    if (m_dbusInterface && !m_currentUpdateId.isEmpty()) {
        m_dbusInterface->getUpdateStatus(m_currentUpdateId);
    }
}

void UpdateManager::parseUpdateList(const QString& jsonResult) {
    m_availableUpdates.clear();
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonResult.toUtf8());
    if (doc.isNull()) {
        return;
    }
    
    if (doc.isArray()) {
        QJsonArray array = doc.array();
        for (const QJsonValue& value : array) {
            if (value.isObject()) {
                m_availableUpdates.append(value.toObject().toVariantMap());
            } else if (value.isString()) {
                QVariantMap package;
                package["name"] = value.toString();
                m_availableUpdates.append(package);
            }
        }
    } else if (doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.contains("packages") && obj["packages"].isArray()) {
            QJsonArray array = obj["packages"].toArray();
            for (const QJsonValue& value : array) {
                if (value.isObject()) {
                    m_availableUpdates.append(value.toObject().toVariantMap());
                }
            }
        }
    }
}

void UpdateManager::parseHistory(const QString& jsonResult) {
    m_updateHistory.clear();
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonResult.toUtf8());
    if (doc.isNull() || !doc.isArray()) {
        return;
    }
    
    QJsonArray array = doc.array();
    for (const QJsonValue& value : array) {
        if (value.isObject()) {
            m_updateHistory.append(value.toObject().toVariantMap());
        }
    }
}

void UpdateManager::startStatusPolling() {
    if (!m_statusPollTimer->isActive()) {
        m_statusPollTimer->start();
    }
}

void UpdateManager::stopStatusPolling() {
    if (m_statusPollTimer->isActive()) {
        m_statusPollTimer->stop();
    }
}

