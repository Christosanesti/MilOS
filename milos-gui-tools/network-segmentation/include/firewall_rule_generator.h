#ifndef FIREWALL_RULE_GENERATOR_H
#define FIREWALL_RULE_GENERATOR_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include "segment_config.h"

/**
 * @brief Firewall Rule
 */
struct FirewallRule {
    QString ruleId;
    QString name;
    QString description;
    QString action;  // ACCEPT, DROP, REJECT
    QString source;  // Source IP/network
    QString destination;  // Destination IP/network
    QString protocol;  // TCP, UDP, ICMP, etc.
    QStringList sourcePorts;
    QStringList destinationPorts;
    int priority;
    bool enabled;
    QString segmentId;  // Associated segment

    FirewallRule() : priority(0), enabled(true) {}
    
    QString toIptablesRule() const;
    QString toNftablesRule() const;
};

/**
 * @brief Firewall Rule Generator
 * 
 * Generates firewall rules based on network segmentation.
 */
class FirewallRuleGenerator : public QObject {
    Q_OBJECT

public:
    explicit FirewallRuleGenerator(QObject* parent = nullptr);
    ~FirewallRuleGenerator();

    /**
     * @brief Initialize rule generator
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Generate firewall rules from segments
     * @param segments List of segment configurations
     * @return List of generated firewall rules
     */
    QList<FirewallRule> generateRulesFromSegments(const QList<SegmentConfig>& segments);

    /**
     * @brief Generate isolation rules for segment
     * @param segment Segment configuration
     * @return List of isolation rules
     */
    QList<FirewallRule> generateIsolationRules(const SegmentConfig& segment);

    /**
     * @brief Generate connection rules between segments
     * @param segment1 First segment
     * @param segment2 Second segment
     * @return List of connection rules
     */
    QList<FirewallRule> generateConnectionRules(const SegmentConfig& segment1, const SegmentConfig& segment2);

    /**
     * @brief Generate default rules
     * @return List of default firewall rules
     */
    QList<FirewallRule> generateDefaultRules();

Q_SIGNALS:
    /**
     * @brief Emitted when rules are generated
     */
    void rulesGenerated(const QList<FirewallRule>& rules);

private:
    bool m_initialized;

    /**
     * @brief Generate rule ID
     */
    QString generateRuleId() const;
};

#endif // FIREWALL_RULE_GENERATOR_H

