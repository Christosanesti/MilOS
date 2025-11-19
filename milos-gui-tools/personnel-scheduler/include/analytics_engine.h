#ifndef ANALYTICS_ENGINE_H
#define ANALYTICS_ENGINE_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QVariantMap>
#include <QList>

/**
 * @brief Analytics Engine
 * 
 * Provides analytics and trend analysis for attendance and access control.
 */
class AnalyticsEngine : public QObject {
    Q_OBJECT

public:
    explicit AnalyticsEngine(QObject* parent = nullptr);
    ~AnalyticsEngine();

    /**
     * @brief Initialize analytics engine
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Analyze attendance trends
     * @param startDate Start date
     * @param endDate End date
     * @param personnelId Optional personnel ID filter
     * @return Trend analysis data
     */
    QVariantMap analyzeAttendanceTrends(const QDateTime& startDate,
                                       const QDateTime& endDate,
                                       const QString& personnelId = QString());

    /**
     * @brief Get attendance statistics
     * @param startDate Start date
     * @param endDate End date
     * @param personnelId Optional personnel ID filter
     * @return Statistics map
     */
    QVariantMap getAttendanceStatistics(const QDateTime& startDate,
                                      const QDateTime& endDate,
                                      const QString& personnelId = QString());

    /**
     * @brief Get access control statistics
     * @param startDate Start date
     * @param endDate End date
     * @param location Optional location filter
     * @return Statistics map
     */
    QVariantMap getAccessControlStatistics(const QDateTime& startDate,
                                         const QDateTime& endDate,
                                         const QString& location = QString());

    /**
     * @brief Get dashboard data
     * @return Dashboard data map
     */
    QVariantMap getDashboardData() const;

Q_SIGNALS:
    /**
     * @brief Emitted when analytics data is updated
     */
    void analyticsUpdated();

private:
    QVariantMap calculateTrends(const QList<QVariantMap>& data) const;
};

#endif // ANALYTICS_ENGINE_H

