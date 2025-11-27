#include "dbus_interface.h"
#include "fim_service.h"
#include "baseline_manager.h"
#include "change_detector.h"
#include "integrity_verifier.h"
#include "remediation_manager.h"
#include "verification_scheduler.h"
#include <milos/logging/logger.h>
#include <QDBusConnection>
#include <QDBusMetaType>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

DBusInterface::DBusInterface(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_fimService(nullptr)
    , m_baselineManager(nullptr)
    , m_changeDetector(nullptr)
    , m_integrityVerifier(nullptr)
    , m_remediationManager(nullptr)
    , m_verificationScheduler(nullptr)
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
        LOG_ERROR("Failed to register D-Bus service");
        return false;
    }

    // Register D-Bus object
    if (!QDBusConnection::systemBus().registerObject("/org/milos/FileIntegrityMonitoring", this,
                                                      QDBusConnection::ExportAllContents)) {
        LOG_ERROR("Failed to register D-Bus object");
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

void DBusInterface::setVerificationScheduler(VerificationScheduler* scheduler) {
    m_verificationScheduler = scheduler;
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
            QJsonObject filterObj = doc.object();
            
            // Parse filter object and convert to "key:value" format
            if (filterObj.contains("file_path")) {
                filterList.push_back("file_path:" + filterObj["file_path"].toString().toStdString());
            }
            if (filterObj.contains("change_type")) {
                filterList.push_back("change_type:" + filterObj["change_type"].toString().toStdString());
            }
            if (filterObj.contains("severity")) {
                filterList.push_back("severity:" + filterObj["severity"].toString().toStdString());
            }
            if (filterObj.contains("baseline_id")) {
                filterList.push_back("baseline_id:" + filterObj["baseline_id"].toString().toStdString());
            }
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

QString DBusInterface::AddWhitelistEntry(const QString& filePattern,
                                         const QString& changeType,
                                         const QString& description) {
    if (!m_changeDetector) {
        return "";
    }

    std::string entryId = m_changeDetector->addWhitelistEntry(
        filePattern.toStdString(),
        changeType.toStdString(),
        description.toStdString()
    );

    return QString::fromStdString(entryId);
}

bool DBusInterface::RemoveWhitelistEntry(const QString& whitelistId) {
    if (!m_changeDetector) {
        return false;
    }

    return m_changeDetector->removeWhitelistEntry(whitelistId.toStdString());
}

QString DBusInterface::GetWhitelistEntries() {
    QJsonObject result;

    if (!m_changeDetector) {
        result["success"] = false;
        result["error"] = "Change detector not initialized";
        return QJsonDocument(result).toJson();
    }

    auto entries = m_changeDetector->getWhitelistEntries();
    QJsonArray entriesArray;

    for (const auto& entryMap : entries) {
        QJsonObject entryObj;
        entryObj["entry_id"] = QString::fromStdString(entryMap.at("entry_id"));
        entryObj["file_pattern"] = QString::fromStdString(entryMap.at("file_pattern"));
        entryObj["change_type"] = QString::fromStdString(entryMap.at("change_type"));
        entryObj["description"] = QString::fromStdString(entryMap.at("description"));
        entryObj["created_at"] = QString::fromStdString(entryMap.at("created_at"));
        entriesArray.append(entryObj);
    }

    result["success"] = true;
    result["entries"] = entriesArray;
    return QJsonDocument(result).toJson();
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

QString DBusInterface::GetBaselineVersions(const QString& baselineId) {
    QJsonObject result;

    if (!m_baselineManager) {
        result["success"] = false;
        result["error"] = "Baseline manager not initialized";
        return QJsonDocument(result).toJson();
    }

    auto versions = m_baselineManager->getBaselineVersions(baselineId.toStdString());
    if (versions.empty()) {
        result["success"] = false;
        result["error"] = "Baseline not found or no versions available";
        return QJsonDocument(result).toJson();
    }

    QJsonArray versionsArray;
    for (const auto& baseline : versions) {
        QJsonObject baselineObj;
        baselineObj["baseline_id"] = QString::fromStdString(baseline.baseline_id);
        baselineObj["file_path"] = QString::fromStdString(baseline.file_path);
        baselineObj["version"] = QString::fromStdString(baseline.version);
        baselineObj["file_hash"] = QString::fromStdString(baseline.file_hash);
        baselineObj["file_size"] = static_cast<qint64>(baseline.file_size);
        baselineObj["permissions"] = QString::fromStdString(baseline.permissions);
        baselineObj["owner"] = QString::fromStdString(baseline.owner);
        baselineObj["group"] = QString::fromStdString(baseline.group);
        baselineObj["created_at"] = QString::fromStdString(baseline.created_at);
        baselineObj["is_valid"] = baseline.is_valid;
        versionsArray.append(baselineObj);
    }

    result["success"] = true;
    result["baseline_id"] = baselineId;
    result["versions"] = versionsArray;
    return QJsonDocument(result).toJson();
}

QString DBusInterface::GetBaselineVersion(const QString& baselineId, const QString& version) {
    QJsonObject result;

    if (!m_baselineManager) {
        result["success"] = false;
        result["error"] = "Baseline manager not initialized";
        return QJsonDocument(result).toJson();
    }

    auto baseline = m_baselineManager->getBaselineVersion(baselineId.toStdString(), version.toStdString());
    if (baseline.baseline_id.empty()) {
        result["success"] = false;
        result["error"] = "Baseline version not found";
        return QJsonDocument(result).toJson();
    }

    result["success"] = true;
    result["baseline_id"] = QString::fromStdString(baseline.baseline_id);
    result["file_path"] = QString::fromStdString(baseline.file_path);
    result["version"] = QString::fromStdString(baseline.version);
    result["file_hash"] = QString::fromStdString(baseline.file_hash);
    result["file_size"] = static_cast<qint64>(baseline.file_size);
    result["permissions"] = QString::fromStdString(baseline.permissions);
    result["owner"] = QString::fromStdString(baseline.owner);
    result["group"] = QString::fromStdString(baseline.group);
    result["created_at"] = QString::fromStdString(baseline.created_at);
    result["is_valid"] = baseline.is_valid;

    return QJsonDocument(result).toJson();
}

bool DBusInterface::RollbackBaseline(const QString& baselineId, const QString& version) {
    if (!m_baselineManager) {
        return false;
    }

    return m_baselineManager->rollbackBaseline(baselineId.toStdString(), version.toStdString());
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

QString DBusInterface::CreateVerificationSchedule(const QString& scheduleJson) {
    QJsonObject result;

    if (!m_verificationScheduler) {
        result["success"] = false;
        result["error"] = "Verification scheduler not initialized";
        return QJsonDocument(result).toJson();
    }

    QJsonDocument doc = QJsonDocument::fromJson(scheduleJson.toUtf8());
    if (!doc.isObject()) {
        result["success"] = false;
        result["error"] = "Invalid schedule JSON";
        return QJsonDocument(result).toJson();
    }

    QJsonObject scheduleObj = doc.object();
    VerificationSchedule schedule;
    schedule.name = scheduleObj["name"].toString().toStdString();
    schedule.description = scheduleObj["description"].toString().toStdString();
    schedule.cron_expression = scheduleObj["cron_expression"].toString().toStdString();
    schedule.enabled = scheduleObj["enabled"].toBool(true);

    QJsonArray filePathsArray = scheduleObj["file_paths"].toArray();
    for (const QJsonValue& value : filePathsArray) {
        schedule.file_paths.push_back(value.toString().toStdString());
    }

    std::string scheduleId = m_verificationScheduler->createSchedule(schedule);
    if (scheduleId.empty()) {
        result["success"] = false;
        result["error"] = "Failed to create schedule";
        return QJsonDocument(result).toJson();
    }

    result["success"] = true;
    result["schedule_id"] = QString::fromStdString(scheduleId);
    return QJsonDocument(result).toJson();
}

QString DBusInterface::GetVerificationSchedule(const QString& scheduleId) {
    QJsonObject result;

    if (!m_verificationScheduler) {
        result["success"] = false;
        result["error"] = "Verification scheduler not initialized";
        return QJsonDocument(result).toJson();
    }

    auto schedule = m_verificationScheduler->getSchedule(scheduleId.toStdString());
    if (schedule.schedule_id.empty()) {
        result["success"] = false;
        result["error"] = "Schedule not found";
        return QJsonDocument(result).toJson();
    }

    result["success"] = true;
    result["schedule_id"] = QString::fromStdString(schedule.schedule_id);
    result["name"] = QString::fromStdString(schedule.name);
    result["description"] = QString::fromStdString(schedule.description);
    result["cron_expression"] = QString::fromStdString(schedule.cron_expression);
    result["enabled"] = schedule.enabled;
    result["created_at"] = QString::fromStdString(schedule.created_at);
    result["last_run_at"] = QString::fromStdString(schedule.last_run_at);
    result["next_run_at"] = QString::fromStdString(schedule.next_run_at);

    QJsonArray filePathsArray;
    for (const auto& path : schedule.file_paths) {
        filePathsArray.append(QString::fromStdString(path));
    }
    result["file_paths"] = filePathsArray;

    return QJsonDocument(result).toJson();
}

QString DBusInterface::GetVerificationSchedules() {
    QJsonObject result;

    if (!m_verificationScheduler) {
        result["success"] = false;
        result["error"] = "Verification scheduler not initialized";
        return QJsonDocument(result).toJson();
    }

    auto schedules = m_verificationScheduler->getAllSchedules();
    QJsonArray schedulesArray;

    for (const auto& schedule : schedules) {
        QJsonObject scheduleObj;
        scheduleObj["schedule_id"] = QString::fromStdString(schedule.schedule_id);
        scheduleObj["name"] = QString::fromStdString(schedule.name);
        scheduleObj["description"] = QString::fromStdString(schedule.description);
        scheduleObj["cron_expression"] = QString::fromStdString(schedule.cron_expression);
        scheduleObj["enabled"] = schedule.enabled;
        scheduleObj["created_at"] = QString::fromStdString(schedule.created_at);
        scheduleObj["last_run_at"] = QString::fromStdString(schedule.last_run_at);
        scheduleObj["next_run_at"] = QString::fromStdString(schedule.next_run_at);

        QJsonArray filePathsArray;
        for (const auto& path : schedule.file_paths) {
            filePathsArray.append(QString::fromStdString(path));
        }
        scheduleObj["file_paths"] = filePathsArray;
        schedulesArray.append(scheduleObj);
    }

    result["success"] = true;
    result["schedules"] = schedulesArray;
    return QJsonDocument(result).toJson();
}

bool DBusInterface::DeleteVerificationSchedule(const QString& scheduleId) {
    if (!m_verificationScheduler) {
        return false;
    }

    return m_verificationScheduler->deleteSchedule(scheduleId.toStdString());
}

bool DBusInterface::SetVerificationScheduleEnabled(const QString& scheduleId, bool enabled) {
    if (!m_verificationScheduler) {
        return false;
    }

    return m_verificationScheduler->setScheduleEnabled(scheduleId.toStdString(), enabled);
}

QString DBusInterface::GetVerificationHistory(const QString& scheduleId,
                                               const QString& filePath,
                                               int limit) {
    QJsonObject result;

    if (!m_verificationScheduler) {
        result["success"] = false;
        result["error"] = "Verification scheduler not initialized";
        return QJsonDocument(result).toJson();
    }

    auto history = m_verificationScheduler->getHistory(
        scheduleId.toStdString(),
        filePath.toStdString(),
        limit
    );

    QJsonArray historyArray;
    for (const auto& entry : history) {
        QJsonObject entryObj;
        entryObj["entry_id"] = QString::fromStdString(entry.entry_id);
        entryObj["schedule_id"] = QString::fromStdString(entry.schedule_id);
        entryObj["file_path"] = QString::fromStdString(entry.file_path);
        entryObj["is_valid"] = entry.is_valid;
        entryObj["baseline_id"] = QString::fromStdString(entry.baseline_id);
        entryObj["error_message"] = QString::fromStdString(entry.error_message);
        entryObj["verified_at"] = QString::fromStdString(entry.verified_at);
        entryObj["duration_ms"] = QString::fromStdString(entry.duration_ms);
        historyArray.append(entryObj);
    }

    result["success"] = true;
    result["history"] = historyArray;
    return QJsonDocument(result).toJson();
}

int DBusInterface::ClearVerificationHistory(const QString& scheduleId, int olderThanDays) {
    if (!m_verificationScheduler) {
        return 0;
    }

    return m_verificationScheduler->clearHistory(scheduleId.toStdString(), olderThanDays);
}

