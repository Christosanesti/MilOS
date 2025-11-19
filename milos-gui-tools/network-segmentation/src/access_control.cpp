#include "access_control.h"
#include "segment_config.h"
#include <QUuid>
#include <QDebug>
#include <algorithm>
#include <iostream>

AccessControl::AccessControl(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
{
}

AccessControl::~AccessControl() {
}

bool AccessControl::initialize() {
    if (m_initialized) {
        return true;
    }

    m_initialized = true;
    return true;
}

QString AccessControl::createPolicy(const AccessControlPolicy& policy) {
    if (!m_initialized) {
        return QString();
    }

    AccessControlPolicy newPolicy = policy;
    if (newPolicy.policyId.isEmpty()) {
        newPolicy.policyId = QUuid::createUuid().toString();
    }

    m_policies[newPolicy.policyId] = newPolicy;

    // Apply policy rules
    if (!applyPolicyRules(newPolicy)) {
        std::cerr << "Failed to apply policy rules" << std::endl;
        m_policies.remove(newPolicy.policyId);
        return QString();
    }

    // Update segment policies mapping
    if (!newPolicy.sourceSegment.isEmpty()) {
        if (!m_segmentPolicies[newPolicy.sourceSegment].contains(newPolicy.policyId)) {
            m_segmentPolicies[newPolicy.sourceSegment].append(newPolicy.policyId);
        }
    }
    if (!newPolicy.destinationSegment.isEmpty()) {
        if (!m_segmentPolicies[newPolicy.destinationSegment].contains(newPolicy.policyId)) {
            m_segmentPolicies[newPolicy.destinationSegment].append(newPolicy.policyId);
        }
    }

    emit policyCreated(newPolicy.policyId);
    return newPolicy.policyId;
}

bool AccessControl::updatePolicy(const QString& policyId, const AccessControlPolicy& policy) {
    if (!m_initialized || !m_policies.contains(policyId)) {
        return false;
    }

    AccessControlPolicy oldPolicy = m_policies[policyId];
    AccessControlPolicy updatedPolicy = policy;
    updatedPolicy.policyId = policyId;

    m_policies[policyId] = updatedPolicy;

    // Update segment policies mapping
    if (oldPolicy.sourceSegment != updatedPolicy.sourceSegment) {
        m_segmentPolicies[oldPolicy.sourceSegment].removeAll(policyId);
        if (!updatedPolicy.sourceSegment.isEmpty()) {
            if (!m_segmentPolicies[updatedPolicy.sourceSegment].contains(policyId)) {
                m_segmentPolicies[updatedPolicy.sourceSegment].append(policyId);
            }
        }
    }
    if (oldPolicy.destinationSegment != updatedPolicy.destinationSegment) {
        m_segmentPolicies[oldPolicy.destinationSegment].removeAll(policyId);
        if (!updatedPolicy.destinationSegment.isEmpty()) {
            if (!m_segmentPolicies[updatedPolicy.destinationSegment].contains(policyId)) {
                m_segmentPolicies[updatedPolicy.destinationSegment].append(policyId);
            }
        }
    }

    // Re-apply policy rules
    if (!applyPolicyRules(updatedPolicy)) {
        std::cerr << "Failed to apply updated policy rules" << std::endl;
        return false;
    }

    emit policyUpdated(policyId);
    return true;
}

bool AccessControl::deletePolicy(const QString& policyId) {
    if (!m_initialized || !m_policies.contains(policyId)) {
        return false;
    }

    AccessControlPolicy policy = m_policies[policyId];

    // Remove from segment policies mapping
    m_segmentPolicies[policy.sourceSegment].removeAll(policyId);
    m_segmentPolicies[policy.destinationSegment].removeAll(policyId);

    m_policies.remove(policyId);
    emit policyDeleted(policyId);
    return true;
}

bool AccessControl::evaluateAccess(const QString& sourceSegment, const QString& destinationSegment,
                                  const QString& protocol, const QString& port) const {
    if (!m_initialized) {
        return false;  // Deny by default if not initialized
    }

    // Find applicable policies
    QList<AccessControlPolicy> applicablePolicies;
    for (const AccessControlPolicy& policy : m_policies.values()) {
        if (!policy.enabled) {
            continue;
        }

        bool matchesSource = policy.sourceSegment.isEmpty() || policy.sourceSegment == sourceSegment;
        bool matchesDestination = policy.destinationSegment.isEmpty() || policy.destinationSegment == destinationSegment;

        if (matchesSource && matchesDestination) {
            applicablePolicies.append(policy);
        }
    }

    // Sort by priority (higher priority first)
    std::sort(applicablePolicies.begin(), applicablePolicies.end(),
              [](const AccessControlPolicy& a, const AccessControlPolicy& b) {
                  return a.priority > b.priority;
              });

    // Evaluate policies
    for (const AccessControlPolicy& policy : applicablePolicies) {
        // Check protocol
        if (!policy.allowedProtocols.isEmpty() && !policy.allowedProtocols.contains(protocol)) {
            continue;  // Protocol not allowed, skip this policy
        }

        // Check port
        if (!policy.blockedPorts.isEmpty() && policy.blockedPorts.contains(port)) {
            return false;  // Port blocked
        }

        if (!policy.allowedPorts.isEmpty() && !policy.allowedPorts.contains(port)) {
            continue;  // Port not in allowed list, skip this policy
        }

        // Policy allows access
        return true;
    }

    // No policy allows access, deny by default
    return false;
}

bool AccessControl::enforcePolicy(const QString& policyId) {
    if (!m_initialized || !m_policies.contains(policyId)) {
        return false;
    }

    AccessControlPolicy policy = m_policies[policyId];
    return applyPolicyRules(policy);
}

QStringList AccessControl::getPoliciesForSegment(const QString& segmentId) const {
    return m_segmentPolicies.value(segmentId, QStringList());
}

bool AccessControl::applyPolicyRules(const AccessControlPolicy& policy) {
    // Apply access control rules via firewall
    // This integrates with the firewall rule system
    
    if (!policy.enabled) {
        return true;  // Policy disabled, no rules to apply
    }

    // This would integrate with FirewallManager to apply rules
    // For now, we'll just log the action
    std::cout << "Applying access control policy: " << policy.name.toStdString() << std::endl;
    std::cout << "  Source: " << policy.sourceSegment.toStdString() << std::endl;
    std::cout << "  Destination: " << policy.destinationSegment.toStdString() << std::endl;
    std::cout << "  Protocols: " << policy.allowedProtocols.join(", ").toStdString() << std::endl;
    std::cout << "  Ports: " << policy.allowedPorts.join(", ").toStdString() << std::endl;

    return true;
}

