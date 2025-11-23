#include "config_parser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

ConfigParser::ConfigParser()
    : m_loaded(false)
{
}

ConfigParser::~ConfigParser() {
}

bool ConfigParser::loadConfig(const std::string& configPath) {
    if (m_loaded) {
        return true;
    }

    // Try to parse YAML (simplified parser)
    if (!parseYAML(configPath)) {
        // Use defaults if config file not found
        std::cerr << "Warning: Could not load config file, using defaults" << std::endl;
    }

    m_loaded = true;
    return true;
}

bool ConfigParser::parseYAML(const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    std::string currentKey;
    int indentLevel = 0;

    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Count indentation
        size_t indent = 0;
        while (indent < line.length() && line[indent] == ' ') {
            indent++;
        }

        // Find key-value pair
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) {
            continue;
        }

        std::string key = line.substr(indent, colonPos - indent);
        std::string value = line.substr(colonPos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        // Remove quotes
        if (value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.length() - 2);
        }

        // Build full key path
        std::string fullKey = key;
        if (!currentKey.empty() && indent > indentLevel) {
            fullKey = currentKey + "." + key;
        }

        m_config[fullKey] = value;
        currentKey = fullKey;
        indentLevel = indent;
    }

    return true;
}

std::string ConfigParser::getConfig(const std::string& key, const std::string& defaultValue) const {
    auto it = m_config.find(key);
    if (it != m_config.end()) {
        return it->second;
    }
    return defaultValue;
}

int ConfigParser::getConfigInt(const std::string& key, int defaultValue) const {
    std::string value = getConfig(key);
    if (value.empty()) {
        return defaultValue;
    }
    return std::stoi(value);
}

bool ConfigParser::getConfigBool(const std::string& key, bool defaultValue) const {
    std::string value = getConfig(key);
    if (value.empty()) {
        return defaultValue;
    }
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    return value == "true" || value == "1" || value == "yes";
}

