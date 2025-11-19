#ifndef STATUS_AGGREGATOR_H
#define STATUS_AGGREGATOR_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <QMap>
#include <QVariantMap>

/**
 * @brief Status Aggregator
 * 
 * Aggregates security status from all system services and tools.
 */
class StatusAggregator : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantMap overallStatus READ overallStatus NOTIFY overallStatusChanged)

public:
    explicit StatusAggregator(QObject* parent = nullptr);
    ~StatusAggregator();

    /**
     * @brief Initialize status aggregator
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get overall status
     */
    QVariantMap overallStatus() const { return m_overallStatus; }

    /**
     * @brief Refresh all status
     */
    Q_INVOKABLE void refreshAll();

    /**
     * @brief Get security posture status
     */
    Q_INVOKABLE QVariantMap getSecurityPosture() const;

    /**
     * @brief Get hardening status
     */
    Q_INVOKABLE QVariantMap getHardeningStatus() const;

    /**
     * @brief Get encryption status
     */
    Q_INVOKABLE QVariantMap getEncryptionStatus() const;

    /**
     * @brief Get network security status
     */
    Q_INVOKABLE QVariantMap getNetworkSecurityStatus() const;

    /**
     * @brief Get update status
     */
    Q_INVOKABLE QVariantMap getUpdateStatus() const;

Q_SIGNALS:
    /**
     * @brief Emitted when overall status changes
     */
    void overallStatusChanged();

    /**
     * @brief Emitted when status is refreshed
     */
    void statusRefreshed();

private slots:
    /**
     * @brief Periodic status refresh
     */
    void onRefreshTimer();

private:
    bool m_initialized;
    QTimer* m_refreshTimer;
    QVariantMap m_overallStatus;

    /**
     * @brief Refresh security posture
     */
    void refreshSecurityPosture();

    /**
     * @brief Refresh hardening status
     */
    void refreshHardeningStatus();

    /**
     * @brief Refresh encryption status
     */
    void refreshEncryptionStatus();

    /**
     * @brief Refresh network security status
     */
    void refreshNetworkSecurityStatus();

    /**
     * @brief Refresh update status
     */
    void refreshUpdateStatus();

    /**
     * @brief Update overall status
     */
    void updateOverallStatus();
};

#endif // STATUS_AGGREGATOR_H

