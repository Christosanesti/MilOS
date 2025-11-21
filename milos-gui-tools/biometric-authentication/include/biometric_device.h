#ifndef BIOMETRIC_DEVICE_H
#define BIOMETRIC_DEVICE_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVariantMap>

/**
 * @brief Biometric Type
 */
enum class BiometricType {
    Fingerprint,
    Iris,
    Voice,
    Face
};

/**
 * @brief Biometric Device
 * 
 * Represents a biometric authentication device.
 */
class BiometricDevice : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString deviceId READ deviceId CONSTANT)
    Q_PROPERTY(QString deviceName READ deviceName CONSTANT)
    Q_PROPERTY(BiometricType biometricType READ biometricType CONSTANT)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionChanged)

public:
    explicit BiometricDevice(QObject* parent = nullptr);
    ~BiometricDevice();

    /**
     * @brief Initialize biometric device
     * @param deviceId Device identifier
     * @param deviceName Device name
     * @param biometricType Biometric type
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const QString& deviceId, const QString& deviceName, BiometricType biometricType);

    /**
     * @brief Get device ID
     * @return Device ID
     */
    QString deviceId() const { return m_deviceId; }

    /**
     * @brief Get device name
     * @return Device name
     */
    QString deviceName() const { return m_deviceName; }

    /**
     * @brief Get biometric type
     * @return Biometric type
     */
    BiometricType biometricType() const { return m_biometricType; }

    /**
     * @brief Check if device is connected
     * @return true if connected, false otherwise
     */
    bool isConnected() const { return m_connected; }

    /**
     * @brief Connect to device
     * @return true if connect successful, false otherwise
     */
    bool connectDevice();

    /**
     * @brief Disconnect from device
     */
    void disconnectDevice();

    /**
     * @brief Capture biometric sample
     * @return Biometric data or empty if failed
     */
    QByteArray captureSample();

    /**
     * @brief Verify biometric sample
     * @param sample Biometric sample
     * @param templateData Template data to match against
     * @return true if match, false otherwise
     */
    bool verifySample(const QByteArray& sample, const QByteArray& templateData);

Q_SIGNALS:
    /**
     * @brief Emitted when device connection changes
     */
    void connectionChanged(bool connected);

    /**
     * @brief Emitted when sample is captured
     */
    void sampleCaptured(const QByteArray& sample);

private:
    QString m_deviceId;
    QString m_deviceName;
    BiometricType m_biometricType;
    bool m_connected;
};

Q_DECLARE_METATYPE(BiometricType)

#endif // BIOMETRIC_DEVICE_H

