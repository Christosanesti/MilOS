#ifndef EMERGENCYWIDGET_H
#define EMERGENCYWIDGET_H

#include <QObject>
#include <QQmlEngine>
#include <QQmlContext>
#include "services/networkkillswitch.h"
#include "services/screenlock.h"
#include "services/securemode.h"
#include "services/emergencyshutdown.h"
#include "services/auditlogger.h"

class EmergencyWidget : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit EmergencyWidget(QObject *parent = nullptr);
    
    static void registerTypes();
    static void registerServices(QQmlContext *context);

private:
    NetworkKillSwitch *m_networkKillSwitch;
    ScreenLock *m_screenLock;
    SecureMode *m_secureMode;
    EmergencyShutdown *m_emergencyShutdown;
    AuditLogger *m_auditLogger;
};

#endif // EMERGENCYWIDGET_H









