#ifndef BIOMETRIC_ABSTRACTION_H
#define BIOMETRIC_ABSTRACTION_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QList>
#include <memory>

/**
 * @brief Biometric Device Type
 */
enum class BiometricDeviceType {
    FaceRecognition,
    Fingerprint
};

/**
 * @brief Device Status
 */
enum class DeviceStatus {
    Connected,
    Disconnected,
    Error,
    Initializing
};

/**
 * @brief Device Health Status
 */
struct DeviceHealth {
    DeviceStatus status;
    QString errorMessage;
    int healthScore;  // 0-100
    QVariantMap metrics;
};

/**
 * @brief Biometric Device Interface
 * 
 * Abstract base class for biometric hardware devices.
 */
class IBiometricDevice : public QObject {
    Q_OBJECT

public:
    virtual ~IBiometricDevice() = default;

    /**
     * @brief Get device ID
     */
    virtual QString deviceId() const = 0;

    /**
     * @brief Get device type
     */
    virtual BiometricDeviceType deviceType() const = 0;

    /**
     * @brief Get device vendor
     */
    virtual QString vendor() const = 0;

    /**
     * @brief Get device model
     */
    virtual QString model() const = 0;

    /**
     * @brief Initialize device
     */
    virtual bool initialize() = 0;

    /**
     * @brief Check if device is connected
     */
    virtual bool isConnected() const = 0;

    /**
     * @brief Get device health status
     */
    virtual DeviceHealth getHealth() const = 0;

    /**
     * @brief Capture biometric data
     * @param timeoutMs Timeout in milliseconds
     * @return Captured biometric data (format depends on device type)
     */
    virtual QByteArray capture(int timeoutMs = 5000) = 0;

    /**
     * @brief Verify biometric data
     * @param templateData Template data to verify against
     * @param capturedData Captured biometric data
     * @return Match score (0-100)
     */
    virtual int verify(const QByteArray& templateData, const QByteArray& capturedData) = 0;

    /**
     * @brief Enroll biometric template
     * @param capturedData Captured biometric data
     * @return Template data
     */
    virtual QByteArray enroll(const QByteArray& capturedData) = 0;

    /**
     * @brief Get device configuration
     */
    virtual QVariantMap getConfiguration() const = 0;

    /**
     * @brief Set device configuration
     */
    virtual bool setConfiguration(const QVariantMap& config) = 0;

Q_SIGNALS:
    /**
     * @brief Emitted when device status changes
     */
    void statusChanged(DeviceStatus status);

    /**
     * @brief Emitted when device error occurs
     */
    void errorOccurred(const QString& error);

    /**
     * @brief Emitted when biometric data is captured
     */
    void biometricCaptured(const QByteArray& data);
};

/**
 * @brief Biometric Device Factory
 * 
 * Factory for creating biometric device instances.
 */
class BiometricDeviceFactory {
public:
    /**
     * @brief Create face recognition device
     * @param deviceId Device identifier
     * @return Device instance or nullptr if creation failed
     */
    static std::unique_ptr<IBiometricDevice> createFaceRecognitionDevice(const QString& deviceId);

    /**
     * @brief Create fingerprint scanner device
     * @param deviceId Device identifier
     * @return Device instance or nullptr if creation failed
     */
    static std::unique_ptr<IBiometricDevice> createFingerprintDevice(const QString& deviceId);

    /**
     * @brief Detect available devices
     * @return List of detected device IDs
     */
    static QList<QString> detectDevices(BiometricDeviceType type);
};

#endif // BIOMETRIC_ABSTRACTION_H


