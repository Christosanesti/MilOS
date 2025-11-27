#include "config_parser.h"
#include <milos/logging/logger.h>
#include <yaml-cpp/yaml.h>
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

bool ConfigParser::loadConfig(const std::string& configPath) {
    m_configPath = configPath;
    
    if (!parseYaml(configPath)) {
        LOG_ERROR(std::string("Failed to parse configuration file: ") + configPath);
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
        LOG_ERROR(std::string("YAML parsing error: ") + e.what());
        return false;
    } catch (...) {
        // File might not exist, use defaults
        return true;
    }
}

std::string ConfigParser::getString(const std::string& key) const {
    if (!m_loaded || !m_yamlRoot) {
        return "";
    }

    try {
        YAML::Node* config = static_cast<YAML::Node*>(m_yamlRoot);
        YAML::Node node = *config;
        
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
        // Ignore
    }
    
    return "";
}

bool ConfigParser::getBool(const std::string& key, bool defaultValue) const {
    std::string value = getString(key);
    if (value.empty()) {
        return defaultValue;
    }
    return (value == "true" || value == "1" || value == "yes");
}

int ConfigParser::getInt(const std::string& key, int defaultValue) const {
    std::string value = getString(key);
    if (value.empty()) {
        return defaultValue;
    }
    try {
        return std::stoi(value);
    } catch (...) {
        return defaultValue;
    }
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
        // Ignore parsing errors
    }
    
    return result;
}

