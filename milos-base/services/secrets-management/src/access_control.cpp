#include "access_control.h"
#include <milos/logging/logger.h>
#include <algorithm>

AccessControl::AccessControl()
    : m_initialized(false)
{
}

AccessControl::~AccessControl() {
}

bool AccessControl::initialize() {
    if (m_initialized) {
        return true;
    }

    // Default policy: admin has all permissions
    AccessPolicy adminPolicy;
    adminPolicy.secret_id_pattern = "*";
    adminPolicy.roles = {"admin"};
    adminPolicy.permissions = {Permission::Read, Permission::Write, Permission::Delete, Permission::Rotate};
    m_policies["*"] = adminPolicy;

    m_initialized = true;
    return true;
}

bool AccessControl::hasPermission(const std::string& userId, const std::string& secretId, Permission permission) const {
    if (!m_initialized) {
        return false;
    }

    // Get user roles
    std::vector<std::string> roles = getUserRoles(userId);
    if (roles.empty()) {
        return false;
    }

    // Check policies
    for (const auto& [pattern, policy] : m_policies) {
        if (patternMatches(policy.secret_id_pattern, secretId)) {
            // Check if user has required role
            bool hasRole = false;
            for (const auto& role : roles) {
                if (std::find(policy.roles.begin(), policy.roles.end(), role) != policy.roles.end()) {
                    hasRole = true;
                    break;
                }
            }

            if (hasRole) {
                // Check if role has required permission
                return std::find(policy.permissions.begin(), policy.permissions.end(), permission) != policy.permissions.end();
            }
        }
    }

    return false;
}

void AccessControl::addPolicy(const AccessPolicy& policy) {
    m_policies[policy.secret_id_pattern] = policy;
}

void AccessControl::removePolicy(const std::string& secretIdPattern) {
    m_policies.erase(secretIdPattern);
}

std::vector<std::string> AccessControl::getUserRoles(const std::string& userId) const {
    auto it = m_userRoles.find(userId);
    if (it != m_userRoles.end()) {
        return it->second;
    }

    // Default: no roles
    return std::vector<std::string>();
}

void AccessControl::setUserRoles(const std::string& userId, const std::vector<std::string>& roles) {
    m_userRoles[userId] = roles;
}

void AccessControl::logAccess(const std::string& userId, const std::string& secretId, Permission permission, bool granted) const {
    // Log access attempt (will be sent to audit service)
    std::string permStr;
    switch (permission) {
        case Permission::Read: permStr = "read"; break;
        case Permission::Write: permStr = "write"; break;
        case Permission::Delete: permStr = "delete"; break;
        case Permission::Rotate: permStr = "rotate"; break;
    }

    LOG_INFO(QString("Access %1: user=%2, secret=%3, permission=%4")
             .arg(granted ? "granted" : "denied")
             .arg(QString::fromStdString(userId))
             .arg(QString::fromStdString(secretId))
             .arg(QString::fromStdString(permStr)));
}

bool AccessControl::patternMatches(const std::string& pattern, const std::string& text) const {
    // Simple wildcard matching (* matches any string)
    if (pattern == "*") {
        return true;
    }

    if (pattern == text) {
        return true;
    }

    // Check if pattern is a prefix
    if (pattern.back() == '*' && text.substr(0, pattern.length() - 1) == pattern.substr(0, pattern.length() - 1)) {
        return true;
    }

    return false;
}

