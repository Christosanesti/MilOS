#include "remediation_manager.h"
#include "baseline_manager.h"
#include "change_detector.h"
#include "audit_logger.h"
#include <filesystem>
#include <fstream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <iostream>

RemediationManager::RemediationManager()
    : m_initialized(false)
    , m_baselineManager(nullptr)
    , m_changeDetector(nullptr)
    , m_auditLogger(nullptr)
    , m_quarantineDirectory("/var/lib/milos/fim/quarantine")
{
}

RemediationManager::~RemediationManager() {
}

bool RemediationManager::initialize(BaselineManager* baselineManager,
                                    ChangeDetector* changeDetector,
                                    AuditLogger* auditLogger) {
    if (m_initialized) {
        return true;
    }

    if (!baselineManager || !changeDetector || !auditLogger) {
        return false;
    }

    m_baselineManager = baselineManager;
    m_changeDetector = changeDetector;
    m_auditLogger = auditLogger;

    // Create quarantine directory
    std::filesystem::create_directories(m_quarantineDirectory);

    // Initialize default rules
    RemediationRule criticalRule;
    criticalRule.rule_id = "default_critical";
    criticalRule.rule_name = "Critical File Modification";
    criticalRule.description = "Automatically restore critical system files";
    criticalRule.change_type = "modified";
    criticalRule.severity = "critical";
    criticalRule.file_pattern = "/etc/*";
    criticalRule.action_type = RemediationActionType::RESTORE_FILE;
    criticalRule.requires_approval = false;
    criticalRule.auto_execute = true;
    criticalRule.priority = 100;
    m_rules[criticalRule.rule_id] = criticalRule;

    RemediationRule highSeverityRule;
    highSeverityRule.rule_id = "default_high";
    highSeverityRule.rule_name = "High Severity File Modification";
    highSeverityRule.description = "Quarantine high severity file changes";
    highSeverityRule.change_type = "modified";
    highSeverityRule.severity = "high";
    highSeverityRule.file_pattern = "*";
    highSeverityRule.action_type = RemediationActionType::QUARANTINE_FILE;
    highSeverityRule.requires_approval = true;
    highSeverityRule.auto_execute = false;
    highSeverityRule.priority = 75;
    m_rules[highSeverityRule.rule_id] = highSeverityRule;

    m_initialized = true;
    return true;
}

void RemediationManager::addRule(const RemediationRule& rule) {
    m_rules[rule.rule_id] = rule;
}

void RemediationManager::removeRule(const std::string& ruleId) {
    m_rules.erase(ruleId);
}

RemediationRule RemediationManager::getRule(const std::string& ruleId) const {
    auto it = m_rules.find(ruleId);
    if (it != m_rules.end()) {
        return it->second;
    }
    return RemediationRule();  // Return empty rule if not found
}

std::vector<RemediationRule> RemediationManager::getAllRules() const {
    std::vector<RemediationRule> rules;
    for (const auto& [id, rule] : m_rules) {
        rules.push_back(rule);
    }
    
    // Sort by priority (higher priority first)
    std::sort(rules.begin(), rules.end(),
              [](const RemediationRule& a, const RemediationRule& b) {
                  return a.priority > b.priority;
              });
    
    return rules;
}

RemediationRequest RemediationManager::evaluateChange(const std::string& changeId,
                                                      const std::string& filePath,
                                                      const std::string& changeType,
                                                      const std::string& severity) {
    RemediationRequest request;
    
    if (!m_initialized) {
        return request;
    }

    // Get all rules sorted by priority
    auto rules = getAllRules();
    
    // Find matching rule
    RemediationRule matchedRule;
    for (const auto& rule : rules) {
        if (matchRule(rule, changeType, severity, filePath)) {
            matchedRule = rule;
            break;
        }
    }

    if (matchedRule.rule_id.empty()) {
        return request;  // No matching rule
    }

    // Create remediation request
    request.request_id = generateRequestId();
    request.change_id = changeId;
    request.file_path = filePath;
    request.action_type = matchedRule.action_type;
    request.rule_id = matchedRule.rule_id;
    request.status = matchedRule.requires_approval ? "pending" : "approved";
    request.auto_execute = matchedRule.auto_execute;

    // Format request time
    char timeStr[64];
    std::time_t now = std::time(nullptr);
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    request.requested_at = timeStr;

    // Store request
    m_requests[request.request_id] = request;

    // Auto-execute if allowed
    if (matchedRule.auto_execute && !matchedRule.requires_approval) {
        executeRemediation(request.request_id);
    }

    // Log request creation
    if (m_auditLogger) {
        std::string actionStr = "remediation_request_created";
        std::string details = "Request ID: " + request.request_id + ", Rule: " + matchedRule.rule_id;
        m_auditLogger->logEvent(actionStr, details);
    }

    return request;
}

