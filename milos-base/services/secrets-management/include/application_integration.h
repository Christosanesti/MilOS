#ifndef APPLICATION_INTEGRATION_H
#define APPLICATION_INTEGRATION_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <mutex>

class SecretStorage;
class AccessControl;
class AuditLogger;

/**
 * @brief Cached Secret
 */
struct CachedSecret {
    std::string secret_id;
    std::vector<uint8_t> secret_data;
    std::string version;
    std::chrono::system_clock::time_point cached_at;
    std::chrono::system_clock::time_point expires_at;
    bool is_valid;
};

/**
 * @brief Application Identity
 */
struct ApplicationIdentity {
    std::string application_id;
    std::string application_name;
    std::string process_id;
    std::string user_id;
    std::string dbus_service_name;
    std::string certificate_hash;  // For certificate-based authentication
    bool is_authenticated;
    std::chrono::system_clock::time_point authenticated_at;
};

/**
 * @brief Secret Injection Request
 */
struct SecretInjectionRequest {
    std::string request_id;
    std::string application_id;
    std::string secret_id;
    std::string injection_method;  // "environment", "file", "memory", "dbus"
    std::string target_path;  // For file injection
    std::string status;  // "pending", "injected", "failed"
    std::string injected_at;
    std::string error_message;
};

/**
 * @brief Application Integration
 * 
 * Provides secure secret access for applications with caching,
 * injection, and authentication.
 */
class ApplicationIntegration {
public:
    ApplicationIntegration();
    ~ApplicationIntegration();

    /**
     * @brief Initialize application integration
     * @param storage Secret storage instance
     * @param accessControl Access control instance
     * @param auditLogger Audit logger instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(SecretStorage* storage, AccessControl* accessControl, AuditLogger* auditLogger);

    /**
     * @brief Authenticate application
     * @param applicationId Application ID
     * @param dbusServiceName D-Bus service name
     * @param certificateHash Certificate hash (optional)
     * @return Application identity, or empty if authentication failed
     */
    ApplicationIdentity authenticateApplication(const std::string& applicationId,
                                               const std::string& dbusServiceName,
                                               const std::string& certificateHash = "");

    /**
     * @brief Get secret for application (with caching)
     * @param applicationId Application ID
     * @param secretId Secret ID
     * @param version Version ID (empty for latest)
     * @return Secret data, or empty if not found or access denied
     */
    std::vector<uint8_t> getSecretForApplication(const std::string& applicationId,
                                                  const std::string& secretId,
                                                  const std::string& version = "");

    /**
     * @brief Inject secret into application
     * @param applicationId Application ID
     * @param secretId Secret ID
     * @param injectionMethod Injection method ("environment", "file", "memory", "dbus")
     * @param targetPath Target path (for file injection)
     * @return Secret injection request, or empty if failed
     */
    SecretInjectionRequest injectSecret(const std::string& applicationId,
                                       const std::string& secretId,
                                       const std::string& injectionMethod,
                                       const std::string& targetPath = "");

    /**
     * @brief Refresh cached secret
     * @param applicationId Application ID
     * @param secretId Secret ID
     * @return true if refresh successful, false otherwise
     */
    bool refreshCachedSecret(const std::string& applicationId, const std::string& secretId);

    /**
     * @brief Clear cached secret
     * @param applicationId Application ID
     * @param secretId Secret ID
     */
    void clearCachedSecret(const std::string& applicationId, const std::string& secretId);

    /**
     * @brief Clear all cached secrets for application
     * @param applicationId Application ID
     */
    void clearApplicationCache(const std::string& applicationId);

    /**
     * @brief Get cached secret
     * @param applicationId Application ID
     * @param secretId Secret ID
     * @return Cached secret, or empty if not cached
     */
    CachedSecret getCachedSecret(const std::string& applicationId, const std::string& secretId) const;

    /**
     * @brief Check if application is authenticated
     * @param applicationId Application ID
     * @return true if authenticated, false otherwise
     */
    bool isApplicationAuthenticated(const std::string& applicationId) const;

    /**
     * @brief Get application identity
     * @param applicationId Application ID
     * @return Application identity, or empty if not found
     */
    ApplicationIdentity getApplicationIdentity(const std::string& applicationId) const;

    /**
     * @brief Register application
     * @param applicationId Application ID
     * @param applicationName Application name
     * @param dbusServiceName D-Bus service name
     * @return true if registration successful, false otherwise
     */
    bool registerApplication(const std::string& applicationId,
                            const std::string& applicationName,
                            const std::string& dbusServiceName);

    /**
     * @brief Unregister application
     * @param applicationId Application ID
     */
    void unregisterApplication(const std::string& applicationId);

private:
    bool m_initialized;
    SecretStorage* m_storage;
    AccessControl* m_accessControl;
    AuditLogger* m_auditLogger;
    
    // Application registry
    std::map<std::string, ApplicationIdentity> m_applications;
    
    // Secret cache (per application)
    std::map<std::string, std::map<std::string, CachedSecret>> m_secretCache;
    
    // Injection requests
    std::map<std::string, SecretInjectionRequest> m_injectionRequests;
    
    mutable std::mutex m_cacheMutex;
    mutable std::mutex m_appMutex;

    /**
     * @brief Generate request ID
     * @return Request ID
     */
    std::string generateRequestId() const;

    /**
     * @brief Verify application certificate
     * @param certificateHash Certificate hash
     * @return true if certificate valid, false otherwise
     */
    bool verifyApplicationCertificate(const std::string& certificateHash) const;

    /**
     * @brief Inject secret into environment
     * @param applicationId Application ID
     * @param secretId Secret ID
     * @param secretData Secret data
     * @return true if injection successful, false otherwise
     */
    bool injectIntoEnvironment(const std::string& applicationId,
                               const std::string& secretId,
                               const std::vector<uint8_t>& secretData);

    /**
     * @brief Inject secret into file
     * @param targetPath Target file path
     * @param secretData Secret data
     * @return true if injection successful, false otherwise
     */
    bool injectIntoFile(const std::string& targetPath,
                       const std::vector<uint8_t>& secretData);

    /**
     * @brief Inject secret via D-Bus
     * @param applicationId Application ID
     * @param secretId Secret ID
     * @param secretData Secret data
     * @return true if injection successful, false otherwise
     */
    bool injectViaDBus(const std::string& applicationId,
                      const std::string& secretId,
                      const std::vector<uint8_t>& secretData);

    /**
     * @brief Clean expired cache entries
     */
    void cleanExpiredCache();
};

#endif // APPLICATION_INTEGRATION_H

