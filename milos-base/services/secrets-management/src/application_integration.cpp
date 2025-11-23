#include "application_integration.h"
#include "secret_storage.h"
#include "access_control.h"
#include "audit_logger.h"
#include <random>
#include <fstream>
#include <filesystem>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <iostream>

ApplicationIntegration::ApplicationIntegration()
    : m_initialized(false)
    , m_storage(nullptr)
    , m_accessControl(nullptr)
    , m_auditLogger(nullptr)
{
}

ApplicationIntegration::~ApplicationIntegration() {
}

bool ApplicationIntegration::initialize(SecretStorage* storage,
                                       AccessControl* accessControl,
                                       AuditLogger* auditLogger) {
    if (m_initialized) {
        return true;
    }

    if (!storage || !accessControl || !auditLogger) {
        return false;
    }

    m_storage = storage;
    m_accessControl = accessControl;
    m_auditLogger = auditLogger;

    m_initialized = true;
    return true;
}

ApplicationIdentity ApplicationIntegration::authenticateApplication(
    const std::string& applicationId,
    const std::string& dbusServiceName,
    const std::string& certificateHash) {
    
    ApplicationIdentity identity;
    
    if (!m_initialized) {
        return identity;
    }

    // Check if application is registered
    {
        std::lock_guard<std::mutex> lock(m_appMutex);
        auto it = m_applications.find(applicationId);
        if (it != m_applications.end()) {
            identity = it->second;
            // Re-authenticate
            if (!certificateHash.empty()) {
                identity.is_authenticated = verifyApplicationCertificate(certificateHash);
            } else {
                // Basic authentication (D-Bus service name verification)
                identity.is_authenticated = (identity.dbus_service_name == dbusServiceName);
            }
            
            if (identity.is_authenticated) {
                identity.authenticated_at = std::chrono::system_clock::now();
            }
            
            m_applications[applicationId] = identity;
        } else {
            // New application - register and authenticate
            identity.application_id = applicationId;
            identity.dbus_service_name = dbusServiceName;
            identity.certificate_hash = certificateHash;
            identity.process_id = std::to_string(getpid());
            
            // Get user ID
            struct passwd* pwd = getpwuid(getuid());
            if (pwd) {
                identity.user_id = pwd->pw_name;
            }
            
            if (!certificateHash.empty()) {
                identity.is_authenticated = verifyApplicationCertificate(certificateHash);
            } else {
                // Basic authentication
                identity.is_authenticated = true;  // Trust D-Bus service name
            }
            
            if (identity.is_authenticated) {
                identity.authenticated_at = std::chrono::system_clock::now();
            }
            
            m_applications[applicationId] = identity;
        }
    }

    // Log authentication
    if (m_auditLogger) {
        std::string actionStr = identity.is_authenticated ? "application_authenticated" : "application_authentication_failed";
        std::string details = "Application ID: " + applicationId + ", D-Bus Service: " + dbusServiceName;
        m_auditLogger->logEvent(actionStr, details);
    }

    return identity;
}

std::vector<uint8_t> ApplicationIntegration::getSecretForApplication(
    const std::string& applicationId,
    const std::string& secretId,
    const std::string& version) {
    
    if (!m_initialized || !m_storage || !m_accessControl) {
        return std::vector<uint8_t>();
    }

    // Check authentication
    if (!isApplicationAuthenticated(applicationId)) {
        if (m_auditLogger) {
            m_auditLogger->logEvent("secret_access_denied", "Application not authenticated: " + applicationId);
        }
        return std::vector<uint8_t>();
    }

    // Check access control
    ApplicationIdentity identity = getApplicationIdentity(applicationId);
    if (!m_accessControl->hasPermission(identity.user_id, secretId, Permission::Read)) {
        if (m_auditLogger) {
            m_auditLogger->logEvent("secret_access_denied", "Access denied for application: " + applicationId + ", secret: " + secretId);
        }
        return std::vector<uint8_t>();
    }

    // Check cache first
    {
        std::lock_guard<std::mutex> lock(m_cacheMutex);
        auto appCacheIt = m_secretCache.find(applicationId);
        if (appCacheIt != m_secretCache.end()) {
            auto secretCacheIt = appCacheIt->second.find(secretId);
            if (secretCacheIt != appCacheIt->second.end()) {
                CachedSecret& cached = secretCacheIt->second;
                
                // Check if cache is still valid
                auto now = std::chrono::system_clock::now();
                if (now < cached.expires_at && cached.is_valid) {
                    // Return cached secret
                    if (m_auditLogger) {
                        m_auditLogger->logEvent("secret_accessed_cached", "Application: " + applicationId + ", Secret: " + secretId);
                    }
                    return cached.secret_data;
                } else {
                    // Cache expired, remove it
                    appCacheIt->second.erase(secretCacheIt);
                }
            }
        }
    }

    // Get secret from storage
    std::vector<uint8_t> secretData;
    if (version.empty()) {
        secretData = m_storage->getSecret(secretId);
    } else {
        // Get specific version (would need versioning support)
        secretData = m_storage->getSecret(secretId);
    }

    if (secretData.empty()) {
        return std::vector<uint8_t>();
    }

    // Cache the secret (with 1 hour expiration by default)
    {
        std::lock_guard<std::mutex> lock(m_cacheMutex);
        CachedSecret cached;
        cached.secret_id = secretId;
        cached.secret_data = secretData;
        cached.version = version;
        cached.cached_at = std::chrono::system_clock::now();
        cached.expires_at = cached.cached_at + std::chrono::hours(1);
        cached.is_valid = true;
        
        m_secretCache[applicationId][secretId] = cached;
    }

    // Log access
    if (m_auditLogger) {
        m_auditLogger->logEvent("secret_accessed", "Application: " + applicationId + ", Secret: " + secretId);
    }

    return secretData;
}

