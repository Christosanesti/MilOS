#ifndef ACCESS_CONTROL_H
#define ACCESS_CONTROL_H

#include <string>
#include <vector>
#include <map>
#include <memory>

/**
 * @brief Access Permission
 */
enum class Permission {
    Read,
    Write,
    Delete,
    Rotate
};

/**
 * @brief Access Policy
 */
struct AccessPolicy {
    std::string secret_id_pattern;  // Wildcard pattern
    std::vector<std::string> roles;
    std::vector<Permission> permissions;
};

/**
 * @brief Access Control
 * 
 * Manages role-based access control for secrets.
 */
class AccessControl {
public:
    AccessControl();
    ~AccessControl();

    /**
     * @brief Initialize access control
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Check if user has permission
     * @param userId User ID
     * @param secretId Secret ID
     * @param permission Permission to check
     * @return true if has permission, false otherwise
     */
    bool hasPermission(const std::string& userId, const std::string& secretId, Permission permission) const;

    /**
     * @brief Add access policy
     * @param policy Access policy
     */
    void addPolicy(const AccessPolicy& policy);

    /**
     * @brief Remove access policy
     * @param secretIdPattern Secret ID pattern
     */
    void removePolicy(const std::string& secretIdPattern);

    /**
     * @brief Get user roles
     * @param userId User ID
     * @return List of roles
     */
    std::vector<std::string> getUserRoles(const std::string& userId) const;

    /**
     * @brief Set user roles
     * @param userId User ID
     * @param roles List of roles
     */
    void setUserRoles(const std::string& userId, const std::vector<std::string>& roles);

    /**
     * @brief Log access attempt
     * @param userId User ID
     * @param secretId Secret ID
     * @param permission Permission attempted
     * @param granted Whether access was granted
     */
    void logAccess(const std::string& userId, const std::string& secretId, Permission permission, bool granted) const;

private:
    bool m_initialized;
    std::map<std::string, AccessPolicy> m_policies;
    std::map<std::string, std::vector<std::string>> m_userRoles;

    /**
     * @brief Check if pattern matches
     * @param pattern Wildcard pattern
     * @param text Text to match
     * @return true if matches, false otherwise
     */
    bool patternMatches(const std::string& pattern, const std::string& text) const;
};

#endif // ACCESS_CONTROL_H

