#ifndef FINGERPRINT_DEVICE_H
#define FINGERPRINT_DEVICE_H

#include "biometric_abstraction.h"
#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVariantMap>

#ifdef HAVE_LIBFPRINT
#include <libfprint/fprint.h>
#endif

/**
 * @brief Fingerprint Scanner Device Implementation
 * 
 * Implements fingerprint scanner device interface using libfprint or vendor-specific APIs.
 */
class FingerprintDevice : public IBiometricDevice {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param deviceId Device identifier (libfprint device ID or USB device path)
     */
    explicit FingerprintDevice(const QString& deviceId, QObject* parent = nullptr);
    ~FingerprintDevice();

    QString deviceId() const override;
    BiometricDeviceType deviceType() const override;
    QString vendor() const override;
    QString model() const override;
    bool initialize() override;
    bool isConnected() const override;
    DeviceHealth getHealth() const override;
    QByteArray capture(int timeoutMs = 5000) override;
    int verify(const QByteArray& templateData, const QByteArray& capturedData) override;
    QByteArray enroll(const QByteArray& capturedData) override;
    QVariantMap getConfiguration() const override;
    bool setConfiguration(const QVariantMap& config) override;

private:
    QString m_deviceId;
    QString m_vendor;
    QString m_model;
    DeviceStatus m_status;
    QVariantMap m_configuration;
    
#ifdef HAVE_LIBFPRINT
    struct fp_dev* m_device;
    bool m_deviceInitialized;
#else
    void* m_device;  // Placeholder for device handle
    bool m_deviceInitialized;
#endif

    bool initializeLibFprint();
    void cleanupLibFprint();
    QByteArray processFingerprintData(const QByteArray& rawData);
};

#endif // FINGERPRINT_DEVICE_H


