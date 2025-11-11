#include "ids_rule.h"
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <fstream>
#include <algorithm>

IDSRuleManager::IDSRuleManager() {
}

IDSRuleManager::~IDSRuleManager() {
}

bool IDSRuleManager::loadRules(const std::string& configPath) {
    return parseYamlRules(configPath);
}

bool IDSRuleManager::parseYamlRules(const std::string& configPath) {
    try {
        YAML::Node config = YAML::LoadFile(configPath);
        
        if (!config["ids_rules"] || !config["ids_rules"].IsSequence()) {
            std::cerr << "Invalid IDS rules configuration format" << std::endl;
            return false;
        }

        for (const auto& ruleNode : config["ids_rules"]) {
            IDSRule rule;
            rule.id = ruleNode["id"].as<std::string>();
            rule.name = ruleNode["name"].as<std::string>();
            rule.description = ruleNode["description"].as<std::string>("");
            rule.severity = ruleNode["severity"].as<std::string>("medium");
            rule.protocol = ruleNode["protocol"].as<std::string>("");
            rule.source_address = ruleNode["source_address"].as<std::string>("");
            rule.destination_address = ruleNode["destination_address"].as<std::string>("");
            rule.source_port = ruleNode["source_port"].as<uint16_t>(0);
            rule.destination_port = ruleNode["destination_port"].as<uint16_t>(0);
            rule.pattern = ruleNode["pattern"].as<std::string>("");
            rule.enabled = ruleNode["enabled"].as<bool>(true);
            rule.priority = ruleNode["priority"].as<uint32_t>(0);

            if (ruleNode["keywords"] && ruleNode["keywords"].IsSequence()) {
                for (const auto& keyword : ruleNode["keywords"]) {
                    rule.keywords.push_back(keyword.as<std::string>());
                }
            }

            if (validateRule(rule)) {
                m_rules[rule.id] = rule;
            }
        }

        return true;
    } catch (const YAML::Exception& e) {
        std::cerr << "Error parsing IDS rules: " << e.what() << std::endl;
        return false;
    }
}

bool IDSRuleManager::addRule(const IDSRule& rule) {
    if (!validateRule(rule)) {
        return false;
    }
    m_rules[rule.id] = rule;
    return true;
}

bool IDSRuleManager::removeRule(const std::string& ruleId) {
    auto it = m_rules.find(ruleId);
    if (it != m_rules.end()) {
        m_rules.erase(it);
        return true;
    }
    return false;
}

bool IDSRuleManager::updateRule(const std::string& ruleId, const IDSRule& rule) {
    if (m_rules.find(ruleId) == m_rules.end()) {
        return false;
    }
    if (!validateRule(rule)) {
        return false;
    }
    m_rules[ruleId] = rule;
    return true;
}

std::vector<IDSRule> IDSRuleManager::getAllRules() const {
    std::vector<IDSRule> rules;
    for (const auto& pair : m_rules) {
        rules.push_back(pair.second);
    }
    return rules;
}

std::vector<IDSRule> IDSRuleManager::getEnabledRules() const {
    std::vector<IDSRule> rules;
    for (const auto& pair : m_rules) {
        if (pair.second.enabled) {
            rules.push_back(pair.second);
        }
    }
    // Sort by priority (higher priority first)
    std::sort(rules.begin(), rules.end(), [](const IDSRule& a, const IDSRule& b) {
        return a.priority > b.priority;
    });
    return rules;
}

const IDSRule* IDSRuleManager::getRule(const std::string& ruleId) const {
    auto it = m_rules.find(ruleId);
    if (it != m_rules.end()) {
        return &it->second;
    }
    return nullptr;
}

bool IDSRuleManager::validateRule(const IDSRule& rule) const {
    if (rule.id.empty()) {
        return false;
    }
    if (rule.name.empty()) {
        return false;
    }
    if (rule.severity != "low" && rule.severity != "medium" && 
        rule.severity != "high" && rule.severity != "critical") {
        return false;
    }
    return true;
}

