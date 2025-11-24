#include "data_guard_manager.h"
#include "dbus_interface_wrapper.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

DataGuardManager::DataGuardManager(QObject* parent)
    : QObject(parent)
    , m_dbusInterface(nullptr)
    , m_loadingStatus(false)
    , m_configuringPolicy(false)
    , m_statusPollTimer(new QTimer(this))
{
    m_statusPollTimer->setInterval(5000); // Poll every 5 seconds
    m_statusPollTimer->setSingleShot(false);
    connect(m_statusPollTimer, &QTimer::timeout, this, &DataGuardManager::onStatusPollTimeout);
}

DataGuardManager::~DataGuardManager() {
    stopStatusPolling();
}

void DataGuardManager::setDBusInterface(DBusInterfaceWrapper* dbusInterface) {
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
        connect(m_dbusInterface, &DBusInterfaceWrapper::transmissionStatusUpdated,
                this, &DataGuardManager::onTransmissionStatusUpdated);
        connect(m_dbusInterface, &DBusInterfaceWrapper::transmissionStatusFailed,
                this, &DataGuardManager::onTransmissionStatusFailed);
        connect(m_dbusInterface, &DBusInterfaceWrapper::blockedTransmissionsUpdated,
                this, &DataGuardManager::onBlockedTransmissionsUpdated);
        connect(m_dbusInterface, &DBusInterfaceWrapper::blockedTransmissionsFailed,
                this, &DataGuardManager::onBlockedTransmissionsFailed);
        connect(m_dbusInterface, &DBusInterfaceWrapper::policyConfigured,
                this, &DataGuardManager::onPolicyConfigured);
        connect(m_dbusInterface, &DBusInterfaceWrapper::policyConfigurationFailed,
                this, &DataGuardManager::onPolicyConfigurationFailed);
        connect(m_dbusInterface, &DBusInterfaceWrapper::policyStatusUpdated,
                this, &DataGuardManager::onPolicyStatusUpdated);
        connect(m_dbusInterface, &DBusInterfaceWrapper::policyStatusFailed,
                this, &DataGuardManager::onPolicyStatusFailed);
        connect(m_dbusInterface, &DBusInterfaceWrapper::healthStatusUpdated,
                this, &DataGuardManager::onHealthStatusUpdated);
        connect(m_dbusInterface, &DBusInterfaceWrapper::transmissionBlocked,
                this, &DataGuardManager::onTransmissionBlocked);
        connect(m_dbusInterface, &DBusInterfaceWrapper::policyViolationDetected,
                this, &DataGuardManager::onPolicyViolationDetected);
    }
}

void DataGuardManager::refreshTransmissionStatus() {
    if (!m_dbusInterface || m_loadingStatus) {
        return;
    }
    
    m_loadingStatus = true;
    emit loadingStatusChanged(m_loadingStatus);
    
    m_dbusInterface->getTransmissionStatus();
}

void DataGuardManager::refreshBlockedTransmissions() {
    if (!m_dbusInterface || m_loadingStatus) {
        return;
    }
    
    m_loadingStatus = true;
    emit loadingStatusChanged(m_loadingStatus);
    
    m_dbusInterface->getBlockedTransmissions();
}

void DataGuardManager::configurePolicy(const QVariantMap& policy) {
    if (!m_dbusInterface || m_configuringPolicy) {
        return;
    }
    
    m_configuringPolicy = true;
    emit configuringPolicyChanged(m_configuringPolicy);
    
    // Convert QVariantMap to JSON
    QJsonObject jsonPolicy;
    for (auto it = policy.begin(); it != policy.end(); ++it) {
        jsonPolicy[it.key()] = QJsonValue::fromVariant(it.value());
    }
    QJsonDocument doc(jsonPolicy);
    QString policyJson = doc.toJson(QJsonDocument::Compact);
    
    m_dbusInterface->configurePolicy(policyJson);
}

void DataGuardManager::getPolicyStatus(const QString& policyId) {
    if (!m_dbusInterface) {
        return;
    }
    
    m_dbusInterface->getPolicyStatus(policyId);
}

void DataGuardManager::refreshHealthStatus() {
    if (!m_dbusInterface) {
        return;
    }
    
    m_dbusInterface->getHealthStatus();
}

void DataGuardManager::onTransmissionStatusUpdated(const QString& status) {
    m_loadingStatus = false;
    emit loadingStatusChanged(m_loadingStatus);
    
    parseTransmissionStatus(status);
    emit transmissionStatusUpdated(m_transmissionStatus);
}

void DataGuardManager::onTransmissionStatusFailed(const QString& error) {
    m_loadingStatus = false;
    emit loadingStatusChanged(m_loadingStatus);
    emit transmissionStatusFailed(error);
}

