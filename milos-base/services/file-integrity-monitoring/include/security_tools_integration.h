#ifndef SECURITY_TOOLS_INTEGRATION_H
#define SECURITY_TOOLS_INTEGRATION_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

class ChangeDetector;
class IntegrityVerifier;
class AuditLogger;

/**
 * @brief Security Tool Type
 */
enum class SecurityToolType {
    SIEM,           // Security Information and Event Management
    IDS,            // Intrusion Detection System
    SOAR,           // Security Orchestration, Automation, and Response
    THREAT_INTEL,   // Threat Intelligence Platform
    LOG_AGGREGATOR, // Log Aggregation System
    CUSTOM          // Custom security tool
};

/**
 * @brief Security Tool Configuration
 */
struct SecurityToolConfig {
    std::string tool_id;
    std::string tool_name;
    SecurityToolType tool_type;
    std::string endpoint_url;  // API endpoint or D-Bus service name
    std::string authentication_token;  // API token or authentication method
    bool enabled;
    std::vector<std::string> severity_levels;  // Which severity levels to forward
    std::vector<std::string> change_types;  // Which change types to forward
    std::map<std::string, std::string> custom_headers;  // Custom HTTP headers
    std::string format;  // "json", "syslog", "cef", "leef"
};

/**
 * @brief Violation Notification
 */
struct ViolationNotification {
    std::string notification_id;
    std::string change_id;
    std::string file_path;
    std::string change_type;
    std::string severity;
    std::string detected_at;
    std::string tool_id;
    std::string status;  // "pending", "sent", "failed", "acknowledged"
    std::string sent_at;
    std::string error_message;
    std::string tool_response;
};

/**
 * @brief Security Tool Integration Adapter Interface
 */
class SecurityToolAdapter {
public:
    virtual ~SecurityToolAdapter() = default;
    
    /**
     * @brief Send violation notification to security tool
     * @param violation Violation details
     * @return true if sent successfully, false otherwise
     */
    virtual bool sendViolation(const ViolationNotification& violation) = 0;
    
    /**
     * @brief Test connection to security tool
     * @return true if connection successful, false otherwise
     */
    virtual bool testConnection() = 0;
    
    /**
     * @brief Get tool configuration
     * @return Tool configuration
     */
    virtual SecurityToolConfig getConfig() const = 0;
};

/**
 * @brief Security Tools Integration
 * 
 * Integrates FIM with security tools (SIEM, IDS, SOAR, etc.)
 * to forward integrity violations.
 */
class SecurityToolsIntegration {
public:
    SecurityToolsIntegration();
    ~SecurityToolsIntegration();

    /**
     * @brief Initialize security tools integration
     * @param changeDetector Change detector instance
     * @param integrityVerifier Integrity verifier instance
     * @param auditLogger Audit logger instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(ChangeDetector* changeDetector,
                   IntegrityVerifier* integrityVerifier,
                   AuditLogger* auditLogger);

    /**
     * @brief Register security tool
     * @param config Security tool configuration
     * @return true if registration successful, false otherwise
     */
    bool registerTool(const SecurityToolConfig& config);

    /**
     * @brief Unregister security tool
     * @param toolId Tool ID
     */
    void unregisterTool(const std::string& toolId);

    /**
     * @brief Get security tool configuration
     * @param toolId Tool ID
     * @return Security tool configuration, or empty if not found
     */
    SecurityToolConfig getToolConfig(const std::string& toolId) const;

    /**
     * @brief Get all registered tools
     * @return List of tool configurations
     */
    std::vector<SecurityToolConfig> getAllTools() const;

    /**
     * @brief Forward violation to security tools
     * @param changeId Change ID
     * @param filePath File path
     * @param changeType Change type
     * @param severity Severity level
     * @return List of notification IDs
     */
    std::vector<std::string> forwardViolation(const std::string& changeId,
                                              const std::string& filePath,
                                              const std::string& changeType,
                                              const std::string& severity);

