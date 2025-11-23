#include "dbus_interface.h"
#include "fim_service.h"
#include "baseline_manager.h"
#include "change_detector.h"
#include "integrity_verifier.h"
#include <QDBusConnection>
#include <QDBusMetaType>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>

DBusInterface::DBusInterface(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_fimService(nullptr)
    , m_baselineManager(nullptr)
    , m_changeDetector(nullptr)
    , m_integrityVerifier(nullptr)
{
}

DBusInterface::~DBusInterface() {
}

bool DBusInterface::initialize() {
    if (m_initialized) {
        return true;
    }

    // Register D-Bus service
    if (!QDBusConnection::systemBus().registerService("org.milos.FileIntegrityMonitoring")) {
        std::cerr << "Failed to register D-Bus service" << std::endl;
        return false;
    }

    // Register D-Bus object
    if (!QDBusConnection::systemBus().registerObject("/org/milos/FileIntegrityMonitoring", this,
                                                      QDBusConnection::ExportAllContents)) {
        std::cerr << "Failed to register D-Bus object" << std::endl;
        return false;
    }

    m_initialized = true;
    return true;
}

void DBusInterface::setFIMService(FIMService* service) {
    m_fimService = service;
}

void DBusInterface::setBaselineManager(BaselineManager* manager) {
    m_baselineManager = manager;
}

void DBusInterface::setChangeDetector(ChangeDetector* detector) {
    m_changeDetector = detector;
}

void DBusInterface::setIntegrityVerifier(IntegrityVerifier* verifier) {
    m_integrityVerifier = verifier;
}

QStringList DBusInterface::CreateBaseline(const QStringList& filePaths) {
    QStringList baselineIds;

    if (!m_baselineManager) {
        return baselineIds;
    }

    std::vector<std::string> paths;
    for (const QString& path : filePaths) {
        paths.push_back(path.toStdString());
    }

    std::vector<std::string> ids = m_baselineManager->createBaseline(paths);
    for (const std::string& id : ids) {
        baselineIds << QString::fromStdString(id);
    }

    return baselineIds;
}

QString DBusInterface::VerifyIntegrity(const QString& filePath) {
    QJsonObject result;

    if (!m_integrityVerifier) {
        result["success"] = false;
        result["error"] = "Integrity verifier not initialized";
        return QJsonDocument(result).toJson();
    }

    auto verificationResult = m_integrityVerifier->verifyIntegrity(filePath.toStdString());

    result["success"] = verificationResult.success;
    result["is_valid"] = verificationResult.is_valid;
    result["file_path"] = QString::fromStdString(verificationResult.file_path);
    result["baseline_id"] = QString::fromStdString(verificationResult.baseline_id);
    result["error_message"] = QString::fromStdString(verificationResult.error_message);
    result["verified_at"] = QString::fromStdString(verificationResult.verified_at);

    return QJsonDocument(result).toJson();
}

QString DBusInterface::GetChanges(const QString& filters) {
    QJsonArray changesArray;

    if (!m_changeDetector) {
        return QJsonDocument(changesArray).toJson();
    }

    // Parse filters if provided
    std::vector<std::string> filterList;
    if (!filters.isEmpty()) {
        QJsonDocument doc = QJsonDocument::fromJson(filters.toUtf8());
        if (doc.isObject()) {
            // TODO: Parse filter object
        }
    }

    auto changes = m_changeDetector->getChanges(filterList);
    for (const auto& change : changes) {
        QJsonObject changeObj;
        changeObj["change_id"] = QString::fromStdString(change.change_id);
        changeObj["file_path"] = QString::fromStdString(change.file_path);
        changeObj["change_type"] = static_cast<int>(change.change_type);
        changeObj["baseline_id"] = QString::fromStdString(change.baseline_id);
        changeObj["severity"] = QString::fromStdString(change.severity);
        changeObj["detected_at"] = QString::fromStdString(change.detected_at);
        changeObj["description"] = QString::fromStdString(change.description);
        changesArray.append(changeObj);
    }

    return QJsonDocument(changesArray).toJson();
}

QString DBusInterface::GetBaselineStatus(const QString& baselineId) {
    if (!m_baselineManager) {
        return "error";
    }

    std::string status = m_baselineManager->getBaselineStatus(baselineId.toStdString());
    return QString::fromStdString(status);
}

bool DBusInterface::UpdateBaseline(const QString& baselineId) {
    if (!m_baselineManager) {
        return false;
    }

    return m_baselineManager->updateBaseline(baselineId.toStdString());
}

