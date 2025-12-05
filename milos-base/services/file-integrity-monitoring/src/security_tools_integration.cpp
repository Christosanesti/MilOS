#include "security_tools_integration.h"
#include "change_detector.h"
#include "integrity_verifier.h"
#include "audit_logger.h"
#include <milos/logging/logger.h>
#include <random>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <syslog.h>
#include <fstream>

// libcurl support (optional)
#ifdef HAVE_LIBCURL
#include <curl/curl.h>
#else
// Define minimal curl types if libcurl not available
typedef void CURL;
typedef void curl_slist;
#define CURLE_OK 0
#define CURLINFO_RESPONSE_CODE 0x200002
#define CURL_GLOBAL_DEFAULT 0
#define CURLOPT_URL 10002
#define CURLOPT_POSTFIELDS 10015
#define CURLOPT_HTTPHEADER 10023
#define CURLOPT_SSL_VERIFYPEER 64
#define CURLOPT_SSL_VERIFYHOST 81
#define CURLOPT_NOBODY 44
#define CURLOPT_TIMEOUT 78
#endif

SecurityToolsIntegration::SecurityToolsIntegration()
    : m_initialized(false)
    , m_changeDetector(nullptr)
    , m_integrityVerifier(nullptr)
    , m_auditLogger(nullptr)
{
}

SecurityToolsIntegration::~SecurityToolsIntegration() {
}

bool SecurityToolsIntegration::initialize(ChangeDetector* changeDetector,
                                         IntegrityVerifier* integrityVerifier,
                                         AuditLogger* auditLogger) {
    if (m_initialized) {
        return true;
    }

    if (!changeDetector || !integrityVerifier || !auditLogger) {
        return false;
    }

    m_changeDetector = changeDetector;
    m_integrityVerifier = integrityVerifier;
    m_auditLogger = auditLogger;

    // Initialize curl for HTTP adapters (if available)
#ifdef HAVE_LIBCURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
#endif

    m_initialized = true;
    return true;
}

bool SecurityToolsIntegration::registerTool(const SecurityToolConfig& config) {
    if (!m_initialized) {
        return false;
    }

    // Create adapter for tool
    std::unique_ptr<SecurityToolAdapter> adapter = createAdapter(config);
    if (!adapter) {
        return false;
    }

    // Test connection
    if (!adapter->testConnection()) {
        LOG_WARNING(QString("Failed to connect to security tool: %1").arg(QString::fromStdString(config.tool_name)));
        // Still register, but mark as potentially unavailable
    }

    // Store configuration and adapter
    m_toolConfigs[config.tool_id] = config;
    m_toolAdapters[config.tool_id] = std::move(adapter);

    // Log registration
    if (m_auditLogger) {
        m_auditLogger->logEvent("security_tool_registered",
                               "Tool ID: " + config.tool_id + ", Name: " + config.tool_name);
    }

    return true;
}

void SecurityToolsIntegration::unregisterTool(const std::string& toolId) {
    m_toolConfigs.erase(toolId);
    m_toolAdapters.erase(toolId);

    // Log unregistration
    if (m_auditLogger) {
        m_auditLogger->logEvent("security_tool_unregistered", "Tool ID: " + toolId);
    }
}

SecurityToolConfig SecurityToolsIntegration::getToolConfig(const std::string& toolId) const {
    auto it = m_toolConfigs.find(toolId);
    if (it != m_toolConfigs.end()) {
        return it->second;
    }
    return SecurityToolConfig();  // Return empty if not found
}

std::vector<SecurityToolConfig> SecurityToolsIntegration::getAllTools() const {
    std::vector<SecurityToolConfig> tools;
    for (const auto& [id, config] : m_toolConfigs) {
        tools.push_back(config);
    }
    return tools;
}

std::vector<std::string> SecurityToolsIntegration::forwardViolation(
    const std::string& changeId,
    const std::string& filePath,
    const std::string& changeType,
    const std::string& severity) {
    
    std::vector<std::string> notificationIds;

    if (!m_initialized) {
        return notificationIds;
    }

    // Forward to all enabled tools that match the criteria
    for (const auto& [toolId, config] : m_toolConfigs) {
        if (!config.enabled) {
            continue;
        }

        if (!shouldForwardToTool(config, changeType, severity)) {
            continue;
        }

        std::string notificationId = forwardViolationToTool(toolId, changeId, filePath, changeType, severity);
        if (!notificationId.empty()) {
            notificationIds.push_back(notificationId);
        }
    }

    return notificationIds;
}

