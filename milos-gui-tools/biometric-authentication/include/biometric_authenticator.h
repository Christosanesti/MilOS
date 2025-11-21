#ifndef BIOMETRIC_AUTHENTICATOR_H
#define BIOMETRIC_AUTHENTICATOR_H

#include "biometric_device.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QVariantMap>

/**
 * @brief Authentication Result
 */
enum class AuthenticationResult {
    Success,
    Failed,
    NotEnrolled,
    DeviceError,
    Timeout
};

/**
 * @brief Biometric Authenticator
 * 
 * Manages biometric authentication with multiple devices.
 */
class BiometricAuthenticator : public QObject {
    Q_OBJECT

public:
    explicit BiometricAuthenticator(QObject* parent = nullptr);
    ~BiometricAuthenticator();

    /**
     * @brief Initialize biometric authenticator
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Add biometric device
     * @param device Biometric device
     * @return true if add successful, false otherwise
     */
    bool addDevice(BiometricDevice* device);

    /**
     * @brief Enroll user biometric
     * @param userId User ID
     * @param biometricType Biometric type
     * @return Template data or empty if failed
     */
    QByteArray enrollUser(const QString& userId, BiometricType biometricType);

    /**
     * @brief Authenticate user
     * @param userId User ID
     * @param biometricType Biometric type
     * @return Authentication result
     */
    AuthenticationResult authenticateUser(const QString& userId, BiometricType biometricType);

    /**
     * @brief Check if user is enrolled
     * @param userId User ID
     * @param biometricType Biometric type
     * @return true if enrolled, false otherwise
     */
    bool isUserEnrolled(const QString& userId, BiometricType biometricType) const;

    /**
     * @brief Get enrolled biometric types for user
     * @param userId User ID
     * @return List of biometric types
     */
    QList<BiometricType> getEnrolledTypes(const QString& userId) const;

Q_SIGNALS:
    /**
     * @brief Emitted when user is enrolled
     */
    void userEnrolled(const QString& userId, BiometricType biometricType);

    /**
     * @brief Emitted when authentication completes
     */
    void authenticationCompleted(const QString& userId, AuthenticationResult result);

private:
    QList<BiometricDevice*> m_devices;
    QMap<QString, QMap<BiometricType, QByteArray>> m_userTemplates;
    
    BiometricDevice* getDeviceForType(BiometricType type) const;
};

Q_DECLARE_METATYPE(AuthenticationResult)

#endif // BIOMETRIC_AUTHENTICATOR_H

