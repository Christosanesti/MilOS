#include "coverage_manager.h"
#include <QUuid>
#include <QDebug>

CoverageManager::CoverageManager(QObject* parent)
    : QObject(parent)
{
}

CoverageManager::~CoverageManager() {
}

QString CoverageManager::createCoverageRequest(const QString& shiftId,
                                              const QString& requesterId,
                                              const QString& reason) {
    if (shiftId.isEmpty() || requesterId.isEmpty()) {
        return QString();
    }
    
    CoverageRequest request;
    request.requestId = generateRequestId();
    request.shiftId = shiftId;
    request.requesterId = requesterId;
    request.requestDate = QDateTime::currentDateTime();
    request.reason = reason;
    request.fulfilled = false;
    
    m_coverageRequests.append(request);
    emit coverageRequestCreated(request.requestId);
    
    return request.requestId;
}

bool CoverageManager::fulfillCoverageRequest(const QString& requestId, const QString& fulfilledBy) {
    for (CoverageRequest& request : m_coverageRequests) {
        if (request.requestId == requestId) {
            if (request.fulfilled) {
                return false;
            }
            
            request.fulfilled = true;
            request.fulfilledBy = fulfilledBy;
            
            emit coverageRequestFulfilled(requestId);
            return true;
        }
    }
    
    return false;
}

QList<CoverageRequest> CoverageManager::getCoverageRequests(const QString& shiftId,
                                                            bool fulfilled) const {
    QList<CoverageRequest> filtered;
    
    for (const CoverageRequest& request : m_coverageRequests) {
        // Filter by shift ID
        if (!shiftId.isEmpty() && request.shiftId != shiftId) {
            continue;
        }
        
        // Filter by fulfilled status
        if (request.fulfilled != fulfilled) {
            continue;
        }
        
        filtered.append(request);
    }
    
    return filtered;
}

QVariantMap CoverageManager::checkCoverage(const QDateTime& startDateTime,
                                           const QDateTime& endDateTime,
                                           const QString& location) const {
    QVariantMap coverage;
    
    // This would analyze shifts in the time period
    // For now, return placeholder data
    coverage["start_date_time"] = startDateTime.toString(Qt::ISODate);
    coverage["end_date_time"] = endDateTime.toString(Qt::ISODate);
    coverage["location"] = location;
    coverage["covered"] = true;
    coverage["coverage_percentage"] = 100;
    
    return coverage;
}

QList<QVariantMap> CoverageManager::getCoverageGaps(const QDateTime& startDateTime,
                                                    const QDateTime& endDateTime,
                                                    const QString& location) const {
    QList<QVariantMap> gaps;
    
    // This would analyze shifts and identify gaps
    // For now, return empty list
    return gaps;
}

QString CoverageManager::generateRequestId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