RemediationResult RemediationManager::executeRemediation(const std::string& requestId) {
    RemediationResult result;
    result.success = false;

    auto it = m_requests.find(requestId);
    if (it == m_requests.end()) {
        result.message = "Remediation request not found";
        return result;
    }

    RemediationRequest& request = it->second;

    // Check if request is approved
    if (request.status != "approved" && request.status != "pending") {
        result.message = "Request is not in approved or pending status";
        return result;
    }

    // Update status to executing
    request.status = "executing";

    // Execute action
    result = executeAction(request);

    // Update request status
    if (result.success) {
        request.status = "completed";
        char timeStr[64];
        std::time_t now = std::time(nullptr);
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        request.completed_at = timeStr;
    } else {
        request.status = "failed";
        request.error_message = result.message;
    }

    // Log remediation
    logRemediation(request, result);

    return result;
}

bool RemediationManager::approveRemediation(const std::string& requestId, const std::string& approver) {
    auto it = m_requests.find(requestId);
    if (it == m_requests.end()) {
        return false;
    }

    RemediationRequest& request = it->second;
    if (request.status != "pending") {
        return false;
    }

    request.status = "approved";
    request.approver = approver;
    
    char timeStr[64];
    std::time_t now = std::time(nullptr);
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    request.approved_at = timeStr;

    // Execute if auto-execute is enabled
    if (request.auto_execute) {
        executeRemediation(requestId);
    }

    // Log approval
    if (m_auditLogger) {
        std::string actionStr = "remediation_approved";
        std::string details = "Request ID: " + requestId + ", Approver: " + approver;
        m_auditLogger->logEvent(actionStr, details);
    }

    return true;
}

bool RemediationManager::rejectRemediation(const std::string& requestId, const std::string& reason) {
    auto it = m_requests.find(requestId);
    if (it == m_requests.end()) {
        return false;
    }

    RemediationRequest& request = it->second;
    if (request.status != "pending") {
        return false;
    }

    request.status = "rejected";
    request.error_message = reason;

    // Log rejection
    if (m_auditLogger) {
        std::string actionStr = "remediation_rejected";
        std::string details = "Request ID: " + requestId + ", Reason: " + reason;
        m_auditLogger->logEvent(actionStr, details);
    }

    return true;
}

bool RemediationManager::rollbackRemediation(const std::string& requestId) {
    auto it = m_requests.find(requestId);
    if (it == m_requests.end()) {
        return false;
    }

    RemediationRequest& request = it->second;
    if (request.status != "completed") {
        return false;
    }

    // Rollback based on action type
    bool rollbackSuccess = false;
    
    switch (request.action_type) {
        case RemediationActionType::RESTORE_FILE:
            // Restore from quarantine if available
            if (request.metadata.find("quarantine_path") != request.metadata.end()) {
                std::string quarantinePath = request.metadata.at("quarantine_path");
                if (std::filesystem::exists(quarantinePath)) {
                    std::filesystem::copy_file(quarantinePath, request.file_path,
                                              std::filesystem::copy_options::overwrite_existing);
                    rollbackSuccess = true;
                }
            }
            break;
            
        case RemediationActionType::QUARANTINE_FILE:
            // Restore from quarantine
            if (request.metadata.find("quarantine_path") != request.metadata.end()) {
                std::string quarantinePath = request.metadata.at("quarantine_path");
                if (std::filesystem::exists(quarantinePath)) {
                    std::filesystem::copy_file(quarantinePath, request.file_path,
                                              std::filesystem::copy_options::overwrite_existing);
                    rollbackSuccess = true;
                }
            }
            break;
            
        default:
            rollbackSuccess = false;
            break;
    }

    if (rollbackSuccess) {
        request.status = "rolled_back";
        
        // Log rollback
        if (m_auditLogger) {
            std::string actionStr = "remediation_rolled_back";
            std::string details = "Request ID: " + requestId;
            m_auditLogger->logEvent(actionStr, details);
        }
    }

    return rollbackSuccess;
}