    /**
     * @brief Forward violation to specific tool
     * @param toolId Tool ID
     * @param changeId Change ID
     * @param filePath File path
     * @param changeType Change type
     * @param severity Severity level
     * @return Notification ID, or empty if failed
     */
    std::string forwardViolationToTool(const std::string& toolId,
                                      const std::string& changeId,
                                      const std::string& filePath,
                                      const std::string& changeType,
                                      const std::string& severity);

    /**
     * @brief Get violation notification
     * @param notificationId Notification ID
     * @return Violation notification, or empty if not found
     */
    ViolationNotification getNotification(const std::string& notificationId) const;

    /**
     * @brief Get all notifications
     * @param toolId Tool ID filter (optional)
     * @param statusFilter Status filter (optional)
     * @return List of notifications
     */
    std::vector<ViolationNotification> getAllNotifications(const std::string& toolId = "",
                                                          const std::string& statusFilter = "") const;

    /**
     * @brief Test tool connection
     * @param toolId Tool ID
     * @return true if connection successful, false otherwise
     */
    bool testToolConnection(const std::string& toolId);

private:
    bool m_initialized;
    ChangeDetector* m_changeDetector;
    IntegrityVerifier* m_integrityVerifier;
    AuditLogger* m_auditLogger;
    
    std::map<std::string, SecurityToolConfig> m_toolConfigs;
    std::map<std::string, std::unique_ptr<SecurityToolAdapter>> m_toolAdapters;
    std::map<std::string, ViolationNotification> m_notifications;

    /**
     * @brief Create adapter for security tool
     * @param config Tool configuration
     * @return Adapter instance, or nullptr if creation failed
     */
    std::unique_ptr<SecurityToolAdapter> createAdapter(const SecurityToolConfig& config);

    /**
     * @brief Generate notification ID
     * @return Notification ID
     */
    std::string generateNotificationId() const;

    /**
     * @brief Check if violation should be forwarded to tool
     * @param config Tool configuration
     * @param changeType Change type
     * @param severity Severity level
     * @return true if should forward, false otherwise
     */
    bool shouldForwardToTool(const SecurityToolConfig& config,
                            const std::string& changeType,
                            const std::string& severity) const;
};

/**
 * @brief D-Bus Security Tool Adapter
 */
class DBusSecurityToolAdapter : public SecurityToolAdapter {
public:
    DBusSecurityToolAdapter(const SecurityToolConfig& config);
    ~DBusSecurityToolAdapter() override;

    bool sendViolation(const ViolationNotification& violation) override;
    bool testConnection() override;
    SecurityToolConfig getConfig() const override { return m_config; }

private:
    SecurityToolConfig m_config;
};

/**
 * @brief HTTP/REST API Security Tool Adapter
 */
class HTTPSecurityToolAdapter : public SecurityToolAdapter {
public:
    HTTPSecurityToolAdapter(const SecurityToolConfig& config);
    ~HTTPSecurityToolAdapter() override;

    bool sendViolation(const ViolationNotification& violation) override;
    bool testConnection() override;
    SecurityToolConfig getConfig() const override { return m_config; }

private:
    SecurityToolConfig m_config;
    
    /**
     * @brief Format violation for tool
     * @param violation Violation notification
     * @return Formatted string
     */
    std::string formatViolation(const ViolationNotification& violation) const;
};

/**
 * @brief Syslog Security Tool Adapter
 */
class SyslogSecurityToolAdapter : public SecurityToolAdapter {
public:
    SyslogSecurityToolAdapter(const SecurityToolConfig& config);
    ~SyslogSecurityToolAdapter() override;

    bool sendViolation(const ViolationNotification& violation) override;
    bool testConnection() override;
    SecurityToolConfig getConfig() const override { return m_config; }

private:
    SecurityToolConfig m_config;
    int m_syslogFd;
    
    /**
     * @brief Format violation as syslog message
     * @param violation Violation notification
     * @return Syslog message
     */
    std::string formatSyslogMessage(const ViolationNotification& violation) const;
};

#endif // SECURITY_TOOLS_INTEGRATION_H

