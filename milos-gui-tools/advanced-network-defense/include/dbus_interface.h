#ifndef ADVANCED_NETWORK_DEFENSE_DBUS_INTERFACE_H
#define ADVANCED_NETWORK_DEFENSE_DBUS_INTERFACE_H

#include "advanced_network_defense_system.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

class AdvancedNetworkDefenseSystem;

/**
 * @brief Advanced Network Defense D-Bus Interface
 */
class AdvancedNetworkDefenseDBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.AdvancedNetworkDefense")

public:
    explicit AdvancedNetworkDefenseDBusInterface(QObject* parent = nullptr);
    ~AdvancedNetworkDefenseDBusInterface();

    bool initialize();
    void setSystem(AdvancedNetworkDefenseSystem* system);

public slots:
    QVariantMap DetectThreats(const QVariantMap& networkData);
    bool TrainModel(const QStringList& trainingData);
    double GetModelAccuracy();
    bool CaptureEvidence(const QString& type, const QString& source, const QByteArray& data);
    QVariantMap GetEvidence(const QString& evidenceId);
    QVariantMap AnalyzePacket(const QByteArray& packetData);
    QStringList DetectAnomalies(const QByteArray& packetData);
    bool AddIDSRule(const QString& name, const QString& pattern, const QString& action);
    QStringList GetIDSRules();
    QString CreateWorkflow(const QString& name, const QStringList& steps);
    bool ExecuteWorkflow(const QString& workflowId, const QVariantMap& threat);

private:
    AdvancedNetworkDefenseSystem* m_system;
};

#endif // ADVANCED_NETWORK_DEFENSE_DBUS_INTERFACE_H

