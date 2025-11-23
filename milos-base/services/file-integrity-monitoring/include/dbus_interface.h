#ifndef DBUS_INTERFACE_H
#define DBUS_INTERFACE_H

#include <QObject>
#include <QString>
#include <QStringList>

class FIMService;
class BaselineManager;
class ChangeDetector;
class IntegrityVerifier;
class RemediationManager;
class VerificationScheduler;

/**
 * @brief D-Bus Interface for File Integrity Monitoring Service
 * 
 * Implements org.milos.FileIntegrityMonitoring D-Bus interface.
 */
class DBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.FileIntegrityMonitoring")

public:
    DBusInterface(QObject* parent = nullptr);
    ~DBusInterface();

    /**
     * @brief Initialize D-Bus interface
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Set FIM service instance
     */
    void setFIMService(FIMService* service);

    /**
     * @brief Set baseline manager instance
     */
    void setBaselineManager(BaselineManager* manager);

    /**
     * @brief Set change detector instance
     */
    void setChangeDetector(ChangeDetector* detector);

    /**
     * @brief Set integrity verifier instance
     */
    void setIntegrityVerifier(IntegrityVerifier* verifier);

    /**
     * @brief Set remediation manager instance
     */
    void setRemediationManager(RemediationManager* manager);

    /**
     * @brief Set verification scheduler instance
     */
    void setVerificationScheduler(VerificationScheduler* scheduler);

public Q_SLOTS:
    // D-Bus methods
    /**
     * @brief Create baseline for file(s)
     * @param filePaths List of file paths
     * @return List of baseline IDs
     */
    QStringList CreateBaseline(const QStringList& filePaths);

    /**
     * @brief Verify file integrity
     * @param filePath File path to verify
     * @return Verification result (JSON string)
     */
    QString VerifyIntegrity(const QString& filePath);

    /**
     * @brief Get detected changes
     * @param filters Optional filters (JSON string)
     * @return List of changes (JSON string)
     */
    QString GetChanges(const QString& filters = "");

    /**
     * @brief Add change whitelist entry
     * @param filePattern File path pattern (supports wildcards)
     * @param changeType Change type (empty for all types)
     * @param description Optional description
     * @return Whitelist entry ID
     */
    QString AddWhitelistEntry(const QString& filePattern,
                              const QString& changeType = "",
                              const QString& description = "");

    /**
     * @brief Remove whitelist entry
     * @param whitelistId Whitelist entry ID
     * @return true if removal successful, false otherwise
     */
    bool RemoveWhitelistEntry(const QString& whitelistId);

    /**
     * @brief Get all whitelist entries
     * @return List of whitelist entries (JSON string)
     */
    QString GetWhitelistEntries();

    /**
     * @brief Get baseline status
     * @param baselineId Baseline ID
     * @return Status string
     */
    QString GetBaselineStatus(const QString& baselineId);

    /**
     * @brief Update baseline
     * @param baselineId Baseline ID
     * @return true if update successful, false otherwise
     */
    bool UpdateBaseline(const QString& baselineId);

    /**
     * @brief Get baseline versions
     * @param baselineId Baseline ID
     * @return List of baseline versions (JSON string)
     */
    QString GetBaselineVersions(const QString& baselineId);

    /**
     * @brief Get specific baseline version
     * @param baselineId Baseline ID
     * @param version Version string
     * @return Baseline information for the specified version (JSON string)
     */
    QString GetBaselineVersion(const QString& baselineId, const QString& version);

    /**
     * @brief Rollback baseline to a specific version
     * @param baselineId Baseline ID
     * @param version Version to rollback to
     * @return true if rollback successful, false otherwise
     */
    bool RollbackBaseline(const QString& baselineId, const QString& version);

    /**
     * @brief Remediate a detected change
     * @param changeId Change ID to remediate
     * @return Remediation result (JSON string)
     */
    QString RemediateChange(const QString& changeId);

    /**
     * @brief Approve a pending remediation request
     * @param requestId Request ID
     * @param approver Approver identifier
     * @return true if approval successful, false otherwise
     */
    bool ApproveRemediation(const QString& requestId, const QString& approver);

    /**
     * @brief Get remediation request status
     * @param requestId Request ID
     * @return Remediation request status (JSON string)
     */
    QString GetRemediationStatus(const QString& requestId);

    /**
     * @brief Get all pending remediation requests
     * @return List of pending requests (JSON string)
     */
    QString GetPendingRemediations();

    /**
     * @brief Create a verification schedule
     * @param scheduleJson Schedule configuration (JSON string)
     * @return Schedule ID if successful, empty string otherwise
     */
    QString CreateVerificationSchedule(const QString& scheduleJson);

    /**
     * @brief Get verification schedule
     * @param scheduleId Schedule ID
     * @return Schedule information (JSON string)
     */
    QString GetVerificationSchedule(const QString& scheduleId);

    /**
     * @brief Get all verification schedules
     * @return List of schedules (JSON string)
     */
    QString GetVerificationSchedules();

    /**
     * @brief Delete verification schedule
     * @param scheduleId Schedule ID
     * @return true if deletion successful, false otherwise
     */
    bool DeleteVerificationSchedule(const QString& scheduleId);

    /**
     * @brief Enable/disable verification schedule
     * @param scheduleId Schedule ID
     * @param enabled Enable flag
     * @return true if update successful, false otherwise
     */
    bool SetVerificationScheduleEnabled(const QString& scheduleId, bool enabled);

    /**
     * @brief Get verification history
     * @param scheduleId Schedule ID (empty for all)
     * @param filePath File path filter (empty for all)
     * @param limit Maximum number of entries
     * @return Verification history (JSON string)
     */
    QString GetVerificationHistory(const QString& scheduleId = "",
                                    const QString& filePath = "",
                                    int limit = 100);

    /**
     * @brief Clear verification history
     * @param scheduleId Schedule ID (empty for all)
     * @param olderThanDays Delete entries older than N days (0 for all)
     * @return Number of entries deleted
     */
    int ClearVerificationHistory(const QString& scheduleId = "", int olderThanDays = 0);

Q_SIGNALS:
    /**
     * @brief Emitted when file changes detected
     * @param filePath File path
     * @param changeType Change type
     */
    void FileChanged(const QString& filePath, const QString& changeType);

    /**
     * @brief Emitted on integrity violation
     * @param filePath File path
     * @param details Violation details (JSON string)
     */
    void IntegrityViolation(const QString& filePath, const QString& details);

private:
    bool m_initialized;
    FIMService* m_fimService;
    BaselineManager* m_baselineManager;
    ChangeDetector* m_changeDetector;
    IntegrityVerifier* m_integrityVerifier;
    RemediationManager* m_remediationManager;
    VerificationScheduler* m_verificationScheduler;
};

#endif // DBUS_INTERFACE_H

