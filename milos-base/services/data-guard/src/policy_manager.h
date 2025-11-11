#ifndef POLICY_MANAGER_H
#define POLICY_MANAGER_H

#include <memory>
#include <string>
#include <vector>
#include <map>

class ConfigParser;

/**
 * @brief Policy rule structure
 */
struct PolicyRule {
    std::string rule_id;
    std::string source;      // CIDR notation or null
    std::string destination; // CIDR notation or null
    std::string protocol;    // TCP, UDP, ICMP, etc. or null
    int port;                // Port number or -1 for any
    bool encryption_required;
    std::string action;      // ALLOW, BLOCK, LOG
};

/**
 * @brief Network policy structure
 */
struct NetworkPolicy {
    std::string policy_id;
    std::string policy_name;
    std::string policy_type;
    bool enabled;
    int priority;
    std::vector<PolicyRule> rules;
    std::string created_at;
    std::string updated_at;
};

/**
 * @brief Policy Manager
 * 
 * Manages network transmission policies, including loading,
 * validation, and application of policies.
 */
class PolicyManager {
public:
    PolicyManager();
    ~PolicyManager();

    /**
     * @brief Initialize policy manager
     * @param configParser Configuration parser instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(ConfigParser* configParser);

    /**
     * @brief Load policies from configuration
     * @return true if load successful, false otherwise
     */
    bool loadPolicies();

    /**
     * @brief Reload policies from configuration
     * @param configParser Configuration parser instance
     * @return true if reload successful, false otherwise
     */
    bool reloadPolicies(ConfigParser* configParser);

    /**
     * @brief Get all policies
     * @return Vector of policies
     */
    std::vector<NetworkPolicy> getPolicies() const;

    /**
     * @brief Get policy by ID
     * @param policyId Policy ID
     * @return Policy if found, nullptr otherwise
     */
    const NetworkPolicy* getPolicy(const std::string& policyId) const;

    /**
     * @brief Validate policy
     * @param policy Policy to validate
     * @return true if valid, false otherwise
     */
    bool validatePolicy(const NetworkPolicy& policy) const;

    /**
     * @brief Apply policy to network enforcement
     * @param policyId Policy ID
     * @return true if applied successfully, false otherwise
     */
    bool applyPolicy(const std::string& policyId);

private:
    ConfigParser* m_configParser;
    std::vector<NetworkPolicy> m_policies;
    std::map<std::string, size_t> m_policyIndex;  // policy_id -> index

    /**
     * @brief Sort policies by priority
     */
    void sortPoliciesByPriority();
};

#endif // POLICY_MANAGER_H

