#include "dbus_interface.h"
#include "config_parser.h"
#include "package_manager.h"
#include "signature_verifier.h"
#include "update_applier.h"
#include "rollback_manager.h"
#include "audit_logger.h"
#include <QDBusConnection>
#include <QDBusError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <iostream>

DBusInterface::DBusInterface(QObject* parent)
    : QObject(parent)
    , m_running(false)
    , m_initialized(false)
    , m_configParser(nullptr)
    , m_packageManager(nullptr)
    , m_signatureVerifier(nullptr)
    , m_updateApplier(nullptr)
    , m_rollbackManager(nullptr)
    , m_auditLogger(nullptr)
{
}

DBusInterface::~DBusInterface() {
    stop();
}

bool DBusInterface::initialize(
    ConfigParser* configParser,
    PackageManager* packageManager,
    SignatureVerifier* signatureVerifier,
    UpdateApplier* updateApplier,
    RollbackManager* rollbackManager,
    AuditLogger* auditLogger
) {
    if (m_initialized) {
        return true;
    }

    m_configParser = configParser;
    m_packageManager = packageManager;
    m_signatureVerifier = signatureVerifier;
    m_updateApplier = updateApplier;
    m_rollbackManager = rollbackManager;
    m_auditLogger = auditLogger;

    m_initialized = true;
    return true;
}

bool DBusInterface::start() {
    if (!m_initialized) {
        std::cerr << "D-Bus interface not initialized" << std::endl;
        return false;
    }

    if (m_running) {
        return true;
    }

    // Register D-Bus interface
    if (!registerInterface()) {
        std::cerr << "Failed to register D-Bus interface" << std::endl;
        return false;
    }

    m_running = true;
    std::cout << "D-Bus interface started" << std::endl;
    return true;
}

void DBusInterface::stop() {
    if (!m_running) {
        return;
    }

    unregisterInterface();
    m_running = false;
    std::cout << "D-Bus interface stopped" << std::endl;
}

bool DBusInterface::isHealthy() const {
    if (!m_running) {
        return false;
    }

    // Check D-Bus connection health
    QDBusConnection connection = QDBusConnection::systemBus();
    if (!connection.isConnected()) {
        return false;
    }

    return true;
}

QString DBusInterface::CheckUpdates() {
    if (!m_packageManager) {
        std::cerr << "Package manager not available" << std::endl;
        return QString("{\"error\":\"Package manager not available\"}");
    }

    // Check for updates
    std::vector<PackageInfo> updates = m_packageManager->checkUpdates();

    // Convert to JSON
    QJsonArray updatesArray;
    for (const auto& pkg : updates) {
        QJsonObject pkgObj;
        pkgObj["name"] = QString::fromStdString(pkg.name);
        pkgObj["version"] = QString::fromStdString(pkg.version);
        pkgObj["installed_version"] = QString::fromStdString(pkg.installed_version);
        pkgObj["repository"] = QString::fromStdString(pkg.repository);
        pkgObj["description"] = QString::fromStdString(pkg.description);
        updatesArray.append(pkgObj);

        // Emit signal for each update
        QJsonDocument doc(pkgObj);
        emit UpdateAvailable(QString::fromUtf8(doc.toJson()));
    }

    QJsonObject result;
    result["updates"] = updatesArray;
    result["count"] = static_cast<int>(updates.size());

    QJsonDocument doc(result);
    return QString::fromUtf8(doc.toJson());
}

QString DBusInterface::ApplyUpdates(const QString& packageList) {
    if (!m_updateApplier) {
        std::cerr << "Update applier not available" << std::endl;
        return QString();
    }

    // Parse package list
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(packageList.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError || !doc.isArray()) {
        std::cerr << "Invalid package list JSON" << std::endl;
        return QString();
    }

    QJsonArray array = doc.array();
    std::vector<std::string> packages;
    for (const auto& value : array) {
        packages.push_back(value.toString().toStdString());
    }

    // Apply updates
    UpdateInfo updateInfo = m_updateApplier->applyUpdates(packages);

    // Emit signals
    emit UpdateStarted(QString::fromStdString(updateInfo.update_id));

    if (updateInfo.status == UpdateStatus::COMPLETED) {
        emit UpdateCompleted(QString::fromStdString(updateInfo.update_id), "COMPLETED");
    } else if (updateInfo.status == UpdateStatus::FAILED) {
        emit UpdateError(
            QString::fromStdString(updateInfo.update_id),
            QString::fromStdString(updateInfo.error_message)
        );
        emit UpdateCompleted(QString::fromStdString(updateInfo.update_id), "FAILED");
    }

    return QString::fromStdString(updateInfo.update_id);
}

bool DBusInterface::RollbackUpdate(const QString& updateId) {
    if (!m_rollbackManager) {
        std::cerr << "Rollback manager not available" << std::endl;
        return false;
    }

    // Get snapshot for this update
    // TODO: Get snapshot ID from update history
    // For now, use update ID as snapshot ID
    bool success = m_rollbackManager->rollbackToSnapshot(updateId.toStdString());

    if (success && m_auditLogger) {
        m_auditLogger->logUpdateActivity(
            updateId,
            "rollback",
            QStringList(),
            "COMPLETED"
        );
    }

    return success;
}

