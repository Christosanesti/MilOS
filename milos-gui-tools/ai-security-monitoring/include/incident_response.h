#ifndef INCIDENT_RESPONSE_H
#define INCIDENT_RESPONSE_H

#include "threat_detector.h"
#include <QObject>
#include <QString>
#include <QStringList>

/**
 * @brief Response Action
 */
enum class ResponseAction {
    Contain,
    Quarantine,
    Block,
    Isolate,
    Notify,
    Log
};

/**
 * @brief Incident Response
 * 
 * Automated incident response system.
 */
class IncidentResponse : public QObject {
    Q_OBJECT

public:
    explicit IncidentResponse(QObject* parent = nullptr);
    ~IncidentResponse();

    /**
     * @brief Initialize incident response
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Respond to threat
     * @param threat Detected threat
     * @return true if response successful, false otherwise
     */
    bool respondToThreat(const DetectedThreat& threat);

    /**
     * @brief Contain threat
     * @param threatId Threat ID
     * @return true if contain successful, false otherwise
     */
    bool containThreat(const QString& threatId);

    /**
     * @brief Quarantine resource
     * @param resourcePath Resource path
     * @return true if quarantine successful, false otherwise
     */
    bool quarantineResource(const QString& resourcePath);

    /**
     * @brief Block network connection
     * @param source Source address
     * @param destination Destination address
     * @return true if block successful, false otherwise
     */
    bool blockConnection(const QString& source, const QString& destination);

Q_SIGNALS:
    /**
     * @brief Emitted when threat is contained
     */
    void threatContained(const QString& threatId);

    /**
     * @brief Emitted when resource is quarantined
     */
    void resourceQuarantined(const QString& resourcePath);

    /**
     * @brief Emitted when connection is blocked
     */
    void connectionBlocked(const QString& source, const QString& destination);

private:
    QStringList m_containedThreats;
    QStringList m_quarantinedResources;
    
    ResponseAction determineResponseAction(const DetectedThreat& threat) const;
};

Q_DECLARE_METATYPE(ResponseAction)

#endif // INCIDENT_RESPONSE_H

