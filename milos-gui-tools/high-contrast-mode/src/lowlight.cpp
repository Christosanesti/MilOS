#include "lowlight.h"
#include <QStandardPaths>
#include <QDebug>

LowLight::LowLight(QObject* parent)
    : QObject(parent)
    , m_enabled(false)
    , m_brightnessLevel(50)
    , m_settings(nullptr)
{
    loadPreferences();
}

LowLight::~LowLight()
{
    if (m_settings) {
        delete m_settings;
    }
}

void LowLight::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        savePreferences();
        emit enabledChanged();
        if (enabled) {
            optimizeForLowLight();
        }
    }
}

void LowLight::setBrightnessLevel(int level)
{
    if (level < 0) level = 0;
    if (level > 100) level = 100;
    
    if (m_brightnessLevel != level) {
        m_brightnessLevel = level;
        savePreferences();
        emit brightnessLevelChanged();
        if (m_enabled) {
            optimizeForLowLight();
        }
    }
}

QColor LowLight::getOptimizedColor(const QColor& baseColor)
{
    if (!m_enabled) {
        return baseColor;
    }
    return adjustColorForLowLight(baseColor);
}

int LowLight::getOptimizedFontSize(int baseSize)
{
    if (!m_enabled) {
        return baseSize;
    }
    // Increase font size by 10-20% for low-light visibility
    return baseSize + (baseSize * (100 - m_brightnessLevel) / 500);
}

void LowLight::optimizeForLowLight()
{
    emit optimizationApplied();
}

void LowLight::savePreferences()
{
    if (!m_settings) {
        QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos/high-contrast-mode.ini";
        m_settings = new QSettings(configPath, QSettings::IniFormat, this);
    }
    
    m_settings->beginGroup("LowLight");
    m_settings->setValue("enabled", m_enabled);
    m_settings->setValue("brightnessLevel", m_brightnessLevel);
    m_settings->endGroup();
    m_settings->sync();
}

void LowLight::loadPreferences()
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos/high-contrast-mode.ini";
    m_settings = new QSettings(configPath, QSettings::IniFormat, this);
    
    m_settings->beginGroup("LowLight");
    m_enabled = m_settings->value("enabled", false).toBool();
    m_brightnessLevel = m_settings->value("brightnessLevel", 50).toInt();
    m_settings->endGroup();
}

QColor LowLight::adjustColorForLowLight(const QColor& color)
{
    // Adjust color for better low-light visibility
    // Increase contrast and adjust brightness based on brightness level
    
    int r = color.red();
    int g = color.green();
    int b = color.blue();
    
    // Adjust based on brightness level (lower brightness = more adjustment)
    int adjustment = (100 - m_brightnessLevel) / 2;
    
    // Increase contrast
    r = qBound(0, r + adjustment, 255);
    g = qBound(0, g + adjustment, 255);
    b = qBound(0, b + adjustment, 255);
    
    return QColor(r, g, b, color.alpha());
}


