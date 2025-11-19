#ifndef ACCESS_CONTROL_H
#define ACCESS_CONTROL_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include "segment_config.h"

/**
 * @brief Network Access Control Policy
 */
struct AccessControlPolicy {
    QString policyId;
    QString name;
    QString description;
    QString sourceSegment;  // Source segment ID
    QString destinationSegment;  // Destination segment ID
    QStringList allowedProtocols;  // Allowed protocols (TCP, UDP, ICMP, etc.)
    QStringList allowedPorts;  // Allowed ports
    QStringList blockedPorts;  // Blocked ports
    QStringList allowedIPs;  // Allowed IPs
    QStringList blockedIPs;  // Blocked IPs
    bool enabled;  // Policy enabled/disabled
    int priority;  // Policy priority
};

/**
 * @brief Policy-Based Network Access Control Manager
 * 
 * Manages policy-based network access control.
 */
class AccessControl : public QObject {
    Q_OBJECT

public:
    explicit AccessControl(QObject* parent = nullptr);
    ~AccessControl();

    /**
     * @brief Initialize access control
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Create access control policy
     * @param policy Access control policy
     * @return Policy ID if successful, empty string otherwise
     */
    QString createPolicy(const AccessControlPolicy& policy);

    /**
     * @brief Update access control policy
     * @param policyId Policy ID
     * @param policy Updated policy
     * @return true if update successful, false otherwise
     */
    bool updatePolicy(const QString& policyId, const AccessControlPolicy& policy);

    /**
     * @brief Delete access control policy
     * @param policyId Policy ID
     * @return true if deletion successful, false otherwise
     */
    bool deletePolicy(const QString& policyId);

    /**
     * @brief Evaluate access control decision
     * @param sourceSegment Source segment ID
     * @param destinationSegment Destination segment ID
     * @param protocol Protocol
     * @param port Port
     * @return true if access allowed, false otherwise
     */
    bool evaluateAccess(const QString& sourceSegment, const QString& destinationSegment,
                       const QString& protocol, const QString& port) const;

    /**
     * @brief Enforce access control policy
     * @param policyId Policy ID
     * @return true if enforcement successful, false otherwise
     */
    bool enforcePolicy(const QString& policyId);

    /**
     * @brief Get policies for segment
     * @param segmentId Segment ID
     * @return List of policy IDs
     */
    QStringList getPoliciesForSegment(const QString& segmentId) const;

Q_SIGNALS:
    /**
     * @brief Emitted when policy is created
     */
    void policyCreated(const QString& policyId);

    /**
     * @brief Emitted when policy is updated
     */
    void policyUpdated(const QString& policyId);

    /**
     * @brief Emitted when policy is deleted
     */
    void policyDeleted(const QString& policyId);

    /**
     * @brief Emitted when access is denied
     */
    void accessDenied(const QString& sourceSegment, const QString& destinationSegment,
                     const QString& reason);

private:
    bool m_initialized;
    QMap<QString, AccessControlPolicy> m_policies;
    QMap<QString, QStringList> m_segmentPolicies;  // segmentId -> policyIds

    /**
     * @brief Apply policy rules
     */
    bool applyPolicyRules(const AccessControlPolicy& policy);
};

#endif // ACCESS_CONTROL_H

