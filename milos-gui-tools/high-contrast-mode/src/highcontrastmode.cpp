#include "highcontrastmode.h"
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDateTime>

HighContrastMode::HighContrastMode(QObject* parent)
    : QObject(parent)
    , m_modeEnabled(false)
    , m_xenonIntensityMultiplier(1.5)  // 150% standard
    , m_xenonGlowRadiusMultiplier(2.0)  // 200% standard
    , m_borderThicknessMultiplier(2.0)  // 2x standard
    , m_clickTargetSize(48)  // 48px minimum
    , m_settings(nullptr)
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos/high-contrast-mode.ini";
    m_settings = new QSettings(configPath, QSettings::IniFormat, this);
    
    loadConfiguration();
}

HighContrastMode::~HighContrastMode()
{
    saveConfiguration();
}

void HighContrastMode::loadConfiguration()
{
    m_settings->beginGroup("HighContrastMode");
    m_modeEnabled = m_settings->value("modeEnabled", false).toBool();
    m_xenonIntensityMultiplier = m_settings->value("xenonIntensityMultiplier", 1.5).toDouble();
    m_xenonGlowRadiusMultiplier = m_settings->value("xenonGlowRadiusMultiplier", 2.0).toDouble();
    m_borderThicknessMultiplier = m_settings->value("borderThicknessMultiplier", 2.0).toDouble();
    m_clickTargetSize = m_settings->value("clickTargetSize", 48).toInt();
    m_settings->endGroup();

    if (m_modeEnabled) {
        applyTheme();
        adjustXenonEffects();
    }
}

void HighContrastMode::saveConfiguration()
{
    m_settings->beginGroup("HighContrastMode");
    m_settings->setValue("modeEnabled", m_modeEnabled);
    m_settings->setValue("xenonIntensityMultiplier", m_xenonIntensityMultiplier);
    m_settings->setValue("xenonGlowRadiusMultiplier", m_xenonGlowRadiusMultiplier);
    m_settings->setValue("borderThicknessMultiplier", m_borderThicknessMultiplier);
    m_settings->setValue("clickTargetSize", m_clickTargetSize);
    m_settings->setValue("lastModified", QDateTime::currentDateTime().toString(Qt::ISODate));
    m_settings->endGroup();
    m_settings->sync();
}

void HighContrastMode::activateMode()
{
    if (m_modeEnabled) {
        return;
    }

    m_modeEnabled = true;
    saveConfiguration();
    applyTheme();
    adjustXenonEffects();
    
    emit modeEnabledChanged(true);
    emit modeActivated();
}

void HighContrastMode::deactivateMode()
{
    if (!m_modeEnabled) {
        return;
    }

    m_modeEnabled = false;
    saveConfiguration();
    applyTheme();
    adjustXenonEffects();
    
    emit modeEnabledChanged(false);
    emit modeDeactivated();
}

void HighContrastMode::toggleMode()
{
    if (m_modeEnabled) {
        deactivateMode();
    } else {
        activateMode();
    }
}

QVariantMap HighContrastMode::getModeConfig() const
{
    QVariantMap config;
    config["modeEnabled"] = m_modeEnabled;
    config["xenonIntensityMultiplier"] = m_xenonIntensityMultiplier;
    config["xenonGlowRadiusMultiplier"] = m_xenonGlowRadiusMultiplier;
    config["borderThicknessMultiplier"] = m_borderThicknessMultiplier;
    config["clickTargetSize"] = m_clickTargetSize;
    return config;
}

void HighContrastMode::setModeConfig(const QVariantMap& config)
{
    if (config.contains("xenonIntensityMultiplier")) {
        m_xenonIntensityMultiplier = config.value("xenonIntensityMultiplier").toDouble();
    }
    if (config.contains("xenonGlowRadiusMultiplier")) {
        m_xenonGlowRadiusMultiplier = config.value("xenonGlowRadiusMultiplier").toDouble();
    }
    if (config.contains("borderThicknessMultiplier")) {
        m_borderThicknessMultiplier = config.value("borderThicknessMultiplier").toDouble();
    }
    if (config.contains("clickTargetSize")) {
        m_clickTargetSize = config.value("clickTargetSize").toInt();
    }

    saveConfiguration();
    if (m_modeEnabled) {
        adjustXenonEffects();
    }
    emit configChanged();
}

void HighContrastMode::applyTheme()
{
    // Apply high-contrast theme via KDE Plasma theme API
    QDBusConnection connection = QDBusConnection::sessionBus();
    QDBusInterface themeInterface("org.kde.KWin", "/KWin", "org.kde.KWin", connection);

    if (!themeInterface.isValid()) {
        qWarning() << "Cannot connect to KDE Plasma theme API";
        return;
    }

    if (m_modeEnabled) {
        // Switch to high-contrast theme variant
        QDBusMessage message = QDBusMessage::createMethodCall(
            "org.kde.KWin",
            "/KWin",
            "org.kde.KWin",
            "setTheme"
        );
        message << "milos-high-contrast";
        connection.call(message);
    } else {
        // Switch back to standard MilOS theme
        QDBusMessage message = QDBusMessage::createMethodCall(
            "org.kde.KWin",
            "/KWin",
            "org.kde.KWin",
            "setTheme"
        );
        message << "milos";
        connection.call(message);
    }
}

void HighContrastMode::adjustXenonEffects()
{
    // Adjust xenon effects via xenon effects library D-Bus interface
    QDBusConnection connection = QDBusConnection::sessionBus();
    QDBusInterface xenonInterface("org.milos.XenonEffects", "/XenonEffects", "org.milos.XenonEffects", connection);

    if (!xenonInterface.isValid()) {
        qDebug() << "Xenon effects service not available";
        return;
    }

    if (m_modeEnabled) {
        // Set enhanced xenon effect parameters
        QDBusMessage message = QDBusMessage::createMethodCall(
            "org.milos.XenonEffects",
            "/XenonEffects",
            "org.milos.XenonEffects",
            "setEffectParameters"
        );
        QVariantMap params;
        params["intensityMultiplier"] = m_xenonIntensityMultiplier;
        params["glowRadiusMultiplier"] = m_xenonGlowRadiusMultiplier;
        message << QVariant::fromValue(params);
        connection.call(message);
    } else {
        // Reset to standard parameters
        QDBusMessage message = QDBusMessage::createMethodCall(
            "org.milos.XenonEffects",
            "/XenonEffects",
            "org.milos.XenonEffects",
            "setEffectParameters"
        );
        QVariantMap params;
        params["intensityMultiplier"] = 1.0;
        params["glowRadiusMultiplier"] = 1.0;
        message << QVariant::fromValue(params);
        connection.call(message);
    }
}