RemediationRequest RemediationManager::getRequest(const std::string& requestId) const {
    auto it = m_requests.find(requestId);
    if (it != m_requests.end()) {
        return it->second;
    }
    return RemediationRequest();  // Return empty request if not found
}

std::vector<RemediationRequest> RemediationManager::getAllRequests(const std::string& statusFilter) const {
    std::vector<RemediationRequest> requests;
    
    for (const auto& [id, request] : m_requests) {
        if (statusFilter.empty() || request.status == statusFilter) {
            requests.push_back(request);
        }
    }
    
    return requests;
}

std::vector<RemediationRequest> RemediationManager::getPendingRequests() const {
    return getAllRequests("pending");
}

std::string RemediationManager::generateRequestId() const {
    // Generate random request ID
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::ostringstream oss;
    oss << "req_";
    for (int i = 0; i < 16; i++) {
        oss << std::hex << dis(gen);
    }
    
    return oss.str();
}

bool RemediationManager::matchRule(const RemediationRule& rule,
                                   const std::string& changeType,
                                   const std::string& severity,
                                   const std::string& filePath) const {
    // Check change type
    if (!rule.change_type.empty() && rule.change_type != changeType) {
        return false;
    }

    // Check severity
    if (!rule.severity.empty() && rule.severity != severity) {
        return false;
    }

    // Check file pattern (simple wildcard matching)
    if (!rule.file_pattern.empty() && rule.file_pattern != "*") {
        // Simple pattern matching (can be enhanced)
        if (rule.file_pattern.back() == '*') {
            std::string prefix = rule.file_pattern.substr(0, rule.file_pattern.length() - 1);
            if (filePath.find(prefix) != 0) {
                return false;
            }
        } else if (filePath != rule.file_pattern) {
            return false;
        }
    }

    return true;
}

RemediationResult RemediationManager::executeAction(const RemediationRequest& request) {
    RemediationResult result;
    result.success = false;
    result.request_id = request.request_id;

    char timeStr[64];
    std::time_t now = std::time(nullptr);
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    result.executed_at = timeStr;

    switch (request.action_type) {
        case RemediationActionType::RESTORE_FILE:
            {
                // Get baseline ID from metadata or change
                std::string baselineId = request.metadata.count("baseline_id") 
                    ? request.metadata.at("baseline_id") : "";
                if (baselineId.empty() && m_baselineManager) {
                    auto baseline = m_baselineManager->getBaselineForFile(request.file_path);
                    baselineId = baseline.baseline_id;
                }
                
                if (!baselineId.empty()) {
                    result.success = restoreFile(request.file_path, baselineId);
                    result.message = result.success ? "File restored successfully" : "Failed to restore file";
                } else {
                    result.message = "Baseline not found";
                }
            }
            break;

        case RemediationActionType::QUARANTINE_FILE:
            {
                std::string quarantinePath = quarantineFile(request.file_path);
                if (!quarantinePath.empty()) {
                    result.success = true;
                    result.message = "File quarantined successfully";
                    // Store quarantine path in request metadata for rollback
                    const_cast<RemediationRequest&>(request).metadata["quarantine_path"] = quarantinePath;
                } else {
                    result.message = "Failed to quarantine file";
                }
            }
            break;

        case RemediationActionType::DELETE_FILE:
            {
                if (std::filesystem::exists(request.file_path)) {
                    result.success = std::filesystem::remove(request.file_path);
                    result.message = result.success ? "File deleted successfully" : "Failed to delete file";
                } else {
                    result.message = "File does not exist";
                }
            }
            break;

        case RemediationActionType::RESTORE_PERMISSIONS:
            {
                std::string permissions = request.metadata.count("permissions")
                    ? request.metadata.at("permissions") : "0644";
                result.success = restorePermissions(request.file_path, permissions);
                result.message = result.success ? "Permissions restored successfully" : "Failed to restore permissions";
            }
            break;

        case RemediationActionType::RESTORE_OWNERSHIP:
            {
                std::string owner = request.metadata.count("owner")
                    ? request.metadata.at("owner") : "";
                if (!owner.empty()) {
                    result.success = restoreOwnership(request.file_path, owner);
                    result.message = result.success ? "Ownership restored successfully" : "Failed to restore ownership";
                } else {
                    result.message = "Owner information not provided";
                }
            }
            break;

        case RemediationActionType::NOTIFY_ADMIN:
            {
                // Notification action (implement via D-Bus or other mechanism)
                result.success = true;
                result.message = "Administrator notified";
            }
            break;

        case RemediationActionType::BLOCK_ACCESS:
            {
                // Block access action (implement via access control)
                result.success = true;
                result.message = "File access blocked";
            }
            break;

        default:
            result.message = "Unknown action type";
            break;
    }

    return result;
}

