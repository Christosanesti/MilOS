#ifndef REMEDIATION_MANAGER_H
#define REMEDIATION_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

class BaselineManager;
class ChangeDetector;
class AuditLogger;

/**
 * @brief Remediation Action Type
 */
enum class RemediationActionType {
    RESTORE_FILE,
    QUARANTINE_FILE,
    DELETE_FILE,
    RESTORE_PERMISSIONS,
    RESTORE_OWNERSHIP,
    NOTIFY_ADMIN,
    BLOCK_ACCESS
};

/**
 * @brief Remediation Rule
 */
struct RemediationRule {
    std::string rule_id;
    std::string rule_name;
    std::string description;
    std::string change_type;  // modified, deleted, created, permissions_changed, ownership_changed
    std::string severity;    // critical, high, medium, low
    std::string file_pattern;  // File path pattern (wildcard)
    RemediationActionType action_type;
    bool requires_approval;
    bool auto_execute;
    int priority;  // Higher priority rules execute first
};

/**
 * @brief Remediation Request
 */
struct RemediationRequest {
    std::string request_id;
    std::string change_id;
    std::string file_path;
    RemediationActionType action_type;
    std::string rule_id;
    std::string status;  // pending, approved, rejected, executing, completed, failed, rolled_back
    std::string requested_at;
    std::string approved_at;
    std::string completed_at;
    std::string approver;
    std::string error_message;
    std::map<std::string, std::string> metadata;  // Additional metadata for the action
};

/**
 * @brief Remediation Result
 */
struct RemediationResult {
    bool success;
    std::string request_id;
    std::string message;
    std::string executed_at;
    std::string rollback_id;  // ID for rollback operation
};

/**
 * @brief Remediation Manager
 * 
 * Manages automated remediation actions for file integrity violations.
 */
class RemediationManager {
public:
    RemediationManager();
    ~RemediationManager();

    /**
     * @brief Initialize remediation manager
     * @param baselineManager Baseline manager instance
     * @param changeDetector Change detector instance
     * @param auditLogger Audit logger instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(BaselineManager* baselineManager, ChangeDetector* changeDetector, AuditLogger* auditLogger);

    /**
     * @brief Add remediation rule
     * @param rule Remediation rule
     */
    void addRule(const RemediationRule& rule);

    /**
     * @brief Remove remediation rule
     * @param ruleId Rule ID
     */
    void removeRule(const std::string& ruleId);

    /**
     * @brief Get remediation rule
     * @param ruleId Rule ID
     * @return Remediation rule, or empty if not found
     */
    RemediationRule getRule(const std::string& ruleId) const;

    /**
     * @brief Get all remediation rules
     * @return List of remediation rules
     */
    std::vector<RemediationRule> getAllRules() const;

    /**
     * @brief Evaluate change and create remediation request
     * @param changeId Change ID
     * @param filePath File path
     * @param changeType Change type
     * @param severity Change severity
     * @return Remediation request, or empty if no matching rule
     */
    RemediationRequest evaluateChange(const std::string& changeId,
                                      const std::string& filePath,
                                      const std::string& changeType,
                                      const std::string& severity);

    /**
     * @brief Execute remediation request
     * @param requestId Request ID
     * @return Remediation result
     */
    RemediationResult executeRemediation(const std::string& requestId);

    /**
     * @brief Approve remediation request
     * @param requestId Request ID
     * @param approver Approver identifier
     * @return true if approval successful, false otherwise
     */
    bool approveRemediation(const std::string& requestId, const std::string& approver);

    /**
     * @brief Reject remediation request
     * @param requestId Request ID
     * @param reason Rejection reason
     * @return true if rejection successful, false otherwise
     */
    bool rejectRemediation(const std::string& requestId, const std::string& reason);

    /**
     * @brief Rollback remediation
     * @param requestId Request ID
     * @return true if rollback successful, false otherwise
     */
    bool rollbackRemediation(const std::string& requestId);

    /**
     * @brief Get remediation request
     * @param requestId Request ID
     * @return Remediation request, or empty if not found
     */
    RemediationRequest getRequest(const std::string& requestId) const;

    /**
     * @brief Get all remediation requests
     * @param statusFilter Status filter (optional)
     * @return List of remediation requests
     */
    std::vector<RemediationRequest> getAllRequests(const std::string& statusFilter = "") const;

    /**
     * @brief Get pending remediation requests
     * @return List of pending requests
     */
    std::vector<RemediationRequest> getPendingRequests() const;

private:
    bool m_initialized;
    BaselineManager* m_baselineManager;
    ChangeDetector* m_changeDetector;
    AuditLogger* m_auditLogger;
    std::map<std::string, RemediationRule> m_rules;
    std::map<std::string, RemediationRequest> m_requests;
    std::string m_quarantineDirectory;

    /**
     * @brief Generate request ID
     * @return Request ID
     */
    std::string generateRequestId() const;

    /**
     * @brief Match rule against change
     * @param rule Remediation rule
     * @param changeType Change type
     * @param severity Change severity
     * @param filePath File path
     * @return true if rule matches, false otherwise
     */
    bool matchRule(const RemediationRule& rule,
                   const std::string& changeType,
                   const std::string& severity,
                   const std::string& filePath) const;

    /**
     * @brief Execute remediation action
     * @param request Remediation request
     * @return Remediation result
     */
    RemediationResult executeAction(const RemediationRequest& request);

    /**
     * @brief Restore file from baseline
     * @param filePath File path
     * @param baselineId Baseline ID
     * @return true if restore successful, false otherwise
     */
    bool restoreFile(const std::string& filePath, const std::string& baselineId);

    /**
     * @brief Quarantine file
     * @param filePath File path
     * @return Quarantine path, or empty if failed
     */
    std::string quarantineFile(const std::string& filePath);

    /**
     * @brief Restore file permissions
     * @param filePath File path
     * @param permissions Permissions string (e.g., "0644")
     * @return true if restore successful, false otherwise
     */
    bool restorePermissions(const std::string& filePath, const std::string& permissions);

    /**
     * @brief Restore file ownership
     * @param filePath File path
     * @param owner Owner string (e.g., "root:root")
     * @return true if restore successful, false otherwise
     */
    bool restoreOwnership(const std::string& filePath, const std::string& owner);

    /**
     * @brief Log remediation action
     * @param request Remediation request
     * @param result Remediation result
     */
    void logRemediation(const RemediationRequest& request, const RemediationResult& result);
};

#endif // REMEDIATION_MANAGER_H

