#ifndef SHIFT_SCHEDULER_H
#define SHIFT_SCHEDULER_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>
#include <QVariantMap>
#include <QTime>

/**
 * @brief Shift Type
 */
enum class ShiftType {
    Day,
    Night,
    Swing,
    Custom
};

/**
 * @brief Shift Status
 */
enum class ShiftStatus {
    Scheduled,
    Confirmed,
    Cancelled,
    Completed
};

/**
 * @brief Shift Assignment
 */
struct ShiftAssignment {
    QString shiftId;
    QString personnelId;
    QDateTime startDateTime;
    QDateTime endDateTime;
    ShiftType shiftType;
    QString location;
    ShiftStatus status;
    QString notes;
};

/**
 * @brief Shift Template
 */
struct ShiftTemplate {
    QString templateId;
    QString name;
    ShiftType shiftType;
    QTime startTime;
    QTime endTime;
    int durationHours;
    QString location;
    QList<int> daysOfWeek;  // 0=Sunday, 6=Saturday
    bool isRecurring;
};

/**
 * @brief Shift Scheduler
 * 
 * Manages shift scheduling and assignments.
 */
class ShiftScheduler : public QObject {
    Q_OBJECT

public:
    explicit ShiftScheduler(QObject* parent = nullptr);
    ~ShiftScheduler();

    /**
     * @brief Initialize scheduler
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Create shift assignment
     * @param personnelId Personnel ID
     * @param startDateTime Start date/time
     * @param endDateTime End date/time
     * @param shiftType Shift type
     * @param location Location/area
     * @return Shift ID or empty string if failed
     */
    QString createShift(const QString& personnelId,
                       const QDateTime& startDateTime,
                       const QDateTime& endDateTime,
                       ShiftType shiftType,
                       const QString& location);

    /**
     * @brief Update shift assignment
     * @param shiftId Shift ID
     * @param updates Update map
     * @return true if update successful, false otherwise
     */
    bool updateShift(const QString& shiftId, const QVariantMap& updates);

    /**
     * @brief Delete shift assignment
     * @param shiftId Shift ID
     * @return true if deletion successful, false otherwise
     */
    bool deleteShift(const QString& shiftId);

    /**
     * @brief Get shift assignments
     * @param personnelId Optional personnel ID filter
     * @param startDate Optional start date filter
     * @param endDate Optional end date filter
     * @return List of shift assignments
     */
    QList<ShiftAssignment> getShifts(const QString& personnelId = QString(),
                                     const QDateTime& startDate = QDateTime(),
                                     const QDateTime& endDate = QDateTime()) const;

    /**
     * @brief Get shift by ID
     * @param shiftId Shift ID
     * @return Shift assignment or empty assignment if not found
     */
    ShiftAssignment getShift(const QString& shiftId) const;

    /**
     * @brief Create shift template
     * @param template Template definition
     * @return Template ID or empty string if failed
     */
    QString createTemplate(const ShiftTemplate& template);

    /**
     * @brief Generate shifts from template
     * @param templateId Template ID
     * @param startDate Start date
     * @param endDate End date
     * @return List of generated shift IDs
     */
    QList<QString> generateShiftsFromTemplate(const QString& templateId,
                                              const QDateTime& startDate,
                                              const QDateTime& endDate);

    /**
     * @brief Apply automated rotation
     * @param personnelIds List of personnel IDs
     * @param startDate Start date
     * @param endDate End date
     * @param rotationPattern Rotation pattern configuration
     * @return true if rotation successful, false otherwise
     */
    bool applyRotation(const QList<QString>& personnelIds,
                      const QDateTime& startDate,
                      const QDateTime& endDate,
                      const QVariantMap& rotationPattern);

Q_SIGNALS:
    /**
     * @brief Emitted when shift is created
     */
    void shiftCreated(const QString& shiftId);

    /**
     * @brief Emitted when shift is updated
     */
    void shiftUpdated(const QString& shiftId);

    /**
     * @brief Emitted when shift is deleted
     */
    void shiftDeleted(const QString& shiftId);

private:
    QList<ShiftAssignment> m_shifts;
    QList<ShiftTemplate> m_templates;
    QString generateShiftId() const;
};

#endif // SHIFT_SCHEDULER_H

