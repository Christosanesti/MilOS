#include "shift_swap.h"
#include <QUuid>
#include <QDebug>

ShiftSwapManager::ShiftSwapManager(QObject* parent)
    : QObject(parent)
{
}

ShiftSwapManager::~ShiftSwapManager() {
}

QString ShiftSwapManager::createSwapRequest(const QString& shiftId,
                                            const QString& requesterId,
                                            const QString& targetPersonnelId,
                                            const QString& reason) {
    if (shiftId.isEmpty() || requesterId.isEmpty() || targetPersonnelId.isEmpty()) {
        return QString();
    }
    
    ShiftSwapRequest swap;
    swap.swapId = generateSwapId();
    swap.shiftId = shiftId;
    swap.requesterId = requesterId;
    swap.targetPersonnelId = targetPersonnelId;
    swap.status = SwapStatus::Pending;
    swap.requestDate = QDateTime::currentDateTime();
    swap.reason = reason;
    
    m_swapRequests.append(swap);
    emit swapRequestCreated(swap.swapId);
    
    return swap.swapId;
}

bool ShiftSwapManager::approveSwapRequest(const QString& swapId, const QString& approverId) {
    for (ShiftSwapRequest& swap : m_swapRequests) {
        if (swap.swapId == swapId) {
            if (swap.status != SwapStatus::Pending) {
                return false;
            }
            
            swap.status = SwapStatus::Approved;
            swap.approverId = approverId;
            swap.approvalDate = QDateTime::currentDateTime();
            
            emit swapRequestApproved(swapId);
            return true;
        }
    }
    
    return false;
}

bool ShiftSwapManager::rejectSwapRequest(const QString& swapId, const QString& approverId, const QString& reason) {
    for (ShiftSwapRequest& swap : m_swapRequests) {
        if (swap.swapId == swapId) {
            if (swap.status != SwapStatus::Pending) {
                return false;
            }
            
            swap.status = SwapStatus::Rejected;
            swap.approverId = approverId;
            swap.approvalDate = QDateTime::currentDateTime();
            swap.reason = reason;
            
            emit swapRequestRejected(swapId);
            return true;
        }
    }
    
    return false;
}

QList<ShiftSwapRequest> ShiftSwapManager::getSwapRequests(const QString& personnelId,
                                                          SwapStatus status) const {
    QList<ShiftSwapRequest> filtered;
    
    for (const ShiftSwapRequest& swap : m_swapRequests) {
        // Filter by personnel ID (requester or target)
        if (!personnelId.isEmpty() && 
            swap.requesterId != personnelId && 
            swap.targetPersonnelId != personnelId) {
            continue;
        }
        
        // Filter by status
        if (status != SwapStatus::Pending && swap.status != status) {
            continue;
        }
        
        filtered.append(swap);
    }
    
    return filtered;
}

ShiftSwapRequest ShiftSwapManager::getSwapRequest(const QString& swapId) const {
    for (const ShiftSwapRequest& swap : m_swapRequests) {
        if (swap.swapId == swapId) {
            return swap;
        }
    }
    
    ShiftSwapRequest empty;
    return empty;
}

QString ShiftSwapManager::generateSwapId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

