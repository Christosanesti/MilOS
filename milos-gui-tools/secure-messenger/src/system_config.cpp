#include "system_config.h"
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

SystemConfig::SystemConfig(QObject* parent)
    : QObject(parent)
    , m_settings(nullptr)
{
}

SystemConfig::~SystemConfig() {
    if (m_settings) {
        delete m_settings;
    }
}

bool SystemConfig::initialize() {
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(configPath);
    
    m_settings = new QSettings(configPath + "/config.ini", QSettings::IniFormat, this);
    
    // Load existing config
    QStringList keys = m_settings->allKeys();
    for (const QString& key : keys) {
        m_config[key] = m_settings->value(key);
    }
    
    return true;
}

QVariant SystemConfig::getConfig(const QString& key, const QVariant& defaultValue) const {
    return m_config.value(key, defaultValue);
}

bool SystemConfig::setConfig(const QString& key, const QVariant& value) {
    m_config[key] = value;
    
    if (m_settings) {
        m_settings->setValue(key, value);
        m_settings->sync();
    }
    
    emit configChanged(key, value);
    
    return true;
}

QVariantMap SystemConfig::getAllConfig() const {
    return m_config;
}

bool SystemConfig::loadConfig(const QString& filePath) {
    QSettings fileSettings(filePath, QSettings::IniFormat);
    
    QStringList keys = fileSettings.allKeys();
    for (const QString& key : keys) {
        m_config[key] = fileSettings.value(key);
    }
    
    return true;
}

bool SystemConfig::saveConfig(const QString& filePath) {
    QSettings fileSettings(filePath, QSettings::IniFormat);
    
    for (auto it = m_config.begin(); it != m_config.end(); ++it) {
        fileSettings.setValue(it.key(), it.value());
    }
    
    fileSettings.sync();
    
    return true;
}

