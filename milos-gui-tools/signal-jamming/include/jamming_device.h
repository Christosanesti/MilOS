#ifndef JAMMING_DEVICE_H
#define JAMMING_DEVICE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

/**
 * @brief Jamming Device Type
 */
enum class JammingDeviceType {
    RFJammer,
    WiFiJammer,
    CellularJammer,
    GPSJammer,
    Generic
};

/**
 * @brief Jamming Pattern
 */
struct JammingPattern {
    QString id;
    QString name;
    double frequencyStart;  // MHz
    double frequencyEnd;    // MHz
    double power;           // dBm
    QString patternType;    // "continuous", "pulse", "sweep"
    QVariantMap parameters;
};

/**
 * @brief Jamming Device
 * 
 * Represents a jamming device hardware interface.
 */
class JammingDevice : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString deviceId READ deviceId CONSTANT)
    Q_PROPERTY(QString deviceName READ deviceName CONSTANT)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionChanged)
    Q_PROPERTY(bool isJamming READ isJamming NOTIFY jammingStateChanged)

public:
    explicit JammingDevice(QObject* parent = nullptr);
    ~JammingDevice();

    /**
     * @brief Initialize jamming device
     * @param deviceId Device identifier
     * @param deviceType Device type
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const QString& deviceId, JammingDeviceType deviceType);

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
     * @brief Check if device is connected
     * @return true if connected, false otherwise
     */
    bool isConnected() const { return m_connected; }

    /**
     * @brief Check if device is jamming
     * @return true if jamming, false otherwise
     */
    bool isJamming() const { return m_jamming; }

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
     * @brief Start jamming with pattern
     * @param pattern Jamming pattern
     * @return true if start successful, false otherwise
     */
    bool startJamming(const JammingPattern& pattern);

    /**
     * @brief Stop jamming
     * @return true if stop successful, false otherwise
     */
    bool stopJamming();

    /**
     * @brief Get supported frequencies
     * @return List of supported frequency ranges
     */
    QList<QPair<double, double>> getSupportedFrequencies() const;

    /**
     * @brief Get device status
     * @return Device status information
     */
    QVariantMap getDeviceStatus() const;

Q_SIGNALS:
    /**
     * @brief Emitted when device connection changes
     */
    void connectionChanged(bool connected);

    /**
     * @brief Emitted when jamming state changes
     */
    void jammingStateChanged(bool jamming);

    /**
     * @brief Emitted when jamming pattern changes
     */
    void patternChanged(const JammingPattern& pattern);

private:
    QString m_deviceId;
    QString m_deviceName;
    JammingDeviceType m_deviceType;
    bool m_connected;
    bool m_jamming;
    JammingPattern m_currentPattern;
    
    bool sendCommand(const QString& command, const QVariantMap& parameters = QVariantMap());
};

Q_DECLARE_METATYPE(JammingDeviceType)
Q_DECLARE_METATYPE(JammingPattern)

#endif // JAMMING_DEVICE_H

