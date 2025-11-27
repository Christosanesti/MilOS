#include "update_applier.h"
#include "config_parser.h"
#include "package_manager.h"
#include "signature_verifier.h"
#include "rollback_manager.h"
#include "audit_logger.h"
#include <milos/logging/logger.h>
#include <QUuid>
#include <QDateTime>
#include <algorithm>
#include <mutex>

UpdateApplier::UpdateApplier()
    : m_initialized(false)
    , m_configParser(nullptr)
    , m_packageManager(nullptr)
    , m_signatureVerifier(nullptr)
    , m_rollbackManager(nullptr)
    , m_auditLogger(nullptr)
    , m_currentUpdate(nullptr)
{
}

UpdateApplier::~UpdateApplier() {
}

bool UpdateApplier::initialize(
    ConfigParser* configParser,
    PackageManager* packageManager,
    SignatureVerifier* signatureVerifier,
    RollbackManager* rollbackManager,
    AuditLogger* auditLogger
) {
    if (m_initialized) {
        return true;
    }

    m_configParser = configParser;
    m_packageManager = packageManager;
    m_signatureVerifier = signatureVerifier;
    m_rollbackManager = rollbackManager;
    m_auditLogger = auditLogger;

    m_initialized = true;
    return true;
}

UpdateInfo UpdateApplier::applyUpdates(
    const std::vector<std::string>& packageList,
    std::function<void(const std::string&, int)> progressCallback
) {
    UpdateInfo updateInfo;
    updateInfo.update_id = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
    updateInfo.package_list = packageList;
    updateInfo.status = UpdateStatus::IN_PROGRESS;
    updateInfo.signature_verified = false;

    if (!m_initialized || packageList.empty()) {
        updateInfo.status = UpdateStatus::FAILED;
        updateInfo.error_message = "Update applier not initialized or empty package list";
        return updateInfo;
    }

    // Log update start
    if (m_auditLogger) {
        QStringList qPackageList;
        for (const auto& pkg : packageList) {
            qPackageList << QString::fromStdString(pkg);
        }
        m_auditLogger->logUpdateActivity(
            QString::fromStdString(updateInfo.update_id),
            "apply",
            qPackageList,
            "IN_PROGRESS"
        );
    }

    // Create backup before update
    bool autoSnapshot = m_configParser->getBool("rollback.auto_snapshot", true);
    if (autoSnapshot && m_rollbackManager) {
        std::string snapshotId = m_rollbackManager->createSnapshot(updateInfo.update_id, packageList);
        if (!snapshotId.empty()) {
            updateInfo.rollback_info = "{\"snapshot_id\":\"" + snapshotId + "\"}";
        }
    }

    // Verify signatures
    if (m_signatureVerifier && m_signatureVerifier->isEnabled()) {
        if (!verifySignatures(packageList)) {
            updateInfo.status = UpdateStatus::FAILED;
            updateInfo.error_message = "Signature verification failed";
            updateInfo.signature_verified = false;
            
            if (m_auditLogger) {
                QStringList qPackageList;
                for (const auto& pkg : packageList) {
                    qPackageList << QString::fromStdString(pkg);
                }
                m_auditLogger->logUpdateActivity(
                    QString::fromStdString(updateInfo.update_id),
                    "apply",
                    qPackageList,
                    "FAILED",
                    QVariantMap{{"error", "Signature verification failed"}}
                );
            }
            return updateInfo;
        }
        updateInfo.signature_verified = true;
    }

    // Store as current update
    {
        std::lock_guard<std::mutex> lock(m_currentUpdateMutex);
        m_currentUpdate = std::make_unique<UpdateInfo>(updateInfo);
    }

    // Apply updates
    if (progressCallback) {
        progressCallback("Installing packages", 50);
    }

    bool success = m_packageManager->installPackages(packageList);

    if (success) {
        updateInfo.status = UpdateStatus::COMPLETED;
        if (progressCallback) {
            progressCallback("Update completed", 100);
        }
        
        if (m_auditLogger) {
            QStringList qPackageList;
            for (const auto& pkg : packageList) {
                qPackageList << QString::fromStdString(pkg);
            }
            m_auditLogger->logUpdateActivity(
                QString::fromStdString(updateInfo.update_id),
                "apply",
                qPackageList,
                "COMPLETED"
            );
        }
    } else {
        updateInfo.status = UpdateStatus::FAILED;
        updateInfo.error_message = "Package installation failed";
        
        if (m_auditLogger) {
            QStringList qPackageList;
            for (const auto& pkg : packageList) {
                qPackageList << QString::fromStdString(pkg);
            }
            m_auditLogger->logUpdateActivity(
                QString::fromStdString(updateInfo.update_id),
                "apply",
                qPackageList,
                "FAILED",
                QVariantMap{{"error", "Package installation failed"}}
            );
        }
    }

    // Store in history
    {
        std::lock_guard<std::mutex> lock(m_historyMutex);
        m_updateHistory.push_back(updateInfo);
        // Keep only last 1000 updates
        if (m_updateHistory.size() > 1000) {
            m_updateHistory.erase(m_updateHistory.begin());
        }
    }

    // Clear current update
    {
        std::lock_guard<std::mutex> lock(m_currentUpdateMutex);
        m_currentUpdate.reset();
    }

    return updateInfo;
}

