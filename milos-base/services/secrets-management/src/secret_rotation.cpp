#include "secret_rotation.h"
#include "secret_storage.h"
#include "secret_versioning.h"
#include <milos/logging/logger.h>
#include <openssl/rand.h>
#include <ctime>
#include <sstream>
#include <iomanip>

SecretRotation::SecretRotation()
    : m_storage(nullptr)
    , m_versioning(nullptr)
    , m_running(false)
{
    // Default policies
    RotationPolicy passwordPolicy;
    passwordPolicy.secret_type = "password";
    passwordPolicy.interval_days = 30;
    passwordPolicy.notification_days = 7;
    passwordPolicy.auto_rotate = true;
    m_policies["password"] = passwordPolicy;

    RotationPolicy apiKeyPolicy;
    apiKeyPolicy.secret_type = "api_key";
    apiKeyPolicy.interval_days = 60;
    apiKeyPolicy.notification_days = 7;
    apiKeyPolicy.auto_rotate = true;
    m_policies["api_key"] = apiKeyPolicy;

    RotationPolicy certPolicy;
    certPolicy.secret_type = "certificate";
    certPolicy.interval_days = 365;
    certPolicy.notification_days = 30;
    certPolicy.auto_rotate = false;
    m_policies["certificate"] = certPolicy;
}

SecretRotation::~SecretRotation() {
    stop();
}

bool SecretRotation::initialize(SecretStorage* storage, SecretVersioning* versioning) {
    if (!storage || !versioning) {
        return false;
    }

    m_storage = storage;
    m_versioning = versioning;
    return true;
}

bool SecretRotation::start() {
    if (m_running) {
        return true;
    }

    if (!m_storage || !m_versioning) {
        return false;
    }

    m_running = true;
    m_rotationThread = std::make_unique<std::thread>(&SecretRotation::rotationThreadFunction, this);
    return true;
}

void SecretRotation::stop() {
    if (!m_running) {
        return;
    }

    m_running = false;
    if (m_rotationThread && m_rotationThread->joinable()) {
        m_rotationThread->join();
    }
    m_rotationThread.reset();
}

bool SecretRotation::rotateSecret(const std::string& secretId) {
    if (!m_storage || !m_versioning) {
        return false;
    }

    // Get current secret metadata
    SecretMetadata metadata = m_storage->getSecretMetadata(secretId);
    if (metadata.secret_id.empty()) {
        return false;
    }

    // Get current secret data
    std::vector<uint8_t> currentSecret = m_storage->getSecret(secretId);
    if (currentSecret.empty()) {
        return false;
    }

    // Create new version before rotation
    if (!m_versioning->createVersion(secretId, currentSecret)) {
        LOG_ERROR(QString("Failed to create version before rotation for secret: %1").arg(QString::fromStdString(secretId)));
        return false;
    }

    // Generate new secret
    std::vector<uint8_t> newSecret = generateNewSecret(secretId);
    if (newSecret.empty()) {
        return false;
    }

    // Update expiration date
    std::time_t now = std::time(nullptr);
    RotationPolicy policy = getRotationPolicy(metadata.secret_type);
    std::time_t expiration = now + (policy.interval_days * 24 * 60 * 60);
    char expirationStr[64];
    std::strftime(expirationStr, sizeof(expirationStr), "%Y-%m-%d %H:%M:%S", std::localtime(&expiration));
    metadata.expires_at = expirationStr;

    // Store new secret
    if (!m_storage->storeSecret(secretId, newSecret, metadata)) {
        return false;
    }

    return true;
}

void SecretRotation::setRotationPolicy(const RotationPolicy& policy) {
    m_policies[policy.secret_type] = policy;
}

RotationPolicy SecretRotation::getRotationPolicy(const std::string& secretType) const {
    auto it = m_policies.find(secretType);
    if (it != m_policies.end()) {
        return it->second;
    }

    // Default policy
    RotationPolicy defaultPolicy;
    defaultPolicy.secret_type = secretType;
    defaultPolicy.interval_days = 90;
    defaultPolicy.notification_days = 7;
    defaultPolicy.auto_rotate = true;
    return defaultPolicy;
}

bool SecretRotation::needsRotation(const std::string& secretId) const {
    if (!m_storage) {
        return false;
    }

    SecretMetadata metadata = m_storage->getSecretMetadata(secretId);
    if (metadata.secret_id.empty() || metadata.expires_at.empty()) {
        return false;
    }

    // Parse expiration date
    std::tm tm = {};
    std::istringstream ss(metadata.expires_at);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        return false;
    }

    std::time_t expiration = std::mktime(&tm);
    std::time_t now = std::time(nullptr);

    return expiration <= now;
}

int SecretRotation::getDaysUntilExpiration(const std::string& secretId) const {
    if (!m_storage) {
        return -1;
    }

    SecretMetadata metadata = m_storage->getSecretMetadata(secretId);
    if (metadata.secret_id.empty() || metadata.expires_at.empty()) {
        return -1;
    }

    // Parse expiration date
    std::tm tm = {};
    std::istringstream ss(metadata.expires_at);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        return -1;
    }

    std::time_t expiration = std::mktime(&tm);
    std::time_t now = std::time(nullptr);
    double diff = std::difftime(expiration, now);
    int days = static_cast<int>(diff / (24 * 60 * 60));

    return days;
}

void SecretRotation::rotationThreadFunction() {
    while (m_running) {
        checkAndRotateSecrets();
        
        // Sleep for 1 hour
        std::this_thread::sleep_for(std::chrono::hours(1));
    }
}

void SecretRotation::checkAndRotateSecrets() {
    if (!m_storage) {
        return;
    }

    std::vector<std::string> secretIds = m_storage->listSecrets();
    for (const auto& secretId : secretIds) {
        if (!m_running) {
            break;
        }

        SecretMetadata metadata = m_storage->getSecretMetadata(secretId);
        RotationPolicy policy = getRotationPolicy(metadata.secret_type);

        if (policy.auto_rotate && needsRotation(secretId)) {
            LOG_INFO(QString("Auto-rotating secret: %1").arg(QString::fromStdString(secretId)));
            rotateSecret(secretId);
        } else {
            int daysUntilExpiration = getDaysUntilExpiration(secretId);
            if (daysUntilExpiration >= 0 && daysUntilExpiration <= policy.notification_days) {
                // Emit notification signal (via D-Bus)
                LOG_WARNING(QString("Secret expiring soon: %1 (%2 days)").arg(QString::fromStdString(secretId)).arg(daysUntilExpiration));
            }
        }
    }
}

std::vector<uint8_t> SecretRotation::generateNewSecret(const std::string& secretId) const {
    if (!m_storage) {
        return std::vector<uint8_t>();
    }

    SecretMetadata metadata = m_storage->getSecretMetadata(secretId);
    
    // Generate random secret based on type
    std::vector<uint8_t> newSecret;
    
    if (metadata.secret_type == "password") {
        // Generate 32-byte password
        newSecret.resize(32);
        RAND_bytes(newSecret.data(), 32);
    } else if (metadata.secret_type == "api_key") {
        // Generate 64-byte API key
        newSecret.resize(64);
        RAND_bytes(newSecret.data(), 64);
    } else {
        // Default: 32 bytes
        newSecret.resize(32);
        RAND_bytes(newSecret.data(), 32);
    }

    return newSecret;
}

