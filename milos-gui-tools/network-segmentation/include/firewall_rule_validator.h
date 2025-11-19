#ifndef FIREWALL_RULE_VALIDATOR_H
#define FIREWALL_RULE_VALIDATOR_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QPair>
#include "firewall_rule_generator.h"

/**
 * @brief Rule Validation Result
 */
struct RuleValidationResult {
    bool isValid;
    QStringList errors;
    QStringList warnings;
    QList<QString> conflictingRules;  // Rule IDs that conflict
};

/**
 * @brief Firewall Rule Validator
 * 
 * Validates firewall rules before application.
 */
class FirewallRuleValidator : public QObject {
    Q_OBJECT

public:
    explicit FirewallRuleValidator(QObject* parent = nullptr);
    ~FirewallRuleValidator();

    /**
     * @brief Initialize validator
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Validate single rule
     * @param rule Firewall rule to validate
     * @return Validation result
     */
    RuleValidationResult validateRule(const FirewallRule& rule);

    /**
     * @brief Validate multiple rules
     * @param rules List of firewall rules
     * @return Validation result
     */
    RuleValidationResult validateRules(const QList<FirewallRule>& rules);

    /**
     * @brief Check for rule conflicts
     * @param rules List of firewall rules
     * @return List of conflicting rule pairs
     */
    QList<QPair<QString, QString>> detectConflicts(const QList<FirewallRule>& rules);

Q_SIGNALS:
    /**
     * @brief Emitted when validation completes
     */
    void validationCompleted(const RuleValidationResult& result);

private:
    bool m_initialized;

    /**
     * @brief Validate rule syntax
     */
    bool validateRuleSyntax(const FirewallRule& rule);

    /**
     * @brief Validate IP address/network
     */
    bool validateNetworkAddress(const QString& address);

    /**
     * @brief Validate port specification
     */
    bool validatePorts(const QStringList& ports);

    /**
     * @brief Check if two rules conflict
     */
    bool rulesConflict(const FirewallRule& rule1, const FirewallRule& rule2);
};

#endif // FIREWALL_RULE_VALIDATOR_H

