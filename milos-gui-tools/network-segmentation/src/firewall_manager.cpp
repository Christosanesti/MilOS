#include "firewall_manager.h"
#include "firewall_rule_generator.h"
#include "firewall_rule_validator.h"
#include "segment_config.h"
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <iostream>

FirewallManager::FirewallManager(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_ruleGenerator(nullptr)
    , m_ruleValidator(nullptr)
{
}

FirewallManager::~FirewallManager() {
}

bool FirewallManager::initialize() {
    if (m_initialized) {
        return true;
    }

    m_ruleGenerator = new FirewallRuleGenerator(this);
    if (!m_ruleGenerator->initialize()) {
        std::cerr << "Failed to initialize firewall rule generator" << std::endl;
        return false;
    }

    m_ruleValidator = new FirewallRuleValidator(this);
    if (!m_ruleValidator->initialize()) {
        std::cerr << "Failed to initialize firewall rule validator" << std::endl;
        return false;
    }

    m_initialized = true;
    return true;
}

bool FirewallManager::generateRulesFromSegments(const QList<SegmentConfig>& segments) {
    if (!m_initialized || !m_ruleGenerator) {
        return false;
    }

    m_rules = m_ruleGenerator->generateRulesFromSegments(segments);
    emit rulesChanged();
    return true;
}

QString FirewallManager::validateRules() const {
    if (!m_ruleValidator) {
        return QString("{\"isValid\":false,\"errors\":[\"Validator not initialized\"]}");
    }

    RuleValidationResult result = m_ruleValidator->validateRules(m_rules);
    
    QJsonObject obj;
    obj["isValid"] = result.isValid;
    
    QJsonArray errorsArray;
    for (const QString& error : result.errors) {
        errorsArray.append(error);
    }
    obj["errors"] = errorsArray;
    
    QJsonArray warningsArray;
    for (const QString& warning : result.warnings) {
        warningsArray.append(warning);
    }
    obj["warnings"] = warningsArray;
    
    QJsonArray conflictsArray;
    for (const QString& conflict : result.conflictingRules) {
        conflictsArray.append(conflict);
    }
    obj["conflictingRules"] = conflictsArray;
    
    QJsonDocument doc(obj);
    return QString::fromUtf8(doc.toJson());
}

QString FirewallManager::previewRules() const {
    QString preview;
    QString firewallType = detectFirewallType();
    
    preview += QString("# Firewall Rules Preview (%1)\n").arg(firewallType);
    preview += "# Generated from network segmentation\n\n";
    
    for (const FirewallRule& rule : m_rules) {
        if (!rule.enabled) {
            continue;
        }
        
        preview += QString("# %1: %2\n").arg(rule.name, rule.description);
        
        if (firewallType == "iptables") {
            preview += QString("iptables -A INPUT%1\n").arg(rule.toIptablesRule());
        } else if (firewallType == "nftables") {
            preview += QString("nft add rule ip filter input %1\n").arg(rule.toNftablesRule());
        }
        
        preview += "\n";
    }
    
    return preview;
}

bool FirewallManager::applyRules() {
    if (!m_initialized) {
        return false;
    }

    // Validate rules first
    RuleValidationResult validation = m_ruleValidator->validateRules(m_rules);
    if (!validation.isValid) {
        std::cerr << "Rules validation failed: " << validation.errors.join(", ").toStdString() << std::endl;
        emit rulesApplied(false);
        return false;
    }

    // Save current rules for rollback
    m_lastAppliedRules = m_rules;

    // Apply rules based on firewall type
    QString firewallType = detectFirewallType();
    bool success = false;

    if (firewallType == "iptables") {
        success = applyIptablesRules(m_rules);
    } else if (firewallType == "nftables") {
        success = applyNftablesRules(m_rules);
    } else {
        std::cerr << "Unknown firewall type: " << firewallType.toStdString() << std::endl;
        emit rulesApplied(false);
        return false;
    }

    emit rulesApplied(success);
    return success;
}

bool FirewallManager::rollbackRules() {
    if (m_lastAppliedRules.isEmpty()) {
        std::cerr << "No previous rules to rollback" << std::endl;
        emit rulesRolledBack(false);
        return false;
    }

    // Restore previous rules
    m_rules = m_lastAppliedRules;
    emit rulesChanged();

    // Re-apply previous rules
    QString firewallType = detectFirewallType();
    bool success = false;

    if (firewallType == "iptables") {
        success = applyIptablesRules(m_rules);
    } else if (firewallType == "nftables") {
        success = applyNftablesRules(m_rules);
    }

    emit rulesRolledBack(success);
    return success;
}

QString FirewallManager::detectFirewallType() const {
    // Check for nftables first (newer)
    QProcess process;
    process.start("which", QStringList() << "nft");
    process.waitForFinished();
    if (process.exitCode() == 0) {
        return "nftables";
    }

    // Check for iptables
    process.start("which", QStringList() << "iptables");
    process.waitForFinished();
    if (process.exitCode() == 0) {
        return "iptables";
    }

    return "unknown";
}

bool FirewallManager::applyIptablesRules(const QList<FirewallRule>& rules) {
    // Note: This requires root privileges
    // In production, this would use proper privilege escalation
    
    for (const FirewallRule& rule : rules) {
        if (!rule.enabled) {
            continue;
        }

        QString iptablesRule = rule.toIptablesRule();
        QString command = QString("iptables -A INPUT %1").arg(iptablesRule);
        
        QProcess process;
        process.start("sh", QStringList() << "-c" << command);
        process.waitForFinished();
        
        if (process.exitCode() != 0) {
            std::cerr << "Failed to apply iptables rule: " << command.toStdString() << std::endl;
            std::cerr << "Error: " << QString::fromUtf8(process.readAllStandardError()).toStdString() << std::endl;
            return false;
        }
    }

    return true;
}

bool FirewallManager::applyNftablesRules(const QList<FirewallRule>& rules) {
    // Note: This requires root privileges
    // In production, this would use proper privilege escalation
    
    for (const FirewallRule& rule : rules) {
        if (!rule.enabled) {
            continue;
        }

        QString nftablesRule = rule.toNftablesRule();
        QString command = QString("nft add rule ip filter input %1").arg(nftablesRule);
        
        QProcess process;
        process.start("sh", QStringList() << "-c" << command);
        process.waitForFinished();
        
        if (process.exitCode() != 0) {
            std::cerr << "Failed to apply nftables rule: " << command.toStdString() << std::endl;
            std::cerr << "Error: " << QString::fromUtf8(process.readAllStandardError()).toStdString() << std::endl;
            return false;
        }
    }

    return true;
}

