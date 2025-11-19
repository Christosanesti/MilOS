#ifndef ISOLATION_ENFORCEMENT_H
#define ISOLATION_ENFORCEMENT_H

#include <QObject>
#include <QString>
#include "network_isolation.h"
#include "security_zone_enforcement.h"
#include "access_control.h"
#include "segment_config.h"

/**
 * @brief Isolation Enforcement Manager
 * 
 * Orchestrates network isolation enforcement, security zone enforcement,
 * and policy-based access control.
 */
class IsolationEnforcement : public QObject {
    Q_OBJECT

public:
    explicit IsolationEnforcement(QObject* parent = nullptr);
    ~IsolationEnforcement();

    /**
     * @brief Initialize isolation enforcement
     * @return true if initialization successful, false otherwise
     */
    Q_INVOKABLE bool initialize();

    /**
     * @brief Enforce isolation for segment
     * @param segment Segment configuration
     * @return true if enforcement successful, false otherwise
     */
    Q_INVOKABLE bool enforceIsolation(const SegmentConfig& segment);

    /**
     * @brief Remove isolation for segment
     * @param segmentId Segment ID
     * @return true if removal successful, false otherwise
     */
    Q_INVOKABLE bool removeIsolation(const QString& segmentId);

    /**
     * @brief Verify segment isolation
     * @param segmentId Segment ID
     * @return true if segment is isolated, false otherwise
     */
    Q_INVOKABLE bool verifyIsolation(const QString& segmentId) const;

Q_SIGNALS:
    /**
     * @brief Emitted when isolation is enforced
     */
    void isolationEnforced(const QString& segmentId);

    /**
     * @brief Emitted when isolation is removed
     */
    void isolationRemoved(const QString& segmentId);

private:
    bool m_initialized;
    NetworkIsolation* m_networkIsolation;
    SecurityZoneEnforcement* m_zoneEnforcement;
    AccessControl* m_accessControl;
};

#endif // ISOLATION_ENFORCEMENT_H

