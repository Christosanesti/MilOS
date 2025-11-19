#ifndef CONFLICT_DETECTOR_H
#define CONFLICT_DETECTOR_H

#include "shift_scheduler.h"
#include <QObject>
#include <QString>
#include <QList>

/**
 * @brief Conflict Type
 */
enum class ConflictType {
    Overlap,
    DoubleBooking,
    InsufficientRest,
    CoverageGap
};

/**
 * @brief Conflict
 */
struct Conflict {
    ConflictType type;
    QString shiftId1;
    QString shiftId2;
    QString personnelId;
    QString description;
    QDateTime conflictTime;
};

/**
 * @brief Conflict Detector
 * 
 * Detects scheduling conflicts and overlaps.
 */
class ConflictDetector : public QObject {
    Q_OBJECT

public:
    explicit ConflictDetector(QObject* parent = nullptr);
    ~ConflictDetector();

    /**
     * @brief Detect conflicts in schedule
     * @param shifts List of shift assignments
     * @return List of detected conflicts
     */
    QList<Conflict> detectConflicts(const QList<ShiftAssignment>& shifts) const;

    /**
     * @brief Check if shift assignment conflicts
     * @param shift Shift assignment to check
     * @param existingShifts Existing shift assignments
     * @return List of conflicts
     */
    QList<Conflict> checkShiftConflicts(const ShiftAssignment& shift,
                                        const QList<ShiftAssignment>& existingShifts) const;

    /**
     * @brief Resolve conflict
     * @param conflict Conflict to resolve
     * @param resolution Resolution action
     * @return true if resolution successful, false otherwise
     */
    bool resolveConflict(const Conflict& conflict, const QString& resolution);

Q_SIGNALS:
    /**
     * @brief Emitted when conflict is detected
     */
    void conflictDetected(const Conflict& conflict);

    /**
     * @brief Emitted when conflict is resolved
     */
    void conflictResolved(const QString& conflictId);

private:
    bool checkOverlap(const ShiftAssignment& shift1, const ShiftAssignment& shift2) const;
    bool checkInsufficientRest(const ShiftAssignment& shift1, const ShiftAssignment& shift2) const;
    int getMinimumRestHours() const;
};

#endif // CONFLICT_DETECTOR_H

