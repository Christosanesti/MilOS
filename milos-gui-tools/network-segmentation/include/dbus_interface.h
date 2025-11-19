#ifndef DBUS_INTERFACE_H
#define DBUS_INTERFACE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

/**
 * @brief D-Bus Interface for Network Segmentation
 * 
 * Provides D-Bus interface org.milos.NetworkSegmentation
 */
class DBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.NetworkSegmentation")

public:
    explicit DBusInterface(QObject* parent = nullptr);
    ~DBusInterface();

    /**
     * @brief Initialize D-Bus interface
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Create segment
     * @param name Segment name
     * @param networkAddress Network address (CIDR)
     * @param description Segment description
     * @return Segment ID if successful, empty string otherwise
     */
    Q_SCRIPTABLE QString CreateSegment(const QString& name, const QString& networkAddress, const QString& description = QString());

    /**
     * @brief Update segment
     * @param segmentId Segment ID
     * @param name Segment name
     * @param networkAddress Network address
     * @param description Segment description
     * @return true if update successful, false otherwise
     */
    Q_SCRIPTABLE bool UpdateSegment(const QString& segmentId, const QString& name, const QString& networkAddress, const QString& description = QString());

    /**
     * @brief Delete segment
     * @param segmentId Segment ID
     * @return true if deletion successful, false otherwise
     */
    Q_SCRIPTABLE bool DeleteSegment(const QString& segmentId);

    /**
     * @brief Get segment
     * @param segmentId Segment ID
     * @return Segment information as JSON string
     */
    Q_SCRIPTABLE QString GetSegment(const QString& segmentId);

    /**
     * @brief List segments
     * @return List of segment IDs
     */
    Q_SCRIPTABLE QStringList ListSegments();

    /**
     * @brief Get segments by network
     * @param networkAddress Network address
     * @return List of segment IDs
     */
    Q_SCRIPTABLE QStringList GetSegmentsByNetwork(const QString& networkAddress);

    /**
     * @brief Generate firewall rules from segments
     * @return true if generation successful, false otherwise
     */
    Q_SCRIPTABLE bool GenerateFirewallRules();

    /**
     * @brief Validate firewall rules
     * @return Validation result as JSON string
     */
    Q_SCRIPTABLE QString ValidateFirewallRules();

    /**
     * @brief Preview firewall rules
     * @return Rules preview as string
     */
    Q_SCRIPTABLE QString PreviewFirewallRules();

    /**
     * @brief Apply firewall rules
     * @return true if application successful, false otherwise
     */
    Q_SCRIPTABLE bool ApplyFirewallRules();

    /**
     * @brief Rollback firewall rules
     * @return true if rollback successful, false otherwise
     */
    Q_SCRIPTABLE bool RollbackFirewallRules();

    /**
     * @brief Enforce isolation for segment
     * @param segmentId Segment ID
     * @return true if enforcement successful, false otherwise
     */
    Q_SCRIPTABLE bool EnforceIsolation(const QString& segmentId);

    /**
     * @brief Remove isolation for segment
     * @param segmentId Segment ID
     * @return true if removal successful, false otherwise
     */
    Q_SCRIPTABLE bool RemoveIsolation(const QString& segmentId);

    /**
     * @brief Verify segment isolation
     * @param segmentId Segment ID
     * @return true if segment is isolated, false otherwise
     */
    Q_SCRIPTABLE bool VerifyIsolation(const QString& segmentId);

    /**
     * @brief Set segment manager
     */
    void setSegmentManager(class SegmentManager* segmentManager);

    /**
     * @brief Set firewall manager
     */
    void setFirewallManager(class FirewallManager* firewallManager);

    /**
     * @brief Set isolation enforcement
     */
    void setIsolationEnforcement(class IsolationEnforcement* isolationEnforcement);

Q_SIGNALS:
    /**
     * @brief Emitted when segment is created
     */
    Q_SCRIPTABLE void SegmentCreated(const QString& segmentId);

    /**
     * @brief Emitted when segment is updated
     */
    Q_SCRIPTABLE void SegmentUpdated(const QString& segmentId);

    /**
     * @brief Emitted when segment is deleted
     */
    Q_SCRIPTABLE void SegmentDeleted(const QString& segmentId);

private:
    bool m_initialized;
    class SegmentManager* m_segmentManager;
    class FirewallManager* m_firewallManager;
    class IsolationEnforcement* m_isolationEnforcement;
};

#endif // DBUS_INTERFACE_H

