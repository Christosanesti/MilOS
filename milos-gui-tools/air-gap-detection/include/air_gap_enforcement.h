#ifndef AIR_GAP_ENFORCEMENT_H
#define AIR_GAP_ENFORCEMENT_H

#include "air_gap_detector.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

/**
 * @brief Security Policy Level
 */
enum class SecurityPolicyLevel {
    Normal,         // Normal security when connected
    Enhanced,       // Enhanced security when air-gapped
    Maximum         // Maximum security (air-gapped + additional restrictions)
};

/**
 * @brief Air-Gap Enforcement
 * 
 * Enforces security policies based on air-gap status.
 */
class AirGapEnforcement : public QObject {
    Q_OBJECT
    Q_PROPERTY(SecurityPolicyLevel currentPolicyLevel READ currentPolicyLevel NOTIFY policyLevelChanged)
    Q_PROPERTY(bool dataLeakagePreventionEnabled READ dataLeakagePreventionEnabled NOTIFY policyLevelChanged)

public:
    explicit AirGapEnforcement(QObject* parent = nullptr);
    ~AirGapEnforcement();

    /**
     * @brief Initialize air-gap enforcement
     * @param detector Air-gap detector
     * @return true if initialization successful, false otherwise
     */
    bool initialize(AirGapDetector* detector);

    /**
     * @brief Get current policy level
     * @return Current policy level
     */
    SecurityPolicyLevel currentPolicyLevel() const { return m_currentPolicyLevel; }

    /**
     * @brief Check if data leakage prevention is enabled
     * @return true if enabled, false otherwise
     */
    bool dataLeakagePreventionEnabled() const { return m_dataLeakagePreventionEnabled; }

    /**
     * @brief Check if operation is allowed
     * @param operation Operation name
     * @param details Operation details
     * @return true if allowed, false otherwise
     */
    bool isOperationAllowed(const QString& operation, const QVariantMap& details = QVariantMap()) const;

    /**
     * @brief Get blocked operations list
     * @return List of blocked operations
     */
    QStringList getBlockedOperations() const;

    /**
     * @brief Get security policy configuration
     * @return Policy configuration
     */
    QVariantMap getPolicyConfiguration() const;

Q_SIGNALS:
    /**
     * @brief Emitted when policy level changes
     */
    void policyLevelChanged(SecurityPolicyLevel level);

    /**
     * @brief Emitted when operation is blocked
     */
    void operationBlocked(const QString& operation, const QString& reason);

private slots:
    void onAirGapStatusChanged(AirGapStatus status);

private:
    AirGapDetector* m_detector;
    SecurityPolicyLevel m_currentPolicyLevel;
    bool m_dataLeakagePreventionEnabled;
    QStringList m_blockedOperations;
    
    void updatePolicyLevel(AirGapStatus status);
    void applySecurityPolicies(SecurityPolicyLevel level);
    bool isDataLeakageOperation(const QString& operation) const;
};

Q_DECLARE_METATYPE(SecurityPolicyLevel)

#endif // AIR_GAP_ENFORCEMENT_H

