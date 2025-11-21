#include "config_manager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>

ConfigManager::ConfigManager(QObject* parent)
    : QObject(parent)
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(configDir);
    m_configFilePath = configDir + "/enterprise-config.json";
}

ConfigManager::~ConfigManager() {
}

bool ConfigManager::initialize() {
    return loadConfig();
}

QVariant ConfigManager::getConfig(const QString& key, const QVariant& defaultValue) const {
    return m_configs.value(key, defaultValue);
}

bool ConfigManager::setConfig(const QString& key, const QVariant& value) {
    QVariant oldValue = m_configs.value(key);
    
    if (oldValue != value) {
        m_configs[key] = value;
        
        if (saveConfig()) {
            emit configChanged(key, value);
            return true;
        }
    }
    
    return true;
}

QVariantMap ConfigManager::getAllConfigs() const {
    QVariantMap result;
    for (auto it = m_configs.begin(); it != m_configs.end(); ++it) {
        result[it.key()] = it.value();
    }
    return result;
}

bool ConfigManager::exportConfig(const QString& filePath) const {
    QJsonObject json;
    for (auto it = m_configs.begin(); it != m_configs.end(); ++it) {
        json[it.key()] = QJsonValue::fromVariant(it.value());
    }
    
    QJsonDocument doc(json);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    return true;
}

bool ConfigManager::importConfig(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        return false;
    }
    
    QJsonObject json = doc.object();
    for (auto it = json.begin(); it != json.end(); ++it) {
        m_configs[it.key()] = it.value().toVariant();
    }
    
    return saveConfig();
}

bool ConfigManager::loadConfig() {
    QFile file(m_configFilePath);
    if (!file.exists()) {
        return true;  // No config file yet, use defaults
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        return false;
    }
    
    QJsonObject json = doc.object();
    for (auto it = json.begin(); it != json.end(); ++it) {
        m_configs[it.key()] = it.value().toVariant();
    }
    
    return true;
}

bool ConfigManager::saveConfig() const {
    QJsonObject json;
    for (auto it = m_configs.begin(); it != m_configs.end(); ++it) {
        json[it.key()] = QJsonValue::fromVariant(it.value());
    }
    
    QJsonDocument doc(json);
    QFile file(m_configFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    return true;
}

