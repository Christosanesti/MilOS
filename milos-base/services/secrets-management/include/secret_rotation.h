#ifndef SECRET_ROTATION_H
#define SECRET_ROTATION_H

#include <string>
#include <map>
#include <memory>
#include <thread>
#include <atomic>
#include <chrono>

class SecretStorage;
class SecretVersioning;

/**
 * @brief Rotation Policy
 */
struct RotationPolicy {
    std::string secret_type;
    int interval_days;
    int notification_days;
    bool auto_rotate;
};

/**
 * @brief Secret Rotation
 * 
 * Manages automated secret rotation and expiration tracking.
 */
class SecretRotation {
public:
    SecretRotation();
    ~SecretRotation();

    /**
     * @brief Initialize rotation engine
     * @param storage Secret storage instance
     * @param versioning Secret versioning instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(SecretStorage* storage, SecretVersioning* versioning);

    /**
     * @brief Start rotation scheduler
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop rotation scheduler
     */
    void stop();

    /**
     * @brief Rotate secret
     * @param secretId Secret ID to rotate
     * @return true if rotation successful, false otherwise
     */
    bool rotateSecret(const std::string& secretId);

    /**
     * @brief Set rotation policy for secret type
     * @param policy Rotation policy
     */
    void setRotationPolicy(const RotationPolicy& policy);

    /**
     * @brief Get rotation policy for secret type
     * @param secretType Secret type
     * @return Rotation policy, or default if not found
     */
    RotationPolicy getRotationPolicy(const std::string& secretType) const;

    /**
     * @brief Check if secret needs rotation
     * @param secretId Secret ID
     * @return true if needs rotation, false otherwise
     */
    bool needsRotation(const std::string& secretId) const;

    /**
     * @brief Get days until expiration
     * @param secretId Secret ID
     * @return Days until expiration, or -1 if no expiration
     */
    int getDaysUntilExpiration(const std::string& secretId) const;

private:
    SecretStorage* m_storage;
    SecretVersioning* m_versioning;
    std::atomic<bool> m_running;
    std::unique_ptr<std::thread> m_rotationThread;
    std::map<std::string, RotationPolicy> m_policies;

    /**
     * @brief Rotation thread function
     */
    void rotationThreadFunction();

    /**
     * @brief Check and rotate secrets
     */
    void checkAndRotateSecrets();

    /**
     * @brief Generate new secret value
     * @param secretId Secret ID
     * @return New secret data
     */
    std::vector<uint8_t> generateNewSecret(const std::string& secretId) const;
};

#endif // SECRET_ROTATION_H

