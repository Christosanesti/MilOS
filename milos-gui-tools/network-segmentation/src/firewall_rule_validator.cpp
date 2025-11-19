#include "firewall_rule_validator.h"
#include "firewall_rule_generator.h"
#include <QRegularExpression>
#include <QDebug>
#include <iostream>

FirewallRuleValidator::FirewallRuleValidator(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
{
}

FirewallRuleValidator::~FirewallRuleValidator() {
}

bool FirewallRuleValidator::initialize() {
    if (m_initialized) {
        return true;
    }

    m_initialized = true;
    return true;
}

RuleValidationResult FirewallRuleValidator::validateRule(const FirewallRule& rule) {
    RuleValidationResult result;
    result.isValid = true;

    // Validate rule syntax
    if (!validateRuleSyntax(rule)) {
        result.isValid = false;
        result.errors.append("Invalid rule syntax");
    }

    // Validate network addresses
    if (!rule.source.isEmpty() && !validateNetworkAddress(rule.source)) {
        result.isValid = false;
        result.errors.append(QString("Invalid source address: %1").arg(rule.source));
    }

    if (!rule.destination.isEmpty() && !validateNetworkAddress(rule.destination)) {
        result.isValid = false;
        result.errors.append(QString("Invalid destination address: %1").arg(rule.destination));
    }

    // Validate ports
    if (!validatePorts(rule.sourcePorts)) {
        result.isValid = false;
        result.errors.append("Invalid source ports");
    }

    if (!validatePorts(rule.destinationPorts)) {
        result.isValid = false;
        result.errors.append("Invalid destination ports");
    }

    // Validate action
    QStringList validActions = {"ACCEPT", "DROP", "REJECT"};
    if (!validActions.contains(rule.action)) {
        result.isValid = false;
        result.errors.append(QString("Invalid action: %1").arg(rule.action));
    }

    emit validationCompleted(result);
    return result;
}

RuleValidationResult FirewallRuleValidator::validateRules(const QList<FirewallRule>& rules) {
    RuleValidationResult result;
    result.isValid = true;

    // Validate each rule
    for (const FirewallRule& rule : rules) {
        RuleValidationResult ruleResult = validateRule(rule);
        if (!ruleResult.isValid) {
            result.isValid = false;
            result.errors.append(QString("Rule %1: %2").arg(rule.ruleId, ruleResult.errors.join(", ")));
        }
    }

    // Check for conflicts
    QList<QPair<QString, QString>> conflicts = detectConflicts(rules);
    if (!conflicts.isEmpty()) {
        result.isValid = false;
        for (const auto& conflict : conflicts) {
            result.errors.append(QString("Rule conflict: %1 <-> %2").arg(conflict.first, conflict.second));
            result.conflictingRules.append(conflict.first);
            result.conflictingRules.append(conflict.second);
        }
    }

    emit validationCompleted(result);
    return result;
}

QList<QPair<QString, QString>> FirewallRuleValidator::detectConflicts(const QList<FirewallRule>& rules) {
    QList<QPair<QString, QString>> conflicts;

    for (int i = 0; i < rules.size(); i++) {
        const FirewallRule& rule1 = rules[i];
        for (int j = i + 1; j < rules.size(); j++) {
            const FirewallRule& rule2 = rules[j];
            if (rulesConflict(rule1, rule2)) {
                conflicts.append(qMakePair(rule1.ruleId, rule2.ruleId));
            }
        }
    }

    return conflicts;
}

bool FirewallRuleValidator::validateRuleSyntax(const FirewallRule& rule) {
    // Basic syntax validation
    if (rule.ruleId.isEmpty() || rule.name.isEmpty()) {
        return false;
    }

    if (rule.source.isEmpty() && rule.destination.isEmpty()) {
        return false;  // At least one address must be specified
    }

    return true;
}

bool FirewallRuleValidator::validateNetworkAddress(const QString& address) {
    // Validate CIDR notation (e.g., 192.168.1.0/24)
    QRegularExpression cidrRegex("^([0-9]{1,3}\\.){3}[0-9]{1,3}/[0-9]{1,2}$");
    QRegularExpression ipRegex("^([0-9]{1,3}\\.){3}[0-9]{1,3}$");
    
    return cidrRegex.match(address).hasMatch() || ipRegex.match(address).hasMatch();
}

bool FirewallRuleValidator::validatePorts(const QStringList& ports) {
    QRegularExpression portRegex("^[0-9]{1,5}(-[0-9]{1,5})?$");
    
    for (const QString& port : ports) {
        if (!portRegex.match(port).hasMatch()) {
            return false;
        }
    }
    
    return true;
}

bool FirewallRuleValidator::rulesConflict(const FirewallRule& rule1, const FirewallRule& rule2) {
    // Check if rules have conflicting actions for same traffic
    if (rule1.action == rule2.action) {
        return false;  // Same action, no conflict
    }

    // Check if rules match same traffic
    bool sameSource = (rule1.source == rule2.source) || rule1.source.isEmpty() || rule2.source.isEmpty();
    bool sameDestination = (rule1.destination == rule2.destination) || rule1.destination.isEmpty() || rule2.destination.isEmpty();
    bool sameProtocol = (rule1.protocol == rule2.protocol) || rule1.protocol.isEmpty() || rule2.protocol.isEmpty();

    if (sameSource && sameDestination && sameProtocol) {
        // Check port overlap
        bool portOverlap = false;
        if (rule1.destinationPorts.isEmpty() || rule2.destinationPorts.isEmpty()) {
            portOverlap = true;
        } else {
            for (const QString& port1 : rule1.destinationPorts) {
                for (const QString& port2 : rule2.destinationPorts) {
                    if (port1 == port2) {
                        portOverlap = true;
                        break;
                    }
                }
                if (portOverlap) break;
            }
        }

        if (portOverlap) {
            return true;  // Rules conflict
        }
    }

    return false;
}

