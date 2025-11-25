#include "performancemode.h"
#include <QDebug>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>

PerformanceMode::PerformanceMode(QObject* parent)
    : QObject(parent)
    , m_currentMode("standard")
    , m_autoSwitchEnabled(true)
    , m_monitor(new PerformanceMonitor(this))
{
    connect(m_monitor, &PerformanceMonitor::performanceDegraded, this, &PerformanceMode::onPerformanceDegraded);
    connect(m_monitor, &PerformanceMonitor::performanceRecovered, this, &PerformanceMode::onPerformanceRecovered);
    
    m_monitor->startMonitoring();
}

PerformanceMode::~PerformanceMode()
{
    m_monitor->stopMonitoring();
}

void PerformanceMode::setMode(const QString& mode)
{
    if (m_currentMode == mode) {
        return;
    }

    m_currentMode = mode;
    applyMode(mode);
    emit modeChanged(mode);
    emit modeSwitched(mode);
}

void PerformanceMode::enableAutoSwitch(bool enabled)
{
    m_autoSwitchEnabled = enabled;
}

QVariantMap PerformanceMode::getModeConfig() const
{
    QVariantMap config;
    config["currentMode"] = m_currentMode;
    config["autoSwitchEnabled"] = m_autoSwitchEnabled;
    config["metrics"] = m_monitor->getMetrics();
    return config;
}

void PerformanceMode::onPerformanceDegraded()
{
    if (!m_autoSwitchEnabled) {
        return;
    }

    // Switch to performance mode when degradation detected
    if (m_currentMode == "standard") {
        setMode("performance");
    }
}

void PerformanceMode::onPerformanceRecovered()
{
    if (!m_autoSwitchEnabled) {
        return;
    }

    // Switch back to standard mode when performance recovers
    if (m_currentMode == "performance") {
        setMode("standard");
    }
}

void PerformanceMode::applyMode(const QString& mode)
{
    adjustXenonEffects(mode);
}

void PerformanceMode::adjustXenonEffects(const QString& mode)
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    QDBusInterface xenonInterface("org.milos.XenonEffects", "/XenonEffects", "org.milos.XenonEffects", connection);

    if (!xenonInterface.isValid()) {
        qDebug() << "Xenon effects service not available";
        return;
    }

    QVariantMap params;
    if (mode == "performance") {
        params["intensityMultiplier"] = 0.5; // 50% intensity
    } else if (mode == "lowpower") {
        params["intensityMultiplier"] = 0.25; // 25% intensity
    } else {
        params["intensityMultiplier"] = 1.0; // 100% intensity (standard)
    }

    QDBusMessage message = QDBusMessage::createMethodCall(
        "org.milos.XenonEffects",
        "/XenonEffects",
        "org.milos.XenonEffects",
        "setEffectParameters"
    );
    message << QVariant::fromValue(params);
    connection.call(message);
}