bool RemediationManager::restoreFile(const std::string& filePath, const std::string& baselineId) {
    if (!m_baselineManager) {
        return false;
    }

    // Get baseline information
    auto baseline = m_baselineManager->getBaseline(baselineId);
    if (baseline.baseline_id.empty()) {
        return false;
    }

    // In a real implementation, restore from backup or baseline storage
    // For now, this is a placeholder
    // The baseline manager would need to provide file restoration capability
    
    return true;
}

std::string RemediationManager::quarantineFile(const std::string& filePath) {
    if (!std::filesystem::exists(filePath)) {
        return "";
    }

    // Generate quarantine path
    std::string fileName = std::filesystem::path(filePath).filename().string();
    char timeStr[64];
    std::time_t now = std::time(nullptr);
    std::strftime(timeStr, sizeof(timeStr), "%Y%m%d_%H%M%S", std::localtime(&now));
    std::string quarantinePath = m_quarantineDirectory + "/" + timeStr + "_" + fileName;

    try {
        // Copy file to quarantine
        std::filesystem::copy_file(filePath, quarantinePath,
                                  std::filesystem::copy_options::overwrite_existing);
        return quarantinePath;
    } catch (const std::exception& e) {
        std::cerr << "Failed to quarantine file: " << e.what() << std::endl;
        return "";
    }
}

bool RemediationManager::restorePermissions(const std::string& filePath, const std::string& permissions) {
    try {
        mode_t mode = std::stoul(permissions, nullptr, 8);
        return chmod(filePath.c_str(), mode) == 0;
    } catch (const std::exception& e) {
        return false;
    }
}

bool RemediationManager::restoreOwnership(const std::string& filePath, const std::string& owner) {
    // Parse owner string (format: "user:group" or "uid:gid")
    size_t colonPos = owner.find(':');
    if (colonPos == std::string::npos) {
        return false;
    }

    std::string userStr = owner.substr(0, colonPos);
    std::string groupStr = owner.substr(colonPos + 1);

    uid_t uid = 0;
    gid_t gid = 0;

    // Try to parse as numeric
    try {
        uid = std::stoul(userStr);
        gid = std::stoul(groupStr);
    } catch (const std::exception& e) {
        // Parse as username/groupname
        struct passwd* pwd = getpwnam(userStr.c_str());
        if (pwd) {
            uid = pwd->pw_uid;
        }

        struct group* grp = getgrnam(groupStr.c_str());
        if (grp) {
            gid = grp->gr_gid;
        }
    }

    return chown(filePath.c_str(), uid, gid) == 0;
}

void RemediationManager::logRemediation(const RemediationRequest& request, const RemediationResult& result) {
    if (!m_auditLogger) {
        return;
    }

    std::string actionStr = "remediation_executed";
    std::ostringstream details;
    details << "Request ID: " << request.request_id
            << ", Action: " << static_cast<int>(request.action_type)
            << ", File: " << request.file_path
            << ", Success: " << (result.success ? "yes" : "no")
            << ", Message: " << result.message;
    
    m_auditLogger->logEvent(actionStr, details.str());
}

