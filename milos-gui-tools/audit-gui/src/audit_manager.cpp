#include "audit_manager.h"
#include "dbus_interface_wrapper.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDateTime>
#include <QDebug>

AuditManager::AuditManager(QObject* parent)
    : QObject(parent)
    , m_dbusInterface(nullptr)
    , m_queryingEvents(false)
    , m_verifyingIntegrity(false)
    , m_exportingLogs(false)
    , m_integrityStatus(true)
    , m_healthPollTimer(new QTimer(this))
{
    m_healthPollTimer->setInterval(5000); // Poll every 5 seconds
    m_healthPollTimer->setSingleShot(false);
    connect(m_healthPollTimer, &QTimer::timeout, this, &AuditManager::onHealthPollTimeout);
}

AuditManager::~AuditManager() {
    stopHealthPolling();
}

void AuditManager::setDBusInterface(DBusInterfaceWrapper* dbusInterface) {
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
        connect(m_dbusInterface, &DBusInterfaceWrapper::queryCompleted,
                this, &AuditManager::onQueryCompleted);
        connect(m_dbusInterface, &DBusInterfaceWrapper::queryFailed,
                this, &AuditManager::onQueryFailed);
        connect(m_dbusInterface, &DBusInterfaceWrapper::statisticsUpdated,
                this, &AuditManager::onStatisticsUpdated);
        connect(m_dbusInterface, &DBusInterfaceWrapper::statisticsFailed,
                this, &AuditManager::onStatisticsFailed);
        connect(m_dbusInterface, &DBusInterfaceWrapper::exportCompleted,
                this, &AuditManager::onExportCompleted);
        connect(m_dbusInterface, &DBusInterfaceWrapper::exportFailed,
                this, &AuditManager::onExportFailed);
        connect(m_dbusInterface, &DBusInterfaceWrapper::integrityVerified,
                this, &AuditManager::onIntegrityVerified);
        connect(m_dbusInterface, &DBusInterfaceWrapper::integrityViolationDetected,
                this, &AuditManager::onIntegrityViolationDetected);
        connect(m_dbusInterface, &DBusInterfaceWrapper::healthStatusUpdated,
                this, &AuditManager::onHealthStatusUpdated);
    }
}

void AuditManager::queryEvents(const QVariantMap& filters) {
    if (!m_dbusInterface || m_queryingEvents) {
        return;
    }
    
    m_queryingEvents = true;
    emit queryingEventsChanged(m_queryingEvents);
    
    // Convert filters to JSON
    QJsonObject jsonFilters;
    for (auto it = filters.begin(); it != filters.end(); ++it) {
        jsonFilters[it.key()] = QJsonValue::fromVariant(it.value());
    }
    QJsonDocument doc(jsonFilters);
    QString queryParams = doc.toJson(QJsonDocument::Compact);
    
    m_dbusInterface->queryEvents(queryParams);
}

void AuditManager::refreshStatistics(const QVariantMap& filters) {
    if (!m_dbusInterface) {
        return;
    }
    
    // Convert filters to JSON
    QJsonObject jsonFilters;
    for (auto it = filters.begin(); it != filters.end(); ++it) {
        jsonFilters[it.key()] = QJsonValue::fromVariant(it.value());
    }
    QJsonDocument doc(jsonFilters);
    QString queryParams = doc.toJson(QJsonDocument::Compact);
    
    m_dbusInterface->getEventStatistics(queryParams);
}

void AuditManager::exportLogs(const QVariantMap& timeRange, const QString& format) {
    if (!m_dbusInterface || m_exportingLogs) {
        return;
    }
    
    m_exportingLogs = true;
    emit exportingLogsChanged(m_exportingLogs);
    
    // Convert timeRange to JSON
    QJsonObject jsonTimeRange;
    for (auto it = timeRange.begin(); it != timeRange.end(); ++it) {
        jsonTimeRange[it.key()] = QJsonValue::fromVariant(it.value());
    }
    QJsonDocument doc(jsonTimeRange);
    QString timeRangeJson = doc.toJson(QJsonDocument::Compact);
    
    m_dbusInterface->exportAuditLog(timeRangeJson, format);
}

