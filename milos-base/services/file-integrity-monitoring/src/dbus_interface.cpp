#include "dbus_interface.h"
#include "fim_service.h"
#include "baseline_manager.h"
#include "change_detector.h"
#include "integrity_verifier.h"
#include "remediation_manager.h"
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
    , m_remediationManager(nullptr)
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

void DBusInterface::setRemediationManager(RemediationManager* manager) {
    m_remediationManager = manager;
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

QString DBusInterface::RemediateChange(const QString& changeId) {
    QJsonObject result;

    if (!m_remediationManager || !m_changeDetector) {
        result["success"] = false;
        result["error"] = "Remediation manager or change detector not initialized";
        return QJsonDocument(result).toJson();
    }

    // Get the change details
    auto changes = m_changeDetector->getChanges({});
    auto changeIt = std::find_if(changes.begin(), changes.end(),
                                 [&changeId](const auto& change) {
                                     return change.change_id == changeId.toStdString();
                                 });

    if (changeIt == changes.end()) {
        result["success"] = false;
        result["error"] = "Change ID not found: " + changeId;
        return QJsonDocument(result).toJson();
    }

    const auto& change = *changeIt;

    // Convert change type to string
    std::string changeTypeStr;
    switch (change.change_type) {
        case ChangeType::MODIFIED: changeTypeStr = "modified"; break;
        case ChangeType::DELETED: changeTypeStr = "deleted"; break;
        case ChangeType::CREATED: changeTypeStr = "created"; break;
        case ChangeType::PERMISSIONS_CHANGED: changeTypeStr = "permissions_changed"; break;
        case ChangeType::OWNERSHIP_CHANGED: changeTypeStr = "ownership_changed"; break;
    }

    // Evaluate change and create remediation request
    auto remediationRequest = m_remediationManager->evaluateChange(
        change.change_id,
        change.file_path,
        changeTypeStr,
        change.severity
    );

    if (remediationRequest.request_id.empty()) {
        result["success"] = false;
        result["error"] = "No matching remediation rule found for this change";
        return QJsonDocument(result).toJson();
    }

    // Execute remediation if status is "approved", otherwise return pending status
    RemediationResult remediationResult;
    if (remediationRequest.status == "approved") {
        // Auto-execute if already approved
        remediationResult = m_remediationManager->executeRemediation(remediationRequest.request_id);
    } else {
        // Return pending status
        result["success"] = true;
        result["request_id"] = QString::fromStdString(remediationRequest.request_id);
        result["status"] = "pending";
        result["message"] = "Remediation request created and pending approval";
        return QJsonDocument(result).toJson();
    }

    result["success"] = remediationResult.success;
    result["request_id"] = QString::fromStdString(remediationResult.request_id);
    result["message"] = QString::fromStdString(remediationResult.message);
    result["executed_at"] = QString::fromStdString(remediationResult.executed_at);
    if (!remediationResult.rollback_id.empty()) {
        result["rollback_id"] = QString::fromStdString(remediationResult.rollback_id);
    }

    return QJsonDocument(result).toJson();
}

bool DBusInterface::ApproveRemediation(const QString& requestId, const QString& approver) {
    if (!m_remediationManager) {
        return false;
    }

    return m_remediationManager->approveRemediation(requestId.toStdString(), approver.toStdString());
}

QString DBusInterface::GetRemediationStatus(const QString& requestId) {
    QJsonObject result;

    if (!m_remediationManager) {
        result["success"] = false;
        result["error"] = "Remediation manager not initialized";
        return QJsonDocument(result).toJson();
    }

    auto request = m_remediationManager->getRequest(requestId.toStdString());
    if (request.request_id.empty()) {
        result["success"] = false;
        result["error"] = "Remediation request not found";
        return QJsonDocument(result).toJson();
    }

    result["success"] = true;
    result["request_id"] = QString::fromStdString(request.request_id);
    result["change_id"] = QString::fromStdString(request.change_id);
    result["file_path"] = QString::fromStdString(request.file_path);
    result["status"] = QString::fromStdString(request.status);
    result["action_type"] = static_cast<int>(request.action_type);
    result["requested_at"] = QString::fromStdString(request.requested_at);
    if (!request.approved_at.empty()) {
        result["approved_at"] = QString::fromStdString(request.approved_at);
    }
    if (!request.completed_at.empty()) {
        result["completed_at"] = QString::fromStdString(request.completed_at);
    }
    if (!request.approver.empty()) {
        result["approver"] = QString::fromStdString(request.approver);
    }
    if (!request.error_message.empty()) {
        result["error_message"] = QString::fromStdString(request.error_message);
    }

    return QJsonDocument(result).toJson();
}

QString DBusInterface::GetPendingRemediations() {
    QJsonArray requestsArray;

    if (!m_remediationManager) {
        return QJsonDocument(requestsArray).toJson();
    }

    auto requests = m_remediationManager->getPendingRequests();
    for (const auto& request : requests) {
        QJsonObject requestObj;
        requestObj["request_id"] = QString::fromStdString(request.request_id);
        requestObj["change_id"] = QString::fromStdString(request.change_id);
        requestObj["file_path"] = QString::fromStdString(request.file_path);
        requestObj["status"] = QString::fromStdString(request.status);
        requestObj["action_type"] = static_cast<int>(request.action_type);
        requestObj["requested_at"] = QString::fromStdString(request.requested_at);
        requestsArray.append(requestObj);
    }

    QJsonObject result;
    result["success"] = true;
    result["requests"] = requestsArray;
    return QJsonDocument(result).toJson();
}