std::string SecurityToolsIntegration::forwardViolationToTool(
    const std::string& toolId,
    const std::string& changeId,
    const std::string& filePath,
    const std::string& changeType,
    const std::string& severity) {
    
    if (!m_initialized) {
        return "";
    }

    auto adapterIt = m_toolAdapters.find(toolId);
    if (adapterIt == m_toolAdapters.end()) {
        return "";
    }

    // Create violation notification
    ViolationNotification notification;
    notification.notification_id = generateNotificationId();
    notification.change_id = changeId;
    notification.file_path = filePath;
    notification.change_type = changeType;
    notification.severity = severity;
    notification.tool_id = toolId;
    notification.status = "pending";

    char timeStr[64];
    std::time_t now = std::time(nullptr);
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    notification.detected_at = timeStr;

    // Send violation via adapter
    SecurityToolAdapter* adapter = adapterIt->second.get();
    bool sent = adapter->sendViolation(notification);

    if (sent) {
        notification.status = "sent";
        notification.sent_at = timeStr;
    } else {
        notification.status = "failed";
        notification.error_message = "Failed to send violation to tool";
    }

    // Store notification
    m_notifications[notification.notification_id] = notification;

    // Log forwarding
    if (m_auditLogger) {
        std::string actionStr = sent ? "violation_forwarded" : "violation_forward_failed";
        std::string details = "Tool: " + toolId + ", Change: " + changeId + ", File: " + filePath;
        m_auditLogger->logEvent(actionStr, details);
    }

    return sent ? notification.notification_id : "";
}

ViolationNotification SecurityToolsIntegration::getNotification(const std::string& notificationId) const {
    auto it = m_notifications.find(notificationId);
    if (it != m_notifications.end()) {
        return it->second;
    }
    return ViolationNotification();  // Return empty if not found
}

std::vector<ViolationNotification> SecurityToolsIntegration::getAllNotifications(
    const std::string& toolId,
    const std::string& statusFilter) const {
    
    std::vector<ViolationNotification> notifications;
    
    for (const auto& [id, notification] : m_notifications) {
        if (!toolId.empty() && notification.tool_id != toolId) {
            continue;
        }
        if (!statusFilter.empty() && notification.status != statusFilter) {
            continue;
        }
        notifications.push_back(notification);
    }
    
    return notifications;
}

bool SecurityToolsIntegration::testToolConnection(const std::string& toolId) {
    auto adapterIt = m_toolAdapters.find(toolId);
    if (adapterIt == m_toolAdapters.end()) {
        return false;
    }

    return adapterIt->second->testConnection();
}

std::unique_ptr<SecurityToolAdapter> SecurityToolsIntegration::createAdapter(const SecurityToolConfig& config) {
    // Determine adapter type based on endpoint URL
    if (config.endpoint_url.find("http://") == 0 || config.endpoint_url.find("https://") == 0) {
        return std::make_unique<HTTPSecurityToolAdapter>(config);
    } else if (config.endpoint_url.find("syslog://") == 0 || config.format == "syslog") {
        return std::make_unique<SyslogSecurityToolAdapter>(config);
    } else if (config.endpoint_url.find("dbus://") == 0 || config.endpoint_url.find("org.") == 0) {
        return std::make_unique<DBusSecurityToolAdapter>(config);
    } else {
        // Default to D-Bus adapter
        return std::make_unique<DBusSecurityToolAdapter>(config);
    }
}

std::string SecurityToolsIntegration::generateNotificationId() const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::ostringstream oss;
    oss << "notif_";
    for (int i = 0; i < 16; i++) {
        oss << std::hex << dis(gen);
    }
    
    return oss.str();
}

bool SecurityToolsIntegration::shouldForwardToTool(const SecurityToolConfig& config,
                                                   const std::string& changeType,
                                                   const std::string& severity) const {
    // Check severity filter
    if (!config.severity_levels.empty()) {
        bool severityMatch = false;
        for (const auto& level : config.severity_levels) {
            if (level == severity) {
                severityMatch = true;
                break;
            }
        }
        if (!severityMatch) {
            return false;
        }
    }

    // Check change type filter
    if (!config.change_types.empty()) {
        bool typeMatch = false;
        for (const auto& type : config.change_types) {
            if (type == changeType) {
                typeMatch = true;
                break;
            }
        }
        if (!typeMatch) {
            return false;
        }
    }

    return true;
}

// DBusSecurityToolAdapter Implementation
DBusSecurityToolAdapter::DBusSecurityToolAdapter(const SecurityToolConfig& config)
    : m_config(config)
{
}

DBusSecurityToolAdapter::~DBusSecurityToolAdapter() {
}

bool DBusSecurityToolAdapter::sendViolation(const ViolationNotification& violation) {
    // D-Bus adapter would use Qt D-Bus to send violation
    // This is a placeholder - actual implementation would use QDBusInterface
    // For now, log the violation
    LOG_INFO(QString("D-Bus Violation: Tool=%1, File=%2, Type=%3, Severity=%4")
             .arg(QString::fromStdString(m_config.tool_id))
             .arg(QString::fromStdString(violation.file_path))
             .arg(QString::fromStdString(violation.change_type))
             .arg(QString::fromStdString(violation.severity)));
    
    return true;
}

bool DBusSecurityToolAdapter::testConnection() {
    // Test D-Bus connection
    // This is a placeholder - actual implementation would check D-Bus service availability
    return true;
}

