#ifndef EMERGENCYSHUTDOWN_H
#define EMERGENCYSHUTDOWN_H

#include <QObject>

class EmergencyShutdown : public QObject
{
    Q_OBJECT

public:
    explicit EmergencyShutdown(QObject *parent = nullptr);
    
    Q_INVOKABLE void shutdown();

signals:
    void shutdownInitiated();
    void error(const QString &errorMessage);

private:
    // TODO: Integrate with systemd-logind D-Bus interface
    bool shutdownViaSystemd();
};

#endif // EMERGENCYSHUTDOWN_H

