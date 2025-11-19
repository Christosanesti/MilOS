#ifndef ACCESS_RESTRICTIONS_H
#define ACCESS_RESTRICTIONS_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QTime>
#include <QList>
#include <QMap>

/**
 * @brief Time Restriction
 */
struct TimeRestriction {
    QString restrictionId;
    QString personnelId;
    QString location;
    QTime startTime;
    QTime endTime;
    QList<int> daysOfWeek;  // 0=Sunday, 6=Saturday
    bool enabled;
};

/**
 * @brief Location Restriction
 */
struct LocationRestriction {
    QString restrictionId;
    QString personnelId;
    QString location;
    bool allowed;
    bool enabled;
};

/**
 * @brief Access Restrictions Manager
 * 
 * Manages time-based and location-based access restrictions.
 */
class AccessRestrictionsManager : public QObject {
    Q_OBJECT

public:
    explicit AccessRestrictionsManager(QObject* parent = nullptr);
    ~AccessRestrictionsManager();

    /**
     * @brief Initialize restrictions manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Create time restriction
     * @param personnelId Personnel ID
     * @param location Location/area
     * @param startTime Start time
     * @param endTime End time
     * @param daysOfWeek Days of week (0=Sunday, 6=Saturday)
     * @return Restriction ID or empty string if failed
     */
    QString createTimeRestriction(const QString& personnelId,
                                 const QString& location,
                                 const QTime& startTime,
                                 const QTime& endTime,
                                 const QList<int>& daysOfWeek);

    /**
     * @brief Check time restriction
     * @param personnelId Personnel ID
     * @param location Location/area
     * @param requestTime Request time
     * @return true if access allowed, false if restricted
     */
    bool checkTimeRestriction(const QString& personnelId,
                             const QString& location,
                             const QDateTime& requestTime) const;

    /**
     * @brief Create location restriction
     * @param personnelId Personnel ID
     * @param location Location/area
     * @param allowed Whether access is allowed
     * @return Restriction ID or empty string if failed
     */
    QString createLocationRestriction(const QString& personnelId,
                                     const QString& location,
                                     bool allowed);

    /**
     * @brief Check location restriction
     * @param personnelId Personnel ID
     * @param location Location/area
     * @return true if access allowed, false if restricted
     */
    bool checkLocationRestriction(const QString& personnelId, const QString& location) const;

    /**
     * @brief Get time restrictions
     * @param personnelId Optional personnel ID filter
     * @param location Optional location filter
     * @return List of time restrictions
     */
    QList<TimeRestriction> getTimeRestrictions(const QString& personnelId = QString(),
                                              const QString& location = QString()) const;

    /**
     * @brief Get location restrictions
     * @param personnelId Optional personnel ID filter
     * @param location Optional location filter
     * @return List of location restrictions
     */
    QList<LocationRestriction> getLocationRestrictions(const QString& personnelId = QString(),
                                                      const QString& location = QString()) const;

Q_SIGNALS:
    /**
     * @brief Emitted when time restriction is created
     */
    void timeRestrictionCreated(const QString& restrictionId);

    /**
     * @brief Emitted when location restriction is created
     */
    void locationRestrictionCreated(const QString& restrictionId);

private:
    QList<TimeRestriction> m_timeRestrictions;
    QList<LocationRestriction> m_locationRestrictions;
    QString generateRestrictionId() const;
};

#endif // ACCESS_RESTRICTIONS_H