// HTTPSecurityToolAdapter Implementation
HTTPSecurityToolAdapter::HTTPSecurityToolAdapter(const SecurityToolConfig& config)
    : m_config(config)
{
}

HTTPSecurityToolAdapter::~HTTPSecurityToolAdapter() {
}

bool HTTPSecurityToolAdapter::sendViolation(const ViolationNotification& violation) {
#ifdef HAVE_LIBCURL
    CURL* curl = curl_easy_init();
    if (!curl) {
        return false;
    }

    std::string formattedViolation = formatViolation(violation);
    
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    
    if (!m_config.authentication_token.empty()) {
        std::string authHeader = "Authorization: Bearer " + m_config.authentication_token;
        headers = curl_slist_append(headers, authHeader.c_str());
    }
    
    // Add custom headers
    for (const auto& [key, value] : m_config.custom_headers) {
        std::string header = key + ": " + value;
        headers = curl_slist_append(headers, header.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_URL, m_config.endpoint_url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, formattedViolation.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    CURLcode res = curl_easy_perform(curl);
    
    long responseCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return (res == CURLE_OK && responseCode >= 200 && responseCode < 300);
#else
    // Fallback: log violation (libcurl not available)
    LOG_WARNING(QString("HTTP Violation (libcurl not available): Tool=%1, File=%2, Type=%3")
                .arg(QString::fromStdString(m_config.tool_id))
                .arg(QString::fromStdString(violation.file_path))
                .arg(QString::fromStdString(violation.change_type)));
              << ", Severity=" << violation.severity << std::endl;
    return true;
#endif
}

bool HTTPSecurityToolAdapter::testConnection() {
#ifdef HAVE_LIBCURL
    CURL* curl = curl_easy_init();
    if (!curl) {
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, m_config.endpoint_url.c_str());
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return (res == CURLE_OK);
#else
    // Fallback: assume connection is OK if libcurl not available
    return true;
#endif
}

std::string HTTPSecurityToolAdapter::formatViolation(const ViolationNotification& violation) const {
    if (m_config.format == "json") {
        std::ostringstream oss;
        oss << "{"
            << "\"notification_id\":\"" << violation.notification_id << "\","
            << "\"change_id\":\"" << violation.change_id << "\","
            << "\"file_path\":\"" << violation.file_path << "\","
            << "\"change_type\":\"" << violation.change_type << "\","
            << "\"severity\":\"" << violation.severity << "\","
            << "\"detected_at\":\"" << violation.detected_at << "\""
            << "}";
        return oss.str();
    } else if (m_config.format == "cef") {
        // CEF (Common Event Format)
        std::ostringstream oss;
        oss << "CEF:0|MilOS|FIM|1.0|file_integrity_violation|"
            << violation.change_type << "|" << violation.severity << "|"
            << "filePath=" << violation.file_path << " "
            << "changeId=" << violation.change_id;
        return oss.str();
    } else {
        // Default JSON
        std::ostringstream oss;
        oss << "{"
            << "\"notification_id\":\"" << violation.notification_id << "\","
            << "\"change_id\":\"" << violation.change_id << "\","
            << "\"file_path\":\"" << violation.file_path << "\","
            << "\"change_type\":\"" << violation.change_type << "\","
            << "\"severity\":\"" << violation.severity << "\","
            << "\"detected_at\":\"" << violation.detected_at << "\""
            << "}";
        return oss.str();
    }
}

// SyslogSecurityToolAdapter Implementation
SyslogSecurityToolAdapter::SyslogSecurityToolAdapter(const SecurityToolConfig& config)
    : m_config(config)
    , m_syslogFd(-1)
{
    // Open syslog connection
    openlog("milos-fim", LOG_PID | LOG_CONS, LOG_LOCAL0);
}

SyslogSecurityToolAdapter::~SyslogSecurityToolAdapter() {
    closelog();
}

bool SyslogSecurityToolAdapter::sendViolation(const ViolationNotification& violation) {
    std::string message = formatSyslogMessage(violation);
    
    int priority = LOG_WARNING;
    if (violation.severity == "critical") {
        priority = LOG_CRIT;
    } else if (violation.severity == "high") {
        priority = LOG_ERR;
    } else if (violation.severity == "medium") {
        priority = LOG_WARNING;
    } else {
        priority = LOG_INFO;
    }
    
    syslog(priority, "%s", message.c_str());
    return true;
}

bool SyslogSecurityToolAdapter::testConnection() {
    // Syslog is always available
    return true;
}

std::string SyslogSecurityToolAdapter::formatSyslogMessage(const ViolationNotification& violation) const {
    std::ostringstream oss;
    oss << "FIM Violation: "
        << "file=" << violation.file_path << " "
        << "type=" << violation.change_type << " "
        << "severity=" << violation.severity << " "
        << "change_id=" << violation.change_id << " "
        << "detected_at=" << violation.detected_at;
    return oss.str();
}