SecretInjectionRequest ApplicationIntegration::injectSecret(
    const std::string& applicationId,
    const std::string& secretId,
    const std::string& injectionMethod,
    const std::string& targetPath) {
    
    SecretInjectionRequest request;
    
    if (!m_initialized) {
        request.error_message = "Application integration not initialized";
        return request;
    }

    // Check authentication
    if (!isApplicationAuthenticated(applicationId)) {
        request.error_message = "Application not authenticated";
        return request;
    }

    // Get secret
    std::vector<uint8_t> secretData = getSecretForApplication(applicationId, secretId);
    if (secretData.empty()) {
        request.error_message = "Secret not found or access denied";
        return request;
    }

    // Create injection request
    request.request_id = generateRequestId();
    request.application_id = applicationId;
    request.secret_id = secretId;
    request.injection_method = injectionMethod;
    request.target_path = targetPath;
    request.status = "pending";

    // Perform injection
    bool injectionSuccess = false;
    if (injectionMethod == "environment") {
        injectionSuccess = injectIntoEnvironment(applicationId, secretId, secretData);
    } else if (injectionMethod == "file") {
        if (targetPath.empty()) {
            request.error_message = "Target path required for file injection";
            return request;
        }
        injectionSuccess = injectIntoFile(targetPath, secretData);
    } else if (injectionMethod == "dbus") {
        injectionSuccess = injectViaDBus(applicationId, secretId, secretData);
    } else {
        request.error_message = "Unknown injection method: " + injectionMethod;
        return request;
    }

    if (injectionSuccess) {
        request.status = "injected";
        char timeStr[64];
        std::time_t now = std::time(nullptr);
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        request.injected_at = timeStr;
    } else {
        request.status = "failed";
        request.error_message = "Injection failed";
    }

    // Store request
    {
        std::lock_guard<std::mutex> lock(m_appMutex);
        m_injectionRequests[request.request_id] = request;
    }

    // Log injection
    if (m_auditLogger) {
        std::string actionStr = injectionSuccess ? "secret_injected" : "secret_injection_failed";
        std::string details = "Application: " + applicationId + ", Secret: " + secretId + ", Method: " + injectionMethod;
        m_auditLogger->logEvent(actionStr, details);
    }

    return request;
}

bool ApplicationIntegration::refreshCachedSecret(const std::string& applicationId, const std::string& secretId) {
    if (!m_initialized || !m_storage) {
        return false;
    }

    // Clear cache and re-fetch
    clearCachedSecret(applicationId, secretId);
    std::vector<uint8_t> secretData = getSecretForApplication(applicationId, secretId);
    
    return !secretData.empty();
}

void ApplicationIntegration::clearCachedSecret(const std::string& applicationId, const std::string& secretId) {
    std::lock_guard<std::mutex> lock(m_cacheMutex);
    auto appCacheIt = m_secretCache.find(applicationId);
    if (appCacheIt != m_secretCache.end()) {
        appCacheIt->second.erase(secretId);
    }
}

void ApplicationIntegration::clearApplicationCache(const std::string& applicationId) {
    std::lock_guard<std::mutex> lock(m_cacheMutex);
    m_secretCache.erase(applicationId);
}

CachedSecret ApplicationIntegration::getCachedSecret(const std::string& applicationId, const std::string& secretId) const {
    std::lock_guard<std::mutex> lock(m_cacheMutex);
    auto appCacheIt = m_secretCache.find(applicationId);
    if (appCacheIt != m_secretCache.end()) {
        auto secretCacheIt = appCacheIt->second.find(secretId);
        if (secretCacheIt != appCacheIt->second.end()) {
            return secretCacheIt->second;
        }
    }
    return CachedSecret();  // Return empty if not found
}

