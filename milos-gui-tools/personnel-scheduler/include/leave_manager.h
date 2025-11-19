#ifndef LEAVE_MANAGER_H
#define LEAVE_MANAGER_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>

/**
 * @brief Leave Type
 */
enum class LeaveType {
    Vacation,
    Sick,
    Personal,
    Emergency,
    Other
};

/**
 * @brief Leave Status
 */
enum class LeaveStatus {
    Pending,
    Approved,
    Rejected,
    Cancelled
};

/**
 * @brief Leave Request
 */
struct LeaveRequest {
    QString requestId;
    QString personnelId;
    QDateTime startDate;
    QDateTime endDate;
    LeaveType leaveType;
    QString reason;
    LeaveStatus status;
    QString approverId;
    QDateTime requestDate;
    QDateTime approvalDate;
};

/**
 * @brief Leave Manager
 * 
 * Manages leave requests and approvals.
 */
class LeaveManager : public QObject {
    Q_OBJECT

public:
    explicit LeaveManager(QObject* parent = nullptr);
    ~LeaveManager();

    /**
     * @brief Create leave request
     * @param personnelId Personnel ID
     * @param startDate Start date
     * @param endDate End date
     * @param leaveType Leave type
     * @param reason Reason for leave
     * @return Request ID or empty string if failed
     */
    QString createLeaveRequest(const QString& personnelId,
                              const QDateTime& startDate,
                              const QDateTime& endDate,
                              LeaveType leaveType,
                              const QString& reason);

    /**
     * @brief Approve leave request
     * @param requestId Request ID
     * @param approverId Approver ID
     * @return true if approval successful, false otherwise
     */
    bool approveLeaveRequest(const QString& requestId, const QString& approverId);

    /**
     * @brief Reject leave request
     * @param requestId Request ID
     * @param approverId Approver ID
     * @param reason Rejection reason
     * @return true if rejection successful, false otherwise
     */
    bool rejectLeaveRequest(const QString& requestId, const QString& approverId, const QString& reason);

    /**
     * @brief Get leave requests
     * @param personnelId Optional personnel ID filter
     * @param status Optional status filter
     * @return List of leave requests
     */
    QList<LeaveRequest> getLeaveRequests(const QString& personnelId = QString(),
                                        LeaveStatus status = LeaveStatus::Pending) const;

    /**
     * @brief Get leave request by ID
     * @param requestId Request ID
     * @return Leave request or empty request if not found
     */
    LeaveRequest getLeaveRequest(const QString& requestId) const;

Q_SIGNALS:
    /**
     * @brief Emitted when leave request is created
     */
    void leaveRequestCreated(const QString& requestId);

    /**
     * @brief Emitted when leave request is approved
     */
    void leaveRequestApproved(const QString& requestId);

    /**
     * @brief Emitted when leave request is rejected
     */
    void leaveRequestRejected(const QString& requestId);

private:
    QList<LeaveRequest> m_leaveRequests;
    QString generateRequestId() const;
};

#endif // LEAVE_MANAGER_H

