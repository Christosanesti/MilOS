#include "config_parser.h"
#include <milos/logging/logger.h>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>
#include <vector>

ConfigParser::ConfigParser()
    : m_loaded(false)
    , m_yamlRoot(nullptr)
{
}

ConfigParser::~ConfigParser() {
    if (m_yamlRoot) {
        delete static_cast<YAML::Node*>(m_yamlRoot);
        m_yamlRoot = nullptr;
    }
}

bool ConfigParser::load(const std::string& configPath) {
    m_configPath = configPath;
    
    if (!parseYaml(configPath)) {
        LOG_ERROR(QString("Failed to parse configuration file: %1").arg(QString::fromStdString(configPath)));
        return false;
    }

    m_loaded = true;
    return true;
}

bool ConfigParser::parseYaml(const std::string& configPath) {
    try {
        YAML::Node* config = new YAML::Node(YAML::LoadFile(configPath));
        if (m_yamlRoot) {
            delete static_cast<YAML::Node*>(m_yamlRoot);
        }
        m_yamlRoot = config;
        return true;
    } catch (const YAML::Exception& e) {
        LOG_ERROR(QString("YAML parsing error: %1").arg(e.what()));
        return false;
    }
}

std::string ConfigParser::getString(const std::string& key) const {
    if (!m_loaded || !m_yamlRoot) {
        return "";
    }

    try {
        YAML::Node* config = static_cast<YAML::Node*>(m_yamlRoot);
        YAML::Node node = *config;
        
        // Split key by dots
        std::istringstream iss(key);
        std::string token;
        while (std::getline(iss, token, '.')) {
            if (node.IsMap() && node[token]) {
                node = node[token];
            } else {
                return "";
            }
        }
        
        if (node.IsScalar()) {
            return node.as<std::string>();
        }
    } catch (const YAML::Exception& e) {
        LOG_ERROR(QString("Error getting string value for key %1: %2").arg(QString::fromStdString(key)).arg(e.what()));
    }
    
    return "";
}

bool ConfigParser::getBool(const std::string& key, bool defaultValue) const {
    if (!m_loaded || !m_yamlRoot) {
        return defaultValue;
    }

    try {
        YAML::Node* config = static_cast<YAML::Node*>(m_yamlRoot);
        YAML::Node node = *config;
        
        // Split key by dots
        std::istringstream iss(key);
        std::string token;
        while (std::getline(iss, token, '.')) {
            if (node.IsMap() && node[token]) {
                node = node[token];
            } else {
                return defaultValue;
            }
        }
        
        if (node.IsScalar()) {
            return node.as<bool>();
        }
    } catch (const YAML::Exception& e) {
        LOG_ERROR(QString("Error getting bool value for key %1: %2").arg(QString::fromStdString(key)).arg(e.what()));
    }
    
    return defaultValue;
}

int ConfigParser::getInt(const std::string& key, int defaultValue) const {
    if (!m_loaded || !m_yamlRoot) {
        return defaultValue;
    }

    try {
        YAML::Node* config = static_cast<YAML::Node*>(m_yamlRoot);
        YAML::Node node = *config;
        
        // Split key by dots
        std::istringstream iss(key);
        std::string token;
        while (std::getline(iss, token, '.')) {
            if (node.IsMap() && node[token]) {
                node = node[token];
            } else {
                return defaultValue;
            }
        }
        
        if (node.IsScalar()) {
            return node.as<int>();
        }
    } catch (const YAML::Exception& e) {
        LOG_ERROR(QString("Error getting int value for key %1: %2").arg(QString::fromStdString(key)).arg(e.what()));
    }
    
    return defaultValue;
}

std::vector<std::string> ConfigParser::getStringArray(const std::string& key) const {
    std::vector<std::string> result;
    
    if (!m_loaded || !m_yamlRoot) {
        return result;
    }

    try {
        YAML::Node* config = static_cast<YAML::Node*>(m_yamlRoot);
        YAML::Node node = *config;
        
        // Split key by dots
        std::istringstream iss(key);
        std::string token;
        while (std::getline(iss, token, '.')) {
            if (node.IsMap() && node[token]) {
                node = node[token];
            } else {
                return result;
            }
        }
        
        if (node.IsSequence()) {
            for (const auto& item : node) {
                if (item.IsScalar()) {
                    result.push_back(item.as<std::string>());
                }
            }
        }
    } catch (const YAML::Exception& e) {
        LOG_ERROR(QString("Error getting string array for key %1: %2").arg(QString::fromStdString(key)).arg(e.what()));
    }
    
    return result;
}

