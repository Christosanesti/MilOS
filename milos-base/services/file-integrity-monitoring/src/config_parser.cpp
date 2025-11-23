#include "config_parser.h"
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <sstream>

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
        std::cerr << "Failed to parse configuration file: " << configPath << std::endl;
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
        std::cerr << "YAML parsing error: " << e.what() << std::endl;
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