bool ApplicationIntegration::isApplicationAuthenticated(const std::string& applicationId) const {
    std::lock_guard<std::mutex> lock(m_appMutex);
    auto it = m_applications.find(applicationId);
    if (it != m_applications.end()) {
        return it->second.is_authenticated;
    }
    return false;
}

ApplicationIdentity ApplicationIntegration::getApplicationIdentity(const std::string& applicationId) const {
    std::lock_guard<std::mutex> lock(m_appMutex);
    auto it = m_applications.find(applicationId);
    if (it != m_applications.end()) {
        return it->second;
    }
    return ApplicationIdentity();  // Return empty if not found
}

bool ApplicationIntegration::registerApplication(const std::string& applicationId,
                                                const std::string& applicationName,
                                                const std::string& dbusServiceName) {
    if (!m_initialized) {
        return false;
    }

    ApplicationIdentity identity;
    identity.application_id = applicationId;
    identity.application_name = applicationName;
    identity.dbus_service_name = dbusServiceName;
    identity.process_id = std::to_string(getpid());
    
    struct passwd* pwd = getpwuid(getuid());
    if (pwd) {
        identity.user_id = pwd->pw_name;
    }
    
    identity.is_authenticated = false;

    {
        std::lock_guard<std::mutex> lock(m_appMutex);
        m_applications[applicationId] = identity;
    }

    // Log registration
    if (m_auditLogger) {
        m_auditLogger->logEvent("application_registered", "Application ID: " + applicationId + ", Name: " + applicationName);
    }

    return true;
}

void ApplicationIntegration::unregisterApplication(const std::string& applicationId) {
    {
        std::lock_guard<std::mutex> lock(m_appMutex);
        m_applications.erase(applicationId);
    }

    // Clear cache
    clearApplicationCache(applicationId);

    // Log unregistration
    if (m_auditLogger) {
        m_auditLogger->logEvent("application_unregistered", "Application ID: " + applicationId);
    }
}

std::string ApplicationIntegration::generateRequestId() const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::ostringstream oss;
    oss << "inj_";
    for (int i = 0; i < 16; i++) {
        oss << std::hex << dis(gen);
    }
    
    return oss.str();
}

bool ApplicationIntegration::verifyApplicationCertificate(const std::string& certificateHash) const {
    // In production, verify certificate against trusted certificate store
    // For now, return true if hash is provided (basic validation)
    return !certificateHash.empty();
}

bool ApplicationIntegration::injectIntoEnvironment(const std::string& applicationId,
                                                    const std::string& secretId,
                                                    const std::vector<uint8_t>& secretData) {
    // Environment injection would typically be done via D-Bus or process communication
    // This is a placeholder - actual implementation would require process communication
    // For now, log the injection request
    if (m_auditLogger) {
        m_auditLogger->logEvent("secret_injection_environment", 
                                "Application: " + applicationId + ", Secret: " + secretId);
    }
    return true;
}

bool ApplicationIntegration::injectIntoFile(const std::string& targetPath,
                                           const std::vector<uint8_t>& secretData) {
    try {
        // Create parent directory if needed
        std::filesystem::create_directories(std::filesystem::path(targetPath).parent_path());
        
        // Write secret to file with restricted permissions
        std::ofstream file(targetPath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        file.write(reinterpret_cast<const char*>(secretData.data()), secretData.size());
        file.close();
        
        // Set restrictive permissions (600)
        std::filesystem::permissions(targetPath,
                                     std::filesystem::perms::owner_read | std::filesystem::perms::owner_write);
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to inject secret into file: " << e.what() << std::endl;
        return false;
    }
}

bool ApplicationIntegration::injectViaDBus(const std::string& applicationId,
                                           const std::string& secretId,
                                           const std::vector<uint8_t>& secretData) {
    // D-Bus injection would send secret via D-Bus signal or method call
    // This is a placeholder - actual implementation would use Qt D-Bus
    if (m_auditLogger) {
        m_auditLogger->logEvent("secret_injection_dbus",
                                "Application: " + applicationId + ", Secret: " + secretId);
    }
    return true;
}

void ApplicationIntegration::cleanExpiredCache() {
    std::lock_guard<std::mutex> lock(m_cacheMutex);
    auto now = std::chrono::system_clock::now();
    
    for (auto& [appId, appCache] : m_secretCache) {
        for (auto it = appCache.begin(); it != appCache.end();) {
            if (now >= it->second.expires_at) {
                it = appCache.erase(it);
            } else {
                ++it;
            }
        }
    }
}

