#ifndef SHIFT_SWAP_H
#define SHIFT_SWAP_H

#include "shift_scheduler.h"
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>

/**
 * @brief Swap Status
 */
enum class SwapStatus {
    Pending,
    Approved,
    Rejected,
    Cancelled
};

/**
 * @brief Shift Swap Request
 */
struct ShiftSwapRequest {
    QString swapId;
    QString shiftId;
    QString requesterId;
    QString targetPersonnelId;
    SwapStatus status;
    QString approverId;
    QDateTime requestDate;
    QDateTime approvalDate;
    QString reason;
};

/**
 * @brief Shift Swap Manager
 * 
 * Manages shift swap requests and approvals.
 */
class ShiftSwapManager : public QObject {
    Q_OBJECT

public:
    explicit ShiftSwapManager(QObject* parent = nullptr);
    ~ShiftSwapManager();

    /**
     * @brief Create swap request
     * @param shiftId Shift ID
     * @param requesterId Requester personnel ID
     * @param targetPersonnelId Target personnel ID
     * @param reason Reason for swap
     * @return Swap ID or empty string if failed
     */
    QString createSwapRequest(const QString& shiftId,
                             const QString& requesterId,
                             const QString& targetPersonnelId,
                             const QString& reason);

    /**
     * @brief Approve swap request
     * @param swapId Swap ID
     * @param approverId Approver ID
     * @return true if approval successful, false otherwise
     */
    bool approveSwapRequest(const QString& swapId, const QString& approverId);

    /**
     * @brief Reject swap request
     * @param swapId Swap ID
     * @param approverId Approver ID
     * @param reason Rejection reason
     * @return true if rejection successful, false otherwise
     */
    bool rejectSwapRequest(const QString& swapId, const QString& approverId, const QString& reason);

    /**
     * @brief Get swap requests
     * @param personnelId Optional personnel ID filter
     * @param status Optional status filter
     * @return List of swap requests
     */
    QList<ShiftSwapRequest> getSwapRequests(const QString& personnelId = QString(),
                                           SwapStatus status = SwapStatus::Pending) const;

    /**
     * @brief Get swap request by ID
     * @param swapId Swap ID
     * @return Swap request or empty request if not found
     */
    ShiftSwapRequest getSwapRequest(const QString& swapId) const;

Q_SIGNALS:
    /**
     * @brief Emitted when swap request is created
     */
    void swapRequestCreated(const QString& swapId);

    /**
     * @brief Emitted when swap request is approved
     */
    void swapRequestApproved(const QString& swapId);

    /**
     * @brief Emitted when swap request is rejected
     */
    void swapRequestRejected(const QString& swapId);

private:
    QList<ShiftSwapRequest> m_swapRequests;
    QString generateSwapId() const;
};

#endif // SHIFT_SWAP_H

