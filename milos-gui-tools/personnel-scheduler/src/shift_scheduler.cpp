#include "shift_scheduler.h"
#include <QUuid>
#include <QDebug>
#include <algorithm>

ShiftScheduler::ShiftScheduler(QObject* parent)
    : QObject(parent)
{
}

ShiftScheduler::~ShiftScheduler() {
}

bool ShiftScheduler::initialize() {
    return true;
}

QString ShiftScheduler::createShift(const QString& personnelId,
                                    const QDateTime& startDateTime,
                                    const QDateTime& endDateTime,
                                    ShiftType shiftType,
                                    const QString& location) {
    if (personnelId.isEmpty() || !startDateTime.isValid() || !endDateTime.isValid()) {
        return QString();
    }
    
    if (startDateTime >= endDateTime) {
        qWarning() << "Invalid shift time range";
        return QString();
    }
    
    ShiftAssignment shift;
    shift.shiftId = generateShiftId();
    shift.personnelId = personnelId;
    shift.startDateTime = startDateTime;
    shift.endDateTime = endDateTime;
    shift.shiftType = shiftType;
    shift.location = location;
    shift.status = ShiftStatus::Scheduled;
    
    m_shifts.append(shift);
    emit shiftCreated(shift.shiftId);
    
    return shift.shiftId;
}

bool ShiftScheduler::updateShift(const QString& shiftId, const QVariantMap& updates) {
    for (ShiftAssignment& shift : m_shifts) {
        if (shift.shiftId == shiftId) {
            if (updates.contains("personnel_id")) {
                shift.personnelId = updates["personnel_id"].toString();
            }
            if (updates.contains("start_date_time")) {
                shift.startDateTime = updates["start_date_time"].toDateTime();
            }
            if (updates.contains("end_date_time")) {
                shift.endDateTime = updates["end_date_time"].toDateTime();
            }
            if (updates.contains("shift_type")) {
                shift.shiftType = static_cast<ShiftType>(updates["shift_type"].toInt());
            }
            if (updates.contains("location")) {
                shift.location = updates["location"].toString();
            }
            if (updates.contains("status")) {
                shift.status = static_cast<ShiftStatus>(updates["status"].toInt());
            }
            if (updates.contains("notes")) {
                shift.notes = updates["notes"].toString();
            }
            
            emit shiftUpdated(shiftId);
            return true;
        }
    }
    
    return false;
}

bool ShiftScheduler::deleteShift(const QString& shiftId) {
    for (int i = 0; i < m_shifts.size(); i++) {
        if (m_shifts[i].shiftId == shiftId) {
            m_shifts.removeAt(i);
            emit shiftDeleted(shiftId);
            return true;
        }
    }
    
    return false;
}

QList<ShiftAssignment> ShiftScheduler::getShifts(const QString& personnelId,
                                                  const QDateTime& startDate,
                                                  const QDateTime& endDate) const {
    QList<ShiftAssignment> filtered;
    
    for (const ShiftAssignment& shift : m_shifts) {
        // Filter by personnel ID
        if (!personnelId.isEmpty() && shift.personnelId != personnelId) {
            continue;
        }
        
        // Filter by date range
        if (startDate.isValid() && shift.startDateTime < startDate) {
            continue;
        }
        if (endDate.isValid() && shift.endDateTime > endDate) {
            continue;
        }
        
        filtered.append(shift);
    }
    
    return filtered;
}

ShiftAssignment ShiftScheduler::getShift(const QString& shiftId) const {
    for (const ShiftAssignment& shift : m_shifts) {
        if (shift.shiftId == shiftId) {
            return shift;
        }
    }
    
    ShiftAssignment empty;
    return empty;
}

QString ShiftScheduler::createTemplate(const ShiftTemplate& template) {
    ShiftTemplate newTemplate = template;
    newTemplate.templateId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    m_templates.append(newTemplate);
    return newTemplate.templateId;
}

QList<QString> ShiftScheduler::generateShiftsFromTemplate(const QString& templateId,
                                                          const QDateTime& startDate,
                                                          const QDateTime& endDate) {
    QList<QString> generatedShiftIds;
    
    // Find template
    ShiftTemplate* template = nullptr;
    for (ShiftTemplate& t : m_templates) {
        if (t.templateId == templateId) {
            template = &t;
            break;
        }
    }
    
    if (!template) {
        return generatedShiftIds;
    }
    
    // Generate shifts for each day in range
    QDateTime currentDate = startDate;
    while (currentDate <= endDate) {
        int dayOfWeek = currentDate.date().dayOfWeek() - 1;  // Qt: 1=Monday, 7=Sunday
        
        // Check if template applies to this day
        if (template->daysOfWeek.contains(dayOfWeek)) {
            QDateTime shiftStart = QDateTime(currentDate.date(), template->startTime);
            QDateTime shiftEnd = QDateTime(currentDate.date(), template->endTime);
            
            // Handle overnight shifts
            if (shiftEnd < shiftStart) {
                shiftEnd = shiftEnd.addDays(1);
            }
            
            // Create shift (personnel assignment would be done separately)
            QString shiftId = createShift(QString(), shiftStart, shiftEnd, template->shiftType, template->location);
            if (!shiftId.isEmpty()) {
                generatedShiftIds.append(shiftId);
            }
        }
        
        currentDate = currentDate.addDays(1);
    }
    
    return generatedShiftIds;
}

bool ShiftScheduler::applyRotation(const QList<QString>& personnelIds,
                                   const QDateTime& startDate,
                                   const QDateTime& endDate,
                                   const QVariantMap& rotationPattern) {
    if (personnelIds.isEmpty()) {
        return false;
    }
    
    // Get rotation parameters
    int shiftDurationHours = rotationPattern.value("shift_duration_hours", 8).toInt();
    int rotationCycleDays = rotationPattern.value("rotation_cycle_days", 7).toInt();
    QString location = rotationPattern.value("location", "").toString();
    ShiftType shiftType = static_cast<ShiftType>(rotationPattern.value("shift_type", 0).toInt());
    
    // Apply rotation pattern
    QDateTime currentDate = startDate;
    int personnelIndex = 0;
    
    while (currentDate <= endDate) {
        QString personnelId = personnelIds[personnelIndex % personnelIds.size()];
        
        QDateTime shiftStart = currentDate;
        QDateTime shiftEnd = shiftStart.addSecs(shiftDurationHours * 3600);
        
        createShift(personnelId, shiftStart, shiftEnd, shiftType, location);
        
        // Advance to next shift
        currentDate = currentDate.addDays(rotationCycleDays);
        personnelIndex++;
    }
    
    return true;
}

QString ShiftScheduler::generateShiftId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

