#ifndef SIGNAL_JAMMING_DBUS_INTERFACE_H
#define SIGNAL_JAMMING_DBUS_INTERFACE_H

#include "jamming_manager.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

class JammingManager;

/**
 * @brief Signal Jamming D-Bus Interface
 */
class SignalJammingDBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.SignalJamming")

public:
    explicit SignalJammingDBusInterface(QObject* parent = nullptr);
    ~SignalJammingDBusInterface();

    bool initialize();
    void setManager(JammingManager* manager);

public slots:
    QStringList GetDevices();
    QVariantMap GetDeviceStatus(const QString& deviceId);
    QString CreatePattern(const QString& name, double frequencyStart, double frequencyEnd, double power, const QString& patternType);
    bool StartJamming(const QString& deviceId, const QString& patternId);
    bool StopJamming(const QString& deviceId);
    bool StartSpectrumAnalysis(double frequencyStart, double frequencyEnd);
    void StopSpectrumAnalysis();
    QVariantMap GetSpectrumData();

private:
    JammingManager* m_manager;
};

#endif // SIGNAL_JAMMING_DBUS_INTERFACE_H

