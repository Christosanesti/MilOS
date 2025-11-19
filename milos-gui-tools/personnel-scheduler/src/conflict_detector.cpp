#include "conflict_detector.h"
#include <QDebug>
#include <algorithm>

ConflictDetector::ConflictDetector(QObject* parent)
    : QObject(parent)
{
}

ConflictDetector::~ConflictDetector() {
}

QList<Conflict> ConflictDetector::detectConflicts(const QList<ShiftAssignment>& shifts) const {
    QList<Conflict> conflicts;
    
    for (int i = 0; i < shifts.size(); i++) {
        for (int j = i + 1; j < shifts.size(); j++) {
            const ShiftAssignment& shift1 = shifts[i];
            const ShiftAssignment& shift2 = shifts[j];
            
            // Check for overlaps (same personnel)
            if (shift1.personnelId == shift2.personnelId) {
                if (checkOverlap(shift1, shift2)) {
                    Conflict conflict;
                    conflict.type = ConflictType::Overlap;
                    conflict.shiftId1 = shift1.shiftId;
                    conflict.shiftId2 = shift2.shiftId;
                    conflict.personnelId = shift1.personnelId;
                    conflict.description = "Shift overlap detected";
                    conflict.conflictTime = shift1.startDateTime;
                    conflicts.append(conflict);
                }
                
                // Check for insufficient rest
                if (checkInsufficientRest(shift1, shift2)) {
                    Conflict conflict;
                    conflict.type = ConflictType::InsufficientRest;
                    conflict.shiftId1 = shift1.shiftId;
                    conflict.shiftId2 = shift2.shiftId;
                    conflict.personnelId = shift1.personnelId;
                    conflict.description = "Insufficient rest period between shifts";
                    conflict.conflictTime = shift1.endDateTime;
                    conflicts.append(conflict);
                }
            }
        }
    }
    
    return conflicts;
}

QList<Conflict> ConflictDetector::checkShiftConflicts(const ShiftAssignment& shift,
                                                      const QList<ShiftAssignment>& existingShifts) const {
    QList<Conflict> conflicts;
    
    for (const ShiftAssignment& existingShift : existingShifts) {
        // Check for overlaps (same personnel)
        if (shift.personnelId == existingShift.personnelId) {
            if (checkOverlap(shift, existingShift)) {
                Conflict conflict;
                conflict.type = ConflictType::Overlap;
                conflict.shiftId1 = shift.shiftId;
                conflict.shiftId2 = existingShift.shiftId;
                conflict.personnelId = shift.personnelId;
                conflict.description = "Shift overlap detected";
                conflict.conflictTime = shift.startDateTime;
                conflicts.append(conflict);
            }
            
            // Check for insufficient rest
            if (checkInsufficientRest(shift, existingShift)) {
                Conflict conflict;
                conflict.type = ConflictType::InsufficientRest;
                conflict.shiftId1 = shift.shiftId;
                conflict.shiftId2 = existingShift.shiftId;
                conflict.personnelId = shift.personnelId;
                conflict.description = "Insufficient rest period between shifts";
                conflict.conflictTime = shift.startDateTime;
                conflicts.append(conflict);
            }
        }
    }
    
    return conflicts;
}

bool ConflictDetector::resolveConflict(const Conflict& conflict, const QString& resolution) {
    // Emit signal for conflict resolution
    emit conflictResolved(conflict.shiftId1);
    return true;
}

bool ConflictDetector::checkOverlap(const ShiftAssignment& shift1, const ShiftAssignment& shift2) const {
    // Check if shifts overlap in time
    return (shift1.startDateTime < shift2.endDateTime && shift1.endDateTime > shift2.startDateTime);
}

bool ConflictDetector::checkInsufficientRest(const ShiftAssignment& shift1, const ShiftAssignment& shift2) const {
    // Determine which shift is earlier
    const ShiftAssignment* earlier = &shift1;
    const ShiftAssignment* later = &shift2;
    
    if (shift2.startDateTime < shift1.startDateTime) {
        earlier = &shift2;
        later = &shift1;
    }
    
    // Calculate rest period
    qint64 restHours = earlier->endDateTime.secsTo(later->startDateTime) / 3600;
    
    return restHours < getMinimumRestHours();
}

int ConflictDetector::getMinimumRestHours() const {
    return 8;  // Minimum 8 hours rest between shifts
}

