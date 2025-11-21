#ifndef AIR_GAP_DETECTOR_H
#define AIR_GAP_DETECTOR_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QDateTime>

/**
 * @brief Air-Gap Status
 */
enum class AirGapStatus {
    Connected,      // System has network connectivity
    AirGapped,      // System is air-gapped (no network)
    Unknown         // Status cannot be determined
};

/**
 * @brief Air-Gap Detector
 * 
 * Automatically detects when system is air-gapped (no network connectivity).
 */
class AirGapDetector : public QObject {
    Q_OBJECT
    Q_PROPERTY(AirGapStatus status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool isAirGapped READ isAirGapped NOTIFY statusChanged)
    Q_PROPERTY(QDateTime lastCheckTime READ lastCheckTime NOTIFY statusChanged)

public:
    explicit AirGapDetector(QObject* parent = nullptr);
    ~AirGapDetector();

    /**
     * @brief Initialize air-gap detector
     * @param checkInterval Check interval in milliseconds
     * @return true if initialization successful, false otherwise
     */
    bool initialize(int checkInterval = 5000);

    /**
     * @brief Get current air-gap status
     * @return Air-gap status
     */
    AirGapStatus status() const { return m_status; }

    /**
     * @brief Check if system is air-gapped
     * @return true if air-gapped, false otherwise
     */
    bool isAirGapped() const { return m_status == AirGapStatus::AirGapped; }

    /**
     * @brief Get last check time
     * @return Last check time
     */
    QDateTime lastCheckTime() const { return m_lastCheckTime; }

    /**
     * @brief Force immediate check
     */
    void checkNow();

    /**
     * @brief Get network interfaces status
     * @return Map of interface names to status (up/down)
     */
    QVariantMap getNetworkInterfacesStatus() const;

Q_SIGNALS:
    /**
     * @brief Emitted when air-gap status changes
     */
    void statusChanged(AirGapStatus status);

    /**
     * @brief Emitted when system becomes air-gapped
     */
    void airGapped();

    /**
     * @brief Emitted when system regains connectivity
     */
    void connectivityRestored();

private slots:
    void performCheck();

private:
    AirGapStatus m_status;
    QTimer* m_checkTimer;
    QDateTime m_lastCheckTime;
    
    bool hasNetworkConnectivity() const;
    bool hasActiveNetworkInterface() const;
    bool canReachExternalHost() const;
};

Q_DECLARE_METATYPE(AirGapStatus)

#endif // AIR_GAP_DETECTOR_H