void DataGuardManager::onBlockedTransmissionsUpdated(const QString& blocked) {
    m_loadingStatus = false;
    emit loadingStatusChanged(m_loadingStatus);
    
    parseBlockedTransmissions(blocked);
    emit blockedTransmissionsUpdated(m_blockedTransmissions);
}

void DataGuardManager::onBlockedTransmissionsFailed(const QString& error) {
    m_loadingStatus = false;
    emit loadingStatusChanged(m_loadingStatus);
    emit blockedTransmissionsFailed(error);
}

void DataGuardManager::onPolicyConfigured(bool success) {
    m_configuringPolicy = false;
    emit configuringPolicyChanged(m_configuringPolicy);
    emit policyConfigured(success);
    if (success) {
        // Refresh status after policy configuration
        refreshTransmissionStatus();
    }
}

void DataGuardManager::onPolicyConfigurationFailed(const QString& error) {
    m_configuringPolicy = false;
    emit configuringPolicyChanged(m_configuringPolicy);
    emit policyConfigurationFailed(error);
}

void DataGuardManager::onPolicyStatusUpdated(const QString& status) {
    QVariantMap policyStatus = jsonToVariantMap(status);
    emit policyStatusUpdated(policyStatus);
}

void DataGuardManager::onPolicyStatusFailed(const QString& error) {
    emit policyStatusFailed(error);
}

void DataGuardManager::onHealthStatusUpdated(const QString& healthStatus) {
    parseHealthStatus(healthStatus);
    emit healthStatusUpdated(m_healthStatus);
}

void DataGuardManager::onTransmissionBlocked(const QString& transmissionInfo) {
    QVariantMap info = jsonToVariantMap(transmissionInfo);
    emit transmissionBlocked(info);
    // Refresh blocked transmissions list
    refreshBlockedTransmissions();
}

void DataGuardManager::onPolicyViolationDetected(const QString& violationInfo) {
    QVariantMap info = jsonToVariantMap(violationInfo);
    emit policyViolationDetected(info);
}

void DataGuardManager::onStatusPollTimeout() {
    refreshTransmissionStatus();
    refreshHealthStatus();
}

void DataGuardManager::parseTransmissionStatus(const QString& jsonResult) {
    m_transmissionStatus = jsonToVariantMap(jsonResult);
    emit transmissionStatusChanged(m_transmissionStatus);
}

void DataGuardManager::parseBlockedTransmissions(const QString& jsonResult) {
    m_blockedTransmissions.clear();
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonResult.toUtf8());
    if (doc.isNull() || !doc.isArray()) {
        return;
    }
    
    QJsonArray array = doc.array();
    for (const QJsonValue& value : array) {
        if (value.isString()) {
            // If it's a JSON string, parse it
            QJsonDocument itemDoc = QJsonDocument::fromJson(value.toString().toUtf8());
            if (!itemDoc.isNull() && itemDoc.isObject()) {
                QVariantMap item;
                QJsonObject obj = itemDoc.object();
                for (auto it = obj.begin(); it != obj.end(); ++it) {
                    item[it.key()] = it.value().toVariant();
                }
                m_blockedTransmissions.append(item);
            }
        } else if (value.isObject()) {
            QVariantMap item;
            QJsonObject obj = value.toObject();
            for (auto it = obj.begin(); it != obj.end(); ++it) {
                item[it.key()] = it.value().toVariant();
            }
            m_blockedTransmissions.append(item);
        }
    }
    
    emit blockedTransmissionsChanged(m_blockedTransmissions);
}

void DataGuardManager::parseHealthStatus(const QString& jsonResult) {
    m_healthStatus = jsonToVariantMap(jsonResult);
    emit healthStatusChanged(m_healthStatus);
}

void DataGuardManager::parsePolicyStatus(const QString& jsonResult) {
    QVariantMap status = jsonToVariantMap(jsonResult);
    emit policyStatusUpdated(status);
}

QVariantMap DataGuardManager::jsonToVariantMap(const QString& jsonString) {
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (doc.isNull() || !doc.isObject()) {
        return QVariantMap();
    }
    
    QVariantMap result;
    QJsonObject obj = doc.object();
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        result[it.key()] = it.value().toVariant();
    }
    return result;
}

void DataGuardManager::startStatusPolling() {
    if (!m_statusPollTimer->isActive()) {
        m_statusPollTimer->start();
        refreshTransmissionStatus();
        refreshHealthStatus();
    }
}

void DataGuardManager::stopStatusPolling() {
    if (m_statusPollTimer->isActive()) {
        m_statusPollTimer->stop();
    }
}

