#ifndef SECURITY_ZONE_ENFORCEMENT_H
#define SECURITY_ZONE_ENFORCEMENT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include "segment_config.h"

/**
 * @brief Security Zone Policy
 */
struct SecurityZonePolicy {
    QString zoneId;
    QString name;
    QString description;
    QStringList allowedSegments;  // Segments allowed in this zone
    QStringList blockedSegments;  // Segments blocked from this zone
    QStringList allowedIPs;  // IPs allowed in this zone
    QStringList blockedIPs;  // IPs blocked from this zone
    bool enforceIsolation;  // Enforce strict isolation
    int priority;  // Policy priority
};

/**
 * @brief Security Zone Enforcement Manager
 * 
 * Enforces security zone policies for network segments.
 */
class SecurityZoneEnforcement : public QObject {
    Q_OBJECT

public:
    explicit SecurityZoneEnforcement(QObject* parent = nullptr);
    ~SecurityZoneEnforcement();

    /**
     * @brief Initialize zone enforcement
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Create security zone
     * @param policy Zone policy
     * @return Zone ID if successful, empty string otherwise
     */
    QString createZone(const SecurityZonePolicy& policy);

    /**
     * @brief Update security zone
     * @param zoneId Zone ID
     * @param policy Updated policy
     * @return true if update successful, false otherwise
     */
    bool updateZone(const QString& zoneId, const SecurityZonePolicy& policy);

    /**
     * @brief Delete security zone
     * @param zoneId Zone ID
     * @return true if deletion successful, false otherwise
     */
    bool deleteZone(const QString& zoneId);

    /**
     * @brief Enforce zone policy for segment
     * @param segmentId Segment ID
     * @param zoneId Zone ID
     * @return true if enforcement successful, false otherwise
     */
    bool enforceZonePolicy(const QString& segmentId, const QString& zoneId);

    /**
     * @brief Check if segment is allowed in zone
     * @param segmentId Segment ID
     * @param zoneId Zone ID
     * @return true if allowed, false otherwise
     */
    bool isSegmentAllowed(const QString& segmentId, const QString& zoneId) const;

    /**
     * @brief Get zones for segment
     * @param segmentId Segment ID
     * @return List of zone IDs
     */
    QStringList getZonesForSegment(const QString& segmentId) const;

Q_SIGNALS:
    /**
     * @brief Emitted when zone is created
     */
    void zoneCreated(const QString& zoneId);

    /**
     * @brief Emitted when zone is updated
     */
    void zoneUpdated(const QString& zoneId);

    /**
     * @brief Emitted when zone is deleted
     */
    void zoneDeleted(const QString& zoneId);

    /**
     * @brief Emitted when zone policy is enforced
     */
    void zonePolicyEnforced(const QString& segmentId, const QString& zoneId);

private:
    bool m_initialized;
    QMap<QString, SecurityZonePolicy> m_zones;
    QMap<QString, QStringList> m_segmentZones;  // segmentId -> zoneIds

    /**
     * @brief Apply zone policy rules
     */
    bool applyZonePolicyRules(const SecurityZonePolicy& policy);
};

#endif // SECURITY_ZONE_ENFORCEMENT_H

