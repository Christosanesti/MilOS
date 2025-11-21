#ifndef SATELLITE_COMMUNICATION_DBUS_INTERFACE_H
#define SATELLITE_COMMUNICATION_DBUS_INTERFACE_H

#include "satellite_communication_system.h"
#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVariantMap>

class SatelliteCommunicationSystem;

/**
 * @brief Satellite Communication D-Bus Interface
 */
class SatelliteCommunicationDBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.SatelliteCommunication")

public:
    explicit SatelliteCommunicationDBusInterface(QObject* parent = nullptr);
    ~SatelliteCommunicationDBusInterface();

    bool initialize();
    void setSystem(SatelliteCommunicationSystem* system);

public slots:
    bool InitializeProtocol(int protocolType, const QString& satelliteId);
    bool SendMessage(int protocolType, const QString& to, const QByteArray& payload, int priority);
    QVariantMap ReceiveMessage(int protocolType);
    double GetAvailableBandwidth(int protocolType);
    QByteArray OptimizeData(const QByteArray& data);

private:
    SatelliteCommunicationSystem* m_system;
};

#endif // SATELLITE_COMMUNICATION_DBUS_INTERFACE_H