QString DBusInterface::GetUpdateStatus(const QString& updateId) {
    if (!m_updateApplier) {
        std::cerr << "Update applier not available" << std::endl;
        return QString("{\"error\":\"Update applier not available\"}");
    }

    std::string updateIdStr = updateId.toStdString();
    if (updateIdStr.empty()) {
        // Return current update status
        // TODO: Get current update from update applier
        QJsonObject status;
        status["status"] = "no_update_in_progress";
        QJsonDocument doc(status);
        return QString::fromUtf8(doc.toJson());
    }

    auto updateInfo = m_updateApplier->getUpdateStatus(updateIdStr);
    if (!updateInfo) {
        QJsonObject status;
        status["status"] = "not_found";
        QJsonDocument doc(status);
        return QString::fromUtf8(doc.toJson());
    }

    // Convert to JSON
    QJsonObject status;
    status["update_id"] = QString::fromStdString(updateInfo->update_id);
    QJsonArray packageArray;
    for (const auto& pkg : updateInfo->package_list) {
        packageArray.append(QString::fromStdString(pkg));
    }
    status["package_list"] = packageArray;
    
    QString statusStr;
    switch (updateInfo->status) {
        case UpdateStatus::PENDING:
            statusStr = "PENDING";
            break;
        case UpdateStatus::IN_PROGRESS:
            statusStr = "IN_PROGRESS";
            break;
        case UpdateStatus::COMPLETED:
            statusStr = "COMPLETED";
            break;
        case UpdateStatus::FAILED:
            statusStr = "FAILED";
            break;
        case UpdateStatus::ROLLED_BACK:
            statusStr = "ROLLED_BACK";
            break;
    }
    status["status"] = statusStr;
    status["signature_verified"] = updateInfo->signature_verified;
    if (!updateInfo->error_message.empty()) {
        status["error_message"] = QString::fromStdString(updateInfo->error_message);
    }
    if (!updateInfo->rollback_info.empty()) {
        status["rollback_info"] = QString::fromStdString(updateInfo->rollback_info);
    }

    QJsonDocument doc(status);
    return QString::fromUtf8(doc.toJson());
}

QString DBusInterface::GetUpdateHistory() {
    // TODO: Load from update history database
    // For now, return empty history
    QJsonObject history;
    history["updates"] = QJsonArray();
    history["count"] = 0;

    QJsonDocument doc(history);
    return QString::fromUtf8(doc.toJson());
}

QString DBusInterface::GetHealthStatus() {
    // Get health status from service (via UpdateService)
    // For now, we'll construct it from available components
    QJsonObject health;

    health["dbus_interface"] = isHealthy();

    if (m_packageManager) {
        QJsonObject pkgMgr;
        pkgMgr["initialized"] = true;
        pkgMgr["available"] = m_packageManager->isAvailable();
        pkgMgr["type"] = QString::fromStdString(m_packageManager->getType());
        health["package_manager"] = pkgMgr;
    }

    if (m_signatureVerifier) {
        QJsonObject sigVer;
        sigVer["initialized"] = true;
        sigVer["enabled"] = m_signatureVerifier->isEnabled();
        sigVer["required"] = m_signatureVerifier->isRequired();
        health["signature_verifier"] = sigVer;
    }

    health["overall_health"] = (isHealthy() ? "healthy" : "unhealthy");
    health["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QJsonDocument doc(health);
    return QString::fromUtf8(doc.toJson());
}

bool DBusInterface::registerInterface() {
    QDBusConnection connection = QDBusConnection::systemBus();

    if (!connection.isConnected()) {
        std::cerr << "Cannot connect to D-Bus system bus: "
                  << connection.lastError().message().toStdString() << std::endl;
        return false;
    }

    // Register object at path /org/milos/UpdateService
    QString objectPath = "/org/milos/UpdateService";
    if (!connection.registerObject(objectPath, this)) {
        std::cerr << "Failed to register D-Bus object: "
                  << connection.lastError().message().toStdString() << std::endl;
        return false;
    }

    // Register service name org.milos.UpdateService
    QString serviceName = "org.milos.UpdateService";
    if (!connection.registerService(serviceName)) {
        std::cerr << "Failed to register D-Bus service: "
                  << connection.lastError().message().toStdString() << std::endl;
        connection.unregisterObject(objectPath);
        return false;
    }

    std::cout << "D-Bus interface registered: " << serviceName.toStdString()
              << " at " << objectPath.toStdString() << std::endl;
    return true;
}

void DBusInterface::unregisterInterface() {
    QDBusConnection connection = QDBusConnection::systemBus();

    QString objectPath = "/org/milos/UpdateService";
    QString serviceName = "org.milos.UpdateService";

    connection.unregisterObject(objectPath);
    connection.unregisterService(serviceName);
}

