#include "styling.h"
#include <QDebug>

PerformanceStyling::PerformanceStyling(QObject *parent)
    : QObject(parent)
    , m_currentMode("standard")
{
    updateStyleProperties();
}

void PerformanceStyling::setCurrentMode(const QString &mode)
{
    if (m_currentMode != mode) {
        m_currentMode = mode;
        updateStyleProperties();
        emit currentModeChanged();
        emit stylePropertiesChanged();
    }
}

void PerformanceStyling::applyMode(const QString &mode)
{
    setCurrentMode(mode);
}

QVariantMap PerformanceStyling::getModeProperties(const QString &mode)
{
    if (mode == "standard" || mode.isEmpty()) {
        return getStandardModeProperties();
    } else if (mode == "performance") {
        return getPerformanceModeProperties();
    } else if (mode == "low-power") {
        return getLowPowerModeProperties();
    }
    return getStandardModeProperties();
}

double PerformanceStyling::getXenonIntensity(const QString &mode)
{
    QVariantMap props = getModeProperties(mode);
    return props.value("xenonIntensity").toDouble();
}

double PerformanceStyling::getGlowRadius(const QString &mode)
{
    QVariantMap props = getModeProperties(mode);
    return props.value("glowRadius").toDouble();
}

bool PerformanceStyling::areParticlesEnabled(const QString &mode)
{
    QVariantMap props = getModeProperties(mode);
    return props.value("particlesEnabled").toBool();
}

bool PerformanceStyling::areAnimationsSimplified(const QString &mode)
{
    QVariantMap props = getModeProperties(mode);
    return props.value("animationsSimplified").toBool();
}

void PerformanceStyling::updateStyleProperties()
{
    m_styleProperties = getModeProperties(m_currentMode);
}

QVariantMap PerformanceStyling::getStandardModeProperties()
{
    QVariantMap props;
    props["mode"] = "standard";
    props["xenonIntensity"] = 1.0; // 100% intensity
    props["glowRadius"] = 1.0; // 100% glow radius
    props["particlesEnabled"] = true;
    props["animationsSimplified"] = false;
    props["animationDuration"] = 1.0; // Full duration
    props["effectQuality"] = "high";
    props["shadowQuality"] = "high";
    props["blurQuality"] = "high";
    return props;
}

QVariantMap PerformanceStyling::getPerformanceModeProperties()
{
    QVariantMap props;
    props["mode"] = "performance";
    props["xenonIntensity"] = 0.5; // 50% intensity (reduced)
    props["glowRadius"] = 0.6; // 60% glow radius (reduced)
    props["particlesEnabled"] = false; // No particles
    props["animationsSimplified"] = true; // Simplified animations
    props["animationDuration"] = 0.7; // 70% duration (faster)
    props["effectQuality"] = "medium";
    props["shadowQuality"] = "medium";
    props["blurQuality"] = "medium";
    return props;
}

QVariantMap PerformanceStyling::getLowPowerModeProperties()
{
    QVariantMap props;
    props["mode"] = "low-power";
    props["xenonIntensity"] = 0.2; // 20% intensity (minimal)
    props["glowRadius"] = 0.3; // 30% glow radius (minimal)
    props["particlesEnabled"] = false; // No particles
    props["animationsSimplified"] = true; // Simplified animations
    props["animationDuration"] = 0.5; // 50% duration (minimal)
    props["effectQuality"] = "low";
    props["shadowQuality"] = "low";
    props["blurQuality"] = "low";
    return props;
}

