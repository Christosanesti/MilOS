#include "leave_manager.h"
#include <QUuid>
#include <QDebug>

LeaveManager::LeaveManager(QObject* parent)
    : QObject(parent)
{
}

LeaveManager::~LeaveManager() {
}

QString LeaveManager::createLeaveRequest(const QString& personnelId,
                                        const QDateTime& startDate,
                                        const QDateTime& endDate,
                                        LeaveType leaveType,
                                        const QString& reason) {
    if (personnelId.isEmpty() || !startDate.isValid() || !endDate.isValid()) {
        return QString();
    }
    
    LeaveRequest request;
    request.requestId = generateRequestId();
    request.personnelId = personnelId;
    request.startDate = startDate;
    request.endDate = endDate;
    request.leaveType = leaveType;
    request.reason = reason;
    request.status = LeaveStatus::Pending;
    request.requestDate = QDateTime::currentDateTime();
    
    m_leaveRequests.append(request);
    emit leaveRequestCreated(request.requestId);
    
    return request.requestId;
}

bool LeaveManager::approveLeaveRequest(const QString& requestId, const QString& approverId) {
    for (LeaveRequest& request : m_leaveRequests) {
        if (request.requestId == requestId) {
            if (request.status != LeaveStatus::Pending) {
                return false;
            }
            
            request.status = LeaveStatus::Approved;
            request.approverId = approverId;
            request.approvalDate = QDateTime::currentDateTime();
            
            emit leaveRequestApproved(requestId);
            return true;
        }
    }
    
    return false;
}

bool LeaveManager::rejectLeaveRequest(const QString& requestId, const QString& approverId, const QString& reason) {
    for (LeaveRequest& request : m_leaveRequests) {
        if (request.requestId == requestId) {
            if (request.status != LeaveStatus::Pending) {
                return false;
            }
            
            request.status = LeaveStatus::Rejected;
            request.approverId = approverId;
            request.approvalDate = QDateTime::currentDateTime();
            request.reason = reason;
            
            emit leaveRequestRejected(requestId);
            return true;
        }
    }
    
    return false;
}

QList<LeaveRequest> LeaveManager::getLeaveRequests(const QString& personnelId,
                                                   LeaveStatus status) const {
    QList<LeaveRequest> filtered;
    
    for (const LeaveRequest& request : m_leaveRequests) {
        // Filter by personnel ID
        if (!personnelId.isEmpty() && request.personnelId != personnelId) {
            continue;
        }
        
        // Filter by status (if status is Pending, show all; otherwise filter)
        if (status != LeaveStatus::Pending && request.status != status) {
            continue;
        }
        
        filtered.append(request);
    }
    
    return filtered;
}

LeaveRequest LeaveManager::getLeaveRequest(const QString& requestId) const {
    for (const LeaveRequest& request : m_leaveRequests) {
        if (request.requestId == requestId) {
            return request;
        }
    }
    
    LeaveRequest empty;
    return empty;
}

QString LeaveManager::generateRequestId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

