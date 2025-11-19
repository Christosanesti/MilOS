#ifndef COVERAGE_MANAGER_H
#define COVERAGE_MANAGER_H

#include "shift_scheduler.h"
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>
#include <QMap>

/**
 * @brief Coverage Request
 */
struct CoverageRequest {
    QString requestId;
    QString shiftId;
    QString requesterId;
    QDateTime requestDate;
    QString reason;
    bool fulfilled;
    QString fulfilledBy;
};

/**
 * @brief Coverage Manager
 * 
 * Manages coverage requests and tracking.
 */
class CoverageManager : public QObject {
    Q_OBJECT

public:
    explicit CoverageManager(QObject* parent = nullptr);
    ~CoverageManager();

    /**
     * @brief Create coverage request
     * @param shiftId Shift ID
     * @param requesterId Requester personnel ID
     * @param reason Reason for coverage request
     * @return Request ID or empty string if failed
     */
    QString createCoverageRequest(const QString& shiftId,
                                 const QString& requesterId,
                                 const QString& reason);

    /**
     * @brief Fulfill coverage request
     * @param requestId Request ID
     * @param fulfilledBy Personnel ID who will cover
     * @return true if fulfillment successful, false otherwise
     */
    bool fulfillCoverageRequest(const QString& requestId, const QString& fulfilledBy);

    /**
     * @brief Get coverage requests
     * @param shiftId Optional shift ID filter
     * @param fulfilled Optional fulfilled filter
     * @return List of coverage requests
     */
    QList<CoverageRequest> getCoverageRequests(const QString& shiftId = QString(),
                                              bool fulfilled = false) const;

    /**
     * @brief Check coverage for time period
     * @param startDateTime Start date/time
     * @param endDateTime End date/time
     * @param location Optional location filter
     * @return Coverage status map
     */
    QVariantMap checkCoverage(const QDateTime& startDateTime,
                              const QDateTime& endDateTime,
                              const QString& location = QString()) const;

    /**
     * @brief Get coverage gaps
     * @param startDateTime Start date/time
     * @param endDateTime End date/time
     * @param location Optional location filter
     * @return List of coverage gaps
     */
    QList<QVariantMap> getCoverageGaps(const QDateTime& startDateTime,
                                      const QDateTime& endDateTime,
                                      const QString& location = QString()) const;

Q_SIGNALS:
    /**
     * @brief Emitted when coverage request is created
     */
    void coverageRequestCreated(const QString& requestId);

    /**
     * @brief Emitted when coverage request is fulfilled
     */
    void coverageRequestFulfilled(const QString& requestId);

    /**
     * @brief Emitted when coverage gap is detected
     */
    void coverageGapDetected(const QVariantMap& gap);

private:
    QList<CoverageRequest> m_coverageRequests;
    QString generateRequestId() const;
};

#endif // COVERAGE_MANAGER_H

