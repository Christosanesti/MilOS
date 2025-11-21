#ifndef IDS_RULES_ENGINE_H
#define IDS_RULES_ENGINE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QList>

/**
 * @brief IDS Rule
 */
struct IDSRule {
    QString id;
    QString name;
    QString description;
    QString pattern;
    QString action;  // "alert", "block", "log"
    int priority;
    bool enabled;
    QVariantMap conditions;
};

/**
 * @brief IDS Rules Engine
 * 
 * Custom intrusion detection rules engine.
 */
class IDSRulesEngine : public QObject {
    Q_OBJECT

public:
    explicit IDSRulesEngine(QObject* parent = nullptr);
    ~IDSRulesEngine();

    /**
     * @brief Initialize IDS rules engine
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Add rule
     * @param rule IDS rule
     * @return true if add successful, false otherwise
     */
    bool addRule(const IDSRule& rule);

    /**
     * @brief Remove rule
     * @param ruleId Rule ID
     * @return true if remove successful, false otherwise
     */
    bool removeRule(const QString& ruleId);

    /**
     * @brief Get rule
     * @param ruleId Rule ID
     * @return IDS rule or empty if not found
     */
    IDSRule getRule(const QString& ruleId) const;

    /**
     * @brief Get all rules
     * @return List of IDS rules
     */
    QList<IDSRule> getAllRules() const;

    /**
     * @brief Evaluate packet against rules
     * @param packetData Packet data
     * @return List of matched rules
     */
    QList<IDSRule> evaluatePacket(const QVariantMap& packetData) const;

    /**
     * @brief Enable/disable rule
     * @param ruleId Rule ID
     * @param enabled Enabled state
     * @return true if set successful, false otherwise
     */
    bool setRuleEnabled(const QString& ruleId, bool enabled);

Q_SIGNALS:
    /**
     * @brief Emitted when rule is matched
     */
    void ruleMatched(const IDSRule& rule, const QVariantMap& packetData);

private:
    QMap<QString, IDSRule> m_rules;
    
    bool matchRule(const IDSRule& rule, const QVariantMap& packetData) const;
    QString generateRuleId() const;
};

Q_DECLARE_METATYPE(IDSRule)

#endif // IDS_RULES_ENGINE_H