std::unique_ptr<UpdateInfo> UpdateApplier::getUpdateStatus(const std::string& updateId) {
    // Check current update first
    {
        std::lock_guard<std::mutex> lock(m_currentUpdateMutex);
        if (m_currentUpdate && m_currentUpdate->update_id == updateId) {
            return std::make_unique<UpdateInfo>(*m_currentUpdate);
        }
    }

    // Search in history
    std::lock_guard<std::mutex> lock(m_historyMutex);
    for (const auto& update : m_updateHistory) {
        if (update.update_id == updateId) {
            return std::make_unique<UpdateInfo>(update);
        }
    }

    return nullptr;
}

std::vector<UpdateInfo> UpdateApplier::getUpdateHistory() const {
    std::lock_guard<std::mutex> lock(m_historyMutex);
    return m_updateHistory;
}

std::unique_ptr<UpdateInfo> UpdateApplier::getCurrentUpdate() const {
    std::lock_guard<std::mutex> lock(m_currentUpdateMutex);
    if (m_currentUpdate) {
        return std::make_unique<UpdateInfo>(*m_currentUpdate);
    }
    return nullptr;
}

bool UpdateApplier::createBackup(const std::vector<std::string>& packageList) {
    if (!m_rollbackManager) {
        return false;
    }

    // Create snapshot (backup is handled by rollback manager)
    std::string snapshotId = m_rollbackManager->createSnapshot("backup", packageList);
    return !snapshotId.empty();
}

bool UpdateApplier::verifySignatures(const std::vector<std::string>& packageList) {
    if (!m_signatureVerifier || !m_signatureVerifier->isEnabled()) {
        return true;  // If verification disabled, allow
    }

    for (const auto& packageName : packageList) {
        SignatureResult result = m_signatureVerifier->verifyPackageByName(packageName);
        
        if (!result.verified) {
            LOG_ERROR(QString("Signature verification failed for package: %1").arg(QString::fromStdString(packageName)));
            LOG_ERROR(QString("Error: %1").arg(QString::fromStdString(result.error_message)));
            
            if (m_auditLogger) {
                m_auditLogger->logSignatureVerification(
                    QString::fromStdString(packageName),
                    false,
                    QString::fromStdString(result.key_id)
                );
            }
            
            if (m_signatureVerifier->isRequired()) {
                return false;
            }
        } else {
            if (m_auditLogger) {
                m_auditLogger->logSignatureVerification(
                    QString::fromStdString(packageName),
                    true,
                    QString::fromStdString(result.key_id)
                );
            }
        }
    }

    return true;
}

