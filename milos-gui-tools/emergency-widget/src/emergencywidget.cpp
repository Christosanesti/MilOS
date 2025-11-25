#include "emergencywidget.h"
#include <QtQml/qqml.h>

EmergencyWidget::EmergencyWidget(QObject *parent)
    : QObject(parent)
    , m_networkKillSwitch(new NetworkKillSwitch(this))
    , m_screenLock(new ScreenLock(this))
    , m_secureMode(new SecureMode(this))
    , m_emergencyShutdown(new EmergencyShutdown(this))
    , m_auditLogger(new AuditLogger(this))
    , m_widgetConfig(new WidgetConfig(this))
    , m_keyboardShortcut(new KeyboardShortcut(this))
    , m_statusIndicators(new StatusIndicators(this))
{
    // Register keyboard shortcut
    m_keyboardShortcut->setShortcut(m_widgetConfig->keyboardShortcut());
    m_keyboardShortcut->registerShortcut();
}

void EmergencyWidget::registerTypes()
{
    qmlRegisterType<NetworkKillSwitch>("EmergencyWidget", 1, 0, "NetworkKillSwitch");
    qmlRegisterType<ScreenLock>("EmergencyWidget", 1, 0, "ScreenLock");
    qmlRegisterType<SecureMode>("EmergencyWidget", 1, 0, "SecureMode");
    qmlRegisterType<EmergencyShutdown>("EmergencyWidget", 1, 0, "EmergencyShutdown");
    qmlRegisterType<AuditLogger>("EmergencyWidget", 1, 0, "AuditLogger");
    qmlRegisterType<WidgetConfig>("EmergencyWidget", 1, 0, "WidgetConfig");
    qmlRegisterType<KeyboardShortcut>("EmergencyWidget", 1, 0, "KeyboardShortcut");
    qmlRegisterType<StatusIndicators>("EmergencyWidget", 1, 0, "StatusIndicators");
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
    context->setContextProperty("widgetConfig", widget->m_widgetConfig);
    context->setContextProperty("keyboardShortcut", widget->m_keyboardShortcut);
    context->setContextProperty("statusIndicators", widget->m_statusIndicators);
}