void AuditManager::verifyIntegrity() {
    if (!m_dbusInterface || m_verifyingIntegrity) {
        return;
    }
    
    m_verifyingIntegrity = true;
    emit verifyingIntegrityChanged(m_verifyingIntegrity);
    
    m_dbusInterface->verifyLogIntegrity();
}

void AuditManager::refreshHealthStatus() {
    if (!m_dbusInterface) {
        return;
    }
    
    m_dbusInterface->getHealthStatus();
}

QVariantMap AuditManager::getEventDetails(const QString& eventId) {
    // Find event in current list
    for (const QVariant& variant : m_eventList) {
        QVariantMap event = variant.toMap();
        if (event["event_id"].toString() == eventId) {
            return event;
        }
    }
    return QVariantMap();
}

void AuditManager::onQueryCompleted(const QString& results) {
    m_queryingEvents = false;
    emit queryingEventsChanged(m_queryingEvents);
    
    parseEventList(results);
    emit queryCompleted(m_eventList);
}

void AuditManager::onQueryFailed(const QString& error) {
    m_queryingEvents = false;
    emit queryingEventsChanged(m_queryingEvents);
    emit queryFailed(error);
}

void AuditManager::onStatisticsUpdated(const QString& statistics) {
    parseStatistics(statistics);
    emit statisticsUpdated(m_statistics);
}

void AuditManager::onStatisticsFailed(const QString& error) {
    emit statisticsFailed(error);
}

void AuditManager::onExportCompleted(const QString& data, const QString& format) {
    m_exportingLogs = false;
    emit exportingLogsChanged(m_exportingLogs);
    
    // Save exported data to file
    QString fileName = QString("audit_log_export_%1.%2")
                       .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"))
                       .arg(format);
    QString filePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/" + fileName;
    
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << data;
        file.close();
        emit exportCompleted(filePath, format);
    } else {
        emit exportFailed("Failed to save export file");
    }
}

void AuditManager::onExportFailed(const QString& error) {
    m_exportingLogs = false;
    emit exportingLogsChanged(m_exportingLogs);
    emit exportFailed(error);
}

void AuditManager::onIntegrityVerified(bool verified) {
    m_verifyingIntegrity = false;
    emit verifyingIntegrityChanged(m_verifyingIntegrity);
    
    m_integrityStatus = verified;
    emit integrityStatusChanged(m_integrityStatus);
    emit integrityVerified(verified);
}

void AuditManager::onIntegrityViolationDetected() {
    m_integrityStatus = false;
    emit integrityStatusChanged(m_integrityStatus);
    emit integrityViolationDetected();
}

void AuditManager::onHealthStatusUpdated(const QString& healthStatus) {
    parseHealthStatus(healthStatus);
    emit healthStatusUpdated(jsonToVariantMap(healthStatus));
}

void AuditManager::onHealthPollTimeout() {
    refreshHealthStatus();
}

void AuditManager::parseEventList(const QString& jsonResult) {
    m_eventList.clear();
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonResult.toUtf8());
    if (doc.isNull() || !doc.isArray()) {
        return;
    }
    
    QJsonArray array = doc.array();
    for (const QJsonValue& value : array) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();
            QVariantMap event;
            for (auto it = obj.begin(); it != obj.end(); ++it) {
                event[it.key()] = it.value().toVariant();
            }
            m_eventList.append(event);
        }
    }
    
    emit eventListChanged(m_eventList);
}

void AuditManager::parseStatistics(const QString& jsonResult) {
    m_statistics = jsonToVariantMap(jsonResult);
    emit statisticsChanged(m_statistics);
}

void AuditManager::parseHealthStatus(const QString& jsonResult) {
    m_healthStatus = jsonResult;
    emit healthStatusChanged(m_healthStatus);
}

void AuditManager::startHealthPolling() {
    if (!m_healthPollTimer->isActive()) {
        m_healthPollTimer->start();
        refreshHealthStatus();
    }
}

void AuditManager::stopHealthPolling() {
    if (m_healthPollTimer->isActive()) {
        m_healthPollTimer->stop();
    }
}

QVariantMap AuditManager::jsonToVariantMap(const QString& jsonString) {
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

