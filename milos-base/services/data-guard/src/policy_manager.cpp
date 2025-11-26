#include "policy_manager.h"
#include "config_parser.h"
#include <algorithm>
#include <iostream>

PolicyManager::PolicyManager()
    : m_configParser(nullptr)
{
}

PolicyManager::~PolicyManager() {
}

bool PolicyManager::initialize(ConfigParser* configParser) {
    m_configParser = configParser;
    return loadPolicies();
}

bool PolicyManager::loadPolicies() {
    if (!m_configParser || !m_configParser->isLoaded()) {
        std::cerr << "Config parser not set or not loaded" << std::endl;
        return false;
    }

    m_policies.clear();
    m_policyIndex.clear();

    // Load policies from YAML configuration
    // Policies are stored in config under "policies" key as a sequence
    try {
        // Use yaml-cpp directly to parse policies array
        // Note: This requires access to the YAML node, which we'll need to expose from ConfigParser
        // For now, we'll use a simplified approach
        
        // TODO: Expose YAML node access from ConfigParser for full policy parsing
        // For MVP, we'll create default policies based on configuration template
        
        // Create default encryption required policy
        NetworkPolicy defaultPolicy;
        defaultPolicy.policy_id = "default-encryption-required";
        defaultPolicy.policy_name = "Default Encryption Required";
        defaultPolicy.policy_type = "ENCRYPTION_REQUIRED";
        defaultPolicy.enabled = m_configParser->getBool("service.enabled", true);
        defaultPolicy.priority = 100;
        
        PolicyRule defaultRule;
        defaultRule.rule_id = "default-rule";
        defaultRule.source = "";
        defaultRule.destination = "";
        defaultRule.protocol = "";
        defaultRule.port = -1;
        defaultRule.encryption_required = true;
        defaultRule.action = "BLOCK";
        
        defaultPolicy.rules.push_back(defaultRule);
        defaultPolicy.created_at = "";
        defaultPolicy.updated_at = "";
        
        m_policies.push_back(defaultPolicy);
        
        // Create localhost exception policy
        NetworkPolicy localhostPolicy;
        localhostPolicy.policy_id = "localhost-exception";
        localhostPolicy.policy_name = "Localhost Exception";
        localhostPolicy.policy_type = "PROTOCOL_ALLOWLIST";
        localhostPolicy.enabled = true;
        localhostPolicy.priority = 200;
        
        PolicyRule localhostRule;
        localhostRule.rule_id = "localhost-rule";
        localhostRule.source = "127.0.0.0/8";
        localhostRule.destination = "127.0.0.0/8";
        localhostRule.protocol = "";
        localhostRule.port = -1;
        localhostRule.encryption_required = false;
        localhostRule.action = "ALLOW";
        
        localhostPolicy.rules.push_back(localhostRule);
        localhostPolicy.created_at = "";
        localhostPolicy.updated_at = "";
        
        m_policies.push_back(localhostPolicy);
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading policies: " << e.what() << std::endl;
        return false;
    }

    sortPoliciesByPriority();
    return true;
}

bool PolicyManager::reloadPolicies(ConfigParser* configParser) {
    m_configParser = configParser;
    return loadPolicies();
}

std::vector<NetworkPolicy> PolicyManager::getPolicies() const {
    return m_policies;
}

const NetworkPolicy* PolicyManager::getPolicy(const std::string& policyId) const {
    auto it = m_policyIndex.find(policyId);
    if (it != m_policyIndex.end()) {
        return &m_policies[it->second];
    }
    return nullptr;
}

bool PolicyManager::validatePolicy(const NetworkPolicy& policy) const {
    // Validate policy ID
    if (policy.policy_id.empty()) {
        return false;
    }

    // Validate policy name
    if (policy.policy_name.empty()) {
        return false;
    }

    // Validate policy type
    if (policy.policy_type != "ENCRYPTION_REQUIRED" &&
        policy.policy_type != "PROTOCOL_ALLOWLIST" &&
        policy.policy_type != "PROTOCOL_BLOCKLIST") {
        return false;
    }

    // Validate rules
    if (policy.rules.empty()) {
        return false;
    }

    // Validate each rule
    for (const auto& rule : policy.rules) {
        if (rule.rule_id.empty()) {
            return false;
        }
        if (rule.action != "ALLOW" && rule.action != "BLOCK" && rule.action != "LOG") {
            return false;
        }
    }

    return true;
}

bool PolicyManager::applyPolicy(const std::string& policyId) {
    const NetworkPolicy* policy = getPolicy(policyId);
    if (!policy) {
        std::cerr << "Policy not found: " << policyId << std::endl;
        return false;
    }

    if (!validatePolicy(*policy)) {
        std::cerr << "Policy validation failed: " << policyId << std::endl;
        return false;
    }

    // Policy is automatically applied through network enforcement's policy manager reference
    // Network enforcement reads policies from policy manager when inspecting packets
    // No explicit application needed - policies are evaluated in priority order

    return true;
}

bool PolicyManager::addOrUpdatePolicy(const NetworkPolicy& policy) {
    if (!validatePolicy(policy)) {
        std::cerr << "Policy validation failed: " << policy.policy_id << std::endl;
        return false;
    }

    // Check if policy already exists
    auto it = m_policyIndex.find(policy.policy_id);
    if (it != m_policyIndex.end()) {
        // Update existing policy
        m_policies[it->second] = policy;
    } else {
        // Add new policy
        m_policies.push_back(policy);
    }

    sortPoliciesByPriority();
    return true;
}

void PolicyManager::sortPoliciesByPriority() {
    std::sort(m_policies.begin(), m_policies.end(),
        [](const NetworkPolicy& a, const NetworkPolicy& b) {
            return a.priority > b.priority;  // Higher priority first
        });

    // Rebuild index
    m_policyIndex.clear();
    for (size_t i = 0; i < m_policies.size(); ++i) {
        m_policyIndex[m_policies[i].policy_id] = i;
    }
}

