#ifndef THREAT_INTELLIGENCE_DBUS_INTERFACE_H
#define THREAT_INTELLIGENCE_DBUS_INTERFACE_H

#include "threat_intelligence_system.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

class ThreatIntelligenceSystem;

/**
 * @brief Threat Intelligence D-Bus Interface
 */
class ThreatIntelligenceDBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.ThreatIntelligence")

public:
    explicit ThreatIntelligenceDBusInterface(QObject* parent = nullptr);
    ~ThreatIntelligenceDBusInterface();

    bool initialize();
    void setSystem(ThreatIntelligenceSystem* system);

public slots:
    QStringList GetFeeds();
    QVariantMap GetFeedStatus(const QString& feedId);
    bool EnableFeed(const QString& feedId, bool enabled);
    QVariantMap GetThreatIndicators(const QString& feedId);
    QVariantMap CorrelateThreats();
    bool BlockIndicator(const QString& indicatorId);
    bool UnblockIndicator(const QString& indicatorId);
    QStringList GetBlockedIndicators();
    bool SetAutoBlocking(bool enabled);

private:
    ThreatIntelligenceSystem* m_system;
};

#endif // THREAT_INTELLIGENCE_DBUS_INTERFACE_H

