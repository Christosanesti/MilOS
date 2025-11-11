#ifndef IDS_RULE_H
#define IDS_RULE_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>

/**
 * @brief IDS Rule
 * 
 * Represents a single IDS detection rule.
 */
struct IDSRule {
    std::string id;
    std::string name;
    std::string description;
    std::string severity;  // "low", "medium", "high", "critical"
    std::string protocol;
    std::string source_address;
    std::string destination_address;
    uint16_t source_port;
    uint16_t destination_port;
    std::string pattern;  // Pattern to match in packet payload
    std::vector<std::string> keywords;  // Keywords to search for
    bool enabled;
    uint32_t priority;  // Rule priority (higher = more important)
    std::map<std::string, std::string> metadata;  // Additional rule metadata
};

/**
 * @brief IDS Rule Manager
 * 
 * Manages IDS rules: loading, validation, and retrieval.
 */
class IDSRuleManager {
public:
    IDSRuleManager();
    ~IDSRuleManager();

    /**
     * @brief Load rules from YAML configuration
     * @param configPath Path to YAML configuration file
     * @return true if load successful, false otherwise
     */
    bool loadRules(const std::string& configPath);

    /**
     * @brief Add rule
     * @param rule Rule to add
     * @return true if add successful, false otherwise
     */
    bool addRule(const IDSRule& rule);

    /**
     * @brief Remove rule
     * @param ruleId Rule ID to remove
     * @return true if remove successful, false otherwise
     */
    bool removeRule(const std::string& ruleId);

    /**
     * @brief Update rule
     * @param ruleId Rule ID to update
     * @param rule Updated rule
     * @return true if update successful, false otherwise
     */
    bool updateRule(const std::string& ruleId, const IDSRule& rule);

    /**
     * @brief Get all rules
     * @return Vector of all rules
     */
    std::vector<IDSRule> getAllRules() const;

    /**
     * @brief Get enabled rules
     * @return Vector of enabled rules
     */
    std::vector<IDSRule> getEnabledRules() const;

    /**
     * @brief Get rule by ID
     * @param ruleId Rule ID
     * @return Rule if found, nullptr otherwise
     */
    const IDSRule* getRule(const std::string& ruleId) const;

    /**
     * @brief Validate rule
     * @param rule Rule to validate
     * @return true if valid, false otherwise
     */
    bool validateRule(const IDSRule& rule) const;

private:
    std::map<std::string, IDSRule> m_rules;

    /**
     * @brief Parse YAML rules
     */
    bool parseYamlRules(const std::string& configPath);
};

#endif // IDS_RULE_H

