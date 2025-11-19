#include "config_parser.h"
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <fstream>
#include <QFileInfo>
#include <QDebug>

ConfigParser::ConfigParser() {
}

bool ConfigParser::load(const QString& filePath) {
    m_configFilePath = filePath;
    
    try {
        YAML::Node config = YAML::LoadFile(filePath.toStdString());
        
        if (config["devices"]) {
            YAML::Node devices = config["devices"];
            for (YAML::const_iterator it = devices.begin(); it != devices.end(); ++it) {
                QString deviceId = QString::fromStdString(it->first.as<std::string>());
                QVariantMap deviceConfig;
                
                if (it->second["vendor"]) {
                    deviceConfig["vendor"] = QString::fromStdString(it->second["vendor"].as<std::string>());
                }
                if (it->second["model"]) {
                    deviceConfig["model"] = QString::fromStdString(it->second["model"].as<std::string>());
                }
                if (it->second["settings"]) {
                    QVariantMap settings;
                    YAML::Node settingsNode = it->second["settings"];
                    for (YAML::const_iterator sit = settingsNode.begin(); sit != settingsNode.end(); ++sit) {
                        QString key = QString::fromStdString(sit->first.as<std::string>());
                        QString value = QString::fromStdString(sit->second.as<std::string>());
                        settings[key] = value;
                    }
                    deviceConfig["settings"] = settings;
                }
                
                m_deviceConfigs[deviceId] = deviceConfig;
            }
        }
        
        return true;
    } catch (const YAML::Exception& e) {
        qWarning() << "YAML parsing error:" << e.what();
        return false;
    } catch (...) {
        // File might not exist, use defaults
        return false;
    }
}

QVariantMap ConfigParser::getDeviceConfig(const QString& deviceId) const {
    return m_deviceConfigs.value(deviceId, QVariantMap());
}

QMap<QString, QVariantMap> ConfigParser::getAllDeviceConfigs() const {
    return m_deviceConfigs;
}

void ConfigParser::setDeviceConfig(const QString& deviceId, const QVariantMap& config) {
    m_deviceConfigs[deviceId] = config;
}

bool ConfigParser::save(const QString& filePath) const {
    try {
        YAML::Node config;
        YAML::Node devices;
        
        for (auto it = m_deviceConfigs.begin(); it != m_deviceConfigs.end(); ++it) {
            YAML::Node deviceNode;
            QVariantMap deviceConfig = it.value();
            
            if (deviceConfig.contains("vendor")) {
                deviceNode["vendor"] = deviceConfig["vendor"].toString().toStdString();
            }
            if (deviceConfig.contains("model")) {
                deviceNode["model"] = deviceConfig["model"].toString().toStdString();
            }
            if (deviceConfig.contains("settings")) {
                YAML::Node settingsNode;
                QVariantMap settings = deviceConfig["settings"].toMap();
                for (auto sit = settings.begin(); sit != settings.end(); ++sit) {
                    settingsNode[sit.key().toStdString()] = sit.value().toString().toStdString();
                }
                deviceNode["settings"] = settingsNode;
            }
            
            devices[it.key().toStdString()] = deviceNode;
        }
        
        config["devices"] = devices;
        
        std::ofstream file(filePath.toStdString());
        file << config;
        file.close();
        
        return true;
    } catch (const std::exception& e) {
        qWarning() << "Failed to save configuration:" << e.what();
        return false;
    }
}


