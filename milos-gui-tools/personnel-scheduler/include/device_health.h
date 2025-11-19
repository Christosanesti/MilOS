#ifndef DEVICE_HEALTH_H
#define DEVICE_HEALTH_H

#include "biometric_abstraction.h"
#include <QObject>
#include <QString>
#include <QTimer>
#include <QMap>
#include <QList>

/**
 * @brief Device Health Monitor
 * 
 * Monitors health status of biometric devices.
 */
class DeviceHealthMonitor : public QObject {
    Q_OBJECT

public:
    explicit DeviceHealthMonitor(QObject* parent = nullptr);
    ~DeviceHealthMonitor();

    /**
     * @brief Start monitoring device health
     * @param deviceId Device ID to monitor
     * @param device Device instance
     */
    void startMonitoring(const QString& deviceId, IBiometricDevice* device);

    /**
     * @brief Stop monitoring device health
     * @param deviceId Device ID
     */
    void stopMonitoring(const QString& deviceId);

    /**
     * @brief Get device health status
     * @param deviceId Device ID
     * @return Device health status
     */
    DeviceHealth getHealth(const QString& deviceId) const;

    /**
     * @brief Get all monitored devices
     * @return List of device IDs
     */
    QList<QString> getMonitoredDevices() const;

Q_SIGNALS:
    /**
     * @brief Emitted when device health status changes
     */
    void healthStatusChanged(const QString& deviceId, const DeviceHealth& health);

    /**
     * @brief Emitted when device error is detected
     */
    void deviceErrorDetected(const QString& deviceId, const QString& error);

private slots:
    /**
     * @brief Check device health
     */
    void checkDeviceHealth();

private:
    struct DeviceMonitorInfo {
        IBiometricDevice* device;
        DeviceHealth lastHealth;
        int errorCount;
    };

    QMap<QString, DeviceMonitorInfo> m_monitoredDevices;
    QTimer* m_healthCheckTimer;
    int m_healthCheckInterval;  // milliseconds
};

#endif // DEVICE_HEALTH_H

