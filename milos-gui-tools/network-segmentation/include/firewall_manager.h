#ifndef FIREWALL_MANAGER_H
#define FIREWALL_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include "firewall_rule_generator.h"
#include "firewall_rule_validator.h"

/**
 * @brief Firewall Manager
 * 
 * Manages firewall rules and application to system firewall.
 */
class FirewallManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QList<FirewallRule> rules READ rules NOTIFY rulesChanged)

public:
    explicit FirewallManager(QObject* parent = nullptr);
    ~FirewallManager();

    /**
     * @brief Initialize firewall manager
     * @return true if initialization successful, false otherwise
     */
    Q_INVOKABLE bool initialize();

    /**
     * @brief Get all rules
     */
    QList<FirewallRule> rules() const { return m_rules; }

    /**
     * @brief Generate rules from segments
     * @param segments List of segment configurations
     * @return true if generation successful, false otherwise
     */
    Q_INVOKABLE bool generateRulesFromSegments(const QList<SegmentConfig>& segments);

    /**
     * @brief Validate rules
     * @return Validation result as JSON string
     */
    Q_INVOKABLE QString validateRules() const;

    /**
     * @brief Preview rules
     * @return Rules as iptables/nftables commands
     */
    Q_INVOKABLE QString previewRules() const;

    /**
     * @brief Apply rules to system firewall
     * @return true if application successful, false otherwise
     */
    Q_INVOKABLE bool applyRules();

    /**
     * @brief Rollback last rule application
     * @return true if rollback successful, false otherwise
     */
    Q_INVOKABLE bool rollbackRules();

Q_SIGNALS:
    /**
     * @brief Emitted when rules change
     */
    void rulesChanged();

    /**
     * @brief Emitted when rules are applied
     */
    void rulesApplied(bool success);

    /**
     * @brief Emitted when rules are rolled back
     */
    void rulesRolledBack(bool success);

private:
    bool m_initialized;
    FirewallRuleGenerator* m_ruleGenerator;
    FirewallRuleValidator* m_ruleValidator;
    QList<FirewallRule> m_rules;
    QList<FirewallRule> m_lastAppliedRules;  // For rollback

    /**
     * @brief Detect firewall type (iptables/nftables)
     */
    QString detectFirewallType() const;

    /**
     * @brief Apply iptables rules
     */
    bool applyIptablesRules(const QList<FirewallRule>& rules);

    /**
     * @brief Apply nftables rules
     */
    bool applyNftablesRules(const QList<FirewallRule>& rules);
};

#endif // FIREWALL_MANAGER_H

