#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include "biometric_abstraction.h"
#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <QTimer>
#include <memory>

/**
 * @brief Device Manager
 * 
 * Manages biometric hardware devices (face recognition and fingerprint scanners).
 */
class DeviceManager : public QObject {
    Q_OBJECT

public:
    explicit DeviceManager(QObject* parent = nullptr);
    ~DeviceManager();

    /**
     * @brief Initialize device manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Start device monitoring
     */
    void startMonitoring();

    /**
     * @brief Stop device monitoring
     */
    void stopMonitoring();

    /**
     * @brief Detect available devices
     * @param type Device type to detect
     * @return List of detected device IDs
     */
    QList<QString> detectDevices(BiometricDeviceType type);

    /**
     * @brief Register device
     * @param device Device instance
     * @return true if registration successful, false otherwise
     */
    bool registerDevice(std::unique_ptr<IBiometricDevice> device);

    /**
     * @brief Unregister device
     * @param deviceId Device ID
     */
    void unregisterDevice(const QString& deviceId);

    /**
     * @brief Get device by ID
     * @param deviceId Device ID
     * @return Device instance or nullptr if not found
     */
    IBiometricDevice* getDevice(const QString& deviceId) const;

    /**
     * @brief Get all registered devices
     * @return List of device IDs
     */
    QList<QString> getRegisteredDevices() const;

    /**
     * @brief Get devices by type
     * @param type Device type
     * @return List of device IDs
     */
    QList<QString> getDevicesByType(BiometricDeviceType type) const;

Q_SIGNALS:
    /**
     * @brief Emitted when device is connected
     */
    void deviceConnected(const QString& deviceId);

    /**
     * @brief Emitted when device is disconnected
     */
    void deviceDisconnected(const QString& deviceId);

    /**
     * @brief Emitted when device error occurs
     */
    void deviceError(const QString& deviceId, const QString& error);

private slots:
    /**
     * @brief Check for new devices
     */
    void checkForDevices();

    /**
     * @brief Handle device status change
     */
    void onDeviceStatusChanged(DeviceStatus status);

    /**
     * @brief Handle device error
     */
    void onDeviceError(const QString& error);

private:
    QMap<QString, std::unique_ptr<IBiometricDevice>> m_devices;
    bool m_monitoring;
    QTimer* m_monitorTimer;
};

#endif // DEVICE_MANAGER_H

