#include "backup_retention.h"
#include "backup_metadata.h"
#include <QTimer>
#include <QDateTime>
#include <algorithm>
#include <iostream>

BackupRetentionManager::BackupRetentionManager(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_running(false)
    , m_metadataManager(nullptr)
    , m_cleanupTimer(nullptr)
{
}

BackupRetentionManager::~BackupRetentionManager() {
    stop();
}

bool BackupRetentionManager::initialize(BackupMetadataManager* metadataManager) {
    if (m_initialized) {
        return true;
    }

    m_metadataManager = metadataManager;
    if (!m_metadataManager) {
        std::cerr << "Backup metadata manager is null" << std::endl;
        return false;
    }

    m_cleanupTimer = new QTimer(this);
    connect(m_cleanupTimer, &QTimer::timeout, this, &BackupRetentionManager::performCleanup);
    m_cleanupTimer->setInterval(3600000);  // Check every hour

    m_initialized = true;
    return true;
}

bool BackupRetentionManager::start() {
    if (!m_initialized) {
        return false;
    }

    if (m_running) {
        return true;
    }

    if (m_cleanupTimer) {
        m_cleanupTimer->start();
    }

    m_running = true;
    return true;
}

void BackupRetentionManager::stop() {
    if (!m_running) {
        return;
    }

    if (m_cleanupTimer) {
        m_cleanupTimer->stop();
    }

    m_running = false;
}

bool BackupRetentionManager::addPolicy(const RetentionPolicy& policy) {
    // Check if policy already exists
    for (const auto& existingPolicy : m_policies) {
        if (existingPolicy.policyId == policy.policyId) {
            return false;
        }
    }

    m_policies.push_back(policy);
    return true;
}

bool BackupRetentionManager::removePolicy(const QString& policyId) {
    auto it = std::remove_if(m_policies.begin(), m_policies.end(),
        [&policyId](const RetentionPolicy& policy) {
            return policy.policyId == policyId;
        });
    
    if (it != m_policies.end()) {
        m_policies.erase(it, m_policies.end());
        return true;
    }
    
    return false;
}

std::vector<RetentionPolicy> BackupRetentionManager::getPolicies() const {
    return m_policies;
}

uint32_t BackupRetentionManager::enforcePolicies() {
    if (!m_metadataManager) {
        return 0;
    }

    uint32_t cleanedCount = 0;
    uint64_t currentTime = QDateTime::currentSecsSinceEpoch();

    // Get all backups
    auto backups = m_metadataManager->listBackups();

    // Apply retention policies
    for (const auto& backup : backups) {
        bool shouldRetain = true;

        // Check each policy
        for (const auto& policy : m_policies) {
            if (!policy.enabled) {
                continue;
            }

            // Check retention days
            if (policy.retentionDays > 0) {
                uint64_t age = currentTime - backup.timestamp;
                uint64_t retentionSeconds = policy.retentionDays * 86400;
                if (age > retentionSeconds) {
                    shouldRetain = false;
                    break;
                }
            }

            // Check max backups (would need to count backups per policy)
            // Simplified for now
        }

        if (!shouldRetain) {
            // Delete backup
            if (m_metadataManager->deleteMetadata(backup.id)) {
                cleanedCount++;
            }
        }
    }

    if (cleanedCount > 0) {
        emit backupsCleanedUp(cleanedCount);
    }

    return cleanedCount;
}

bool BackupRetentionManager::shouldRetainBackup(const QString& backupId, uint64_t timestamp) const {
    uint64_t currentTime = QDateTime::currentSecsSinceEpoch();

    for (const auto& policy : m_policies) {
        if (!policy.enabled) {
            continue;
        }

        if (policy.retentionDays > 0) {
            uint64_t age = currentTime - timestamp;
            uint64_t retentionSeconds = policy.retentionDays * 86400;
            if (age > retentionSeconds) {
                return false;
            }
        }
    }

    return true;
}

void BackupRetentionManager::performCleanup() {
    enforcePolicies();
}

