#include "emergencywidget.h"
#include <QtQml/qqml.h>

EmergencyWidget::EmergencyWidget(QObject *parent)
    : QObject(parent)
    , m_networkKillSwitch(new NetworkKillSwitch(this))
    , m_screenLock(new ScreenLock(this))
    , m_secureMode(new SecureMode(this))
    , m_emergencyShutdown(new EmergencyShutdown(this))
    , m_auditLogger(new AuditLogger(this))
{
}

void EmergencyWidget::registerTypes()
{
    qmlRegisterType<NetworkKillSwitch>("EmergencyWidget", 1, 0, "NetworkKillSwitch");
    qmlRegisterType<ScreenLock>("EmergencyWidget", 1, 0, "ScreenLock");
    qmlRegisterType<SecureMode>("EmergencyWidget", 1, 0, "SecureMode");
    qmlRegisterType<EmergencyShutdown>("EmergencyWidget", 1, 0, "EmergencyShutdown");
    qmlRegisterType<AuditLogger>("EmergencyWidget", 1, 0, "AuditLogger");
}

void EmergencyWidget::registerServices(QQmlContext *context)
{
    if (!context) return;
    
    EmergencyWidget *widget = new EmergencyWidget();
    
    context->setContextProperty("networkKillSwitch", widget->m_networkKillSwitch);
    context->setContextProperty("screenLock", widget->m_screenLock);
    context->setContextProperty("secureMode", widget->m_secureMode);
    context->setContextProperty("emergencyShutdown", widget->m_emergencyShutdown);
    context->setContextProperty("auditLogger", widget->m_auditLogger);
}






