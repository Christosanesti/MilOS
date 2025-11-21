#ifndef MILITARY_RADIO_DBUS_INTERFACE_H
#define MILITARY_RADIO_DBUS_INTERFACE_H

#include "military_radio_system.h"
#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVariantMap>

class MilitaryRadioSystem;

/**
 * @brief Military Radio D-Bus Interface
 */
class MilitaryRadioDBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.MilitaryRadio")

public:
    explicit MilitaryRadioDBusInterface(QObject* parent = nullptr);
    ~MilitaryRadioDBusInterface();

    bool initialize();
    void setSystem(MilitaryRadioSystem* system);

public slots:
    bool InitializeProtocol(int protocolType, double frequency);
    bool SendMessage(int protocolType, const QString& to, const QByteArray& payload, int priority);
    QVariantMap ReceiveMessage(int protocolType);
    bool SetEncryptionKey(const QByteArray& key);
    QStringList GetAvailableProtocols();

private:
    MilitaryRadioSystem* m_system;
};

#endif // MILITARY_RADIO_DBUS_INTERFACE_H

