#ifndef BIOMETRIC_CAPTURE_H
#define BIOMETRIC_CAPTURE_H

#include "biometric_abstraction.h"
#include "device_manager.h"
#include <QObject>
#include <QString>
#include <QByteArray>
#include <QTimer>

/**
 * @brief Biometric Capture
 * 
 * Handles biometric capture for attendance tracking.
 */
class BiometricCapture : public QObject {
    Q_OBJECT

public:
    explicit BiometricCapture(DeviceManager* deviceManager, QObject* parent = nullptr);
    ~BiometricCapture();

    /**
     * @brief Capture biometric data
     * @param deviceId Device ID
     * @param timeoutMs Timeout in milliseconds
     * @return Captured biometric data or empty byte array if failed
     */
    QByteArray capture(const QString& deviceId, int timeoutMs = 5000);

    /**
     * @brief Verify biometric data against template
     * @param deviceId Device ID
     * @param templateData Template data
     * @param capturedData Captured data
     * @return Match score (0-100)
     */
    int verify(const QString& deviceId, const QByteArray& templateData, const QByteArray& capturedData);

    /**
     * @brief Enroll biometric template
     * @param deviceId Device ID
     * @param capturedData Captured data
     * @return Template data or empty byte array if failed
     */
    QByteArray enroll(const QString& deviceId, const QByteArray& capturedData);

Q_SIGNALS:
    /**
     * @brief Emitted when biometric is captured
     */
    void biometricCaptured(const QByteArray& data);

    /**
     * @brief Emitted when capture fails
     */
    void captureFailed(const QString& error);

private:
    DeviceManager* m_deviceManager;
};

#endif // BIOMETRIC_CAPTURE_H

