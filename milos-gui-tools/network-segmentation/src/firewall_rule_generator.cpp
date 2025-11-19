#include "firewall_rule_generator.h"
#include "segment_config.h"
#include <QUuid>
#include <QDebug>
#include <iostream>

FirewallRuleGenerator::FirewallRuleGenerator(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
{
}

FirewallRuleGenerator::~FirewallRuleGenerator() {
}

bool FirewallRuleGenerator::initialize() {
    if (m_initialized) {
        return true;
    }

    m_initialized = true;
    return true;
}

QString FirewallRule::toIptablesRule() const {
    QString rule;
    
    // Build iptables rule
    if (!source.isEmpty()) {
        rule += QString(" -s %1").arg(source);
    }
    if (!destination.isEmpty()) {
        rule += QString(" -d %1").arg(destination);
    }
    if (!protocol.isEmpty()) {
        rule += QString(" -p %1").arg(protocol);
    }
    if (!sourcePorts.isEmpty()) {
        rule += QString(" --sport %1").arg(sourcePorts.join(","));
    }
    if (!destinationPorts.isEmpty()) {
        rule += QString(" --dport %1").arg(destinationPorts.join(","));
    }
    rule += QString(" -j %1").arg(action);
    
    return rule.trimmed();
}

QString FirewallRule::toNftablesRule() const {
    QString rule;
    
    // Build nftables rule
    rule += action.toLower();
    if (!source.isEmpty()) {
        rule += QString(" ip saddr %1").arg(source);
    }
    if (!destination.isEmpty()) {
        rule += QString(" ip daddr %1").arg(destination);
    }
    if (!protocol.isEmpty()) {
        rule += QString(" %1").arg(protocol.toLower());
    }
    if (!sourcePorts.isEmpty()) {
        rule += QString(" sport { %1 }").arg(sourcePorts.join(", "));
    }
    if (!destinationPorts.isEmpty()) {
        rule += QString(" dport { %1 }").arg(destinationPorts.join(", "));
    }
    
    return rule.trimmed();
}

QList<FirewallRule> FirewallRuleGenerator::generateRulesFromSegments(const QList<SegmentConfig>& segments) {
    QList<FirewallRule> rules;

    // Generate isolation rules for isolated segments
    for (const SegmentConfig& segment : segments) {
        if (segment.isIsolated) {
            QList<FirewallRule> isolationRules = generateIsolationRules(segment);
            rules.append(isolationRules);
        }
    }

    // Generate connection rules between non-isolated segments
    for (int i = 0; i < segments.size(); i++) {
        const SegmentConfig& segment1 = segments[i];
        if (segment1.isIsolated) {
            continue;
        }

        for (int j = i + 1; j < segments.size(); j++) {
            const SegmentConfig& segment2 = segments[j];
            if (segment2.isIsolated) {
                continue;
            }

            QList<FirewallRule> connectionRules = generateConnectionRules(segment1, segment2);
            rules.append(connectionRules);
        }
    }

    // Add default rules
    rules.append(generateDefaultRules());

    emit rulesGenerated(rules);
    return rules;
}

QList<FirewallRule> FirewallRuleGenerator::generateIsolationRules(const SegmentConfig& segment) {
    QList<FirewallRule> rules;

    // Block all traffic from isolated segment
    FirewallRule blockOutbound;
    blockOutbound.ruleId = generateRuleId();
    blockOutbound.name = QString("Isolate %1 - Outbound").arg(segment.name);
    blockOutbound.description = QString("Block outbound traffic from isolated segment %1").arg(segment.name);
    blockOutbound.action = "DROP";
    blockOutbound.source = segment.networkAddress;
    blockOutbound.priority = segment.priority;
    blockOutbound.segmentId = segment.segmentId;
    rules.append(blockOutbound);

    // Block all traffic to isolated segment
    FirewallRule blockInbound;
    blockInbound.ruleId = generateRuleId();
    blockInbound.name = QString("Isolate %1 - Inbound").arg(segment.name);
    blockInbound.description = QString("Block inbound traffic to isolated segment %1").arg(segment.name);
    blockInbound.action = "DROP";
    blockInbound.destination = segment.networkAddress;
    blockInbound.priority = segment.priority;
    blockInbound.segmentId = segment.segmentId;
    rules.append(blockInbound);

    return rules;
}

QList<FirewallRule> FirewallRuleGenerator::generateConnectionRules(const SegmentConfig& segment1, const SegmentConfig& segment2) {
    QList<FirewallRule> rules;

    // Allow traffic from segment1 to segment2
    FirewallRule allowRule;
    allowRule.ruleId = generateRuleId();
    allowRule.name = QString("Allow %1 -> %2").arg(segment1.name, segment2.name);
    allowRule.description = QString("Allow traffic from %1 to %2").arg(segment1.name, segment2.name);
    allowRule.action = "ACCEPT";
    allowRule.source = segment1.networkAddress;
    allowRule.destination = segment2.networkAddress;
    
    // Apply allowed protocols and ports from segment configurations
    if (!segment1.allowedProtocols.isEmpty()) {
        allowRule.protocol = segment1.allowedProtocols.first();
    }
    if (!segment1.allowedPorts.isEmpty()) {
        allowRule.destinationPorts = segment1.allowedPorts;
    }
    
    allowRule.priority = qMin(segment1.priority, segment2.priority);
    rules.append(allowRule);

    // Allow traffic from segment2 to segment1 (bidirectional)
    FirewallRule allowRuleReverse;
    allowRuleReverse.ruleId = generateRuleId();
    allowRuleReverse.name = QString("Allow %1 -> %2").arg(segment2.name, segment1.name);
    allowRuleReverse.description = QString("Allow traffic from %1 to %2").arg(segment2.name, segment1.name);
    allowRuleReverse.action = "ACCEPT";
    allowRuleReverse.source = segment2.networkAddress;
    allowRuleReverse.destination = segment1.networkAddress;
    
    if (!segment2.allowedProtocols.isEmpty()) {
        allowRuleReverse.protocol = segment2.allowedProtocols.first();
    }
    if (!segment2.allowedPorts.isEmpty()) {
        allowRuleReverse.destinationPorts = segment2.allowedPorts;
    }
    
    allowRuleReverse.priority = qMin(segment1.priority, segment2.priority);
    rules.append(allowRuleReverse);

    return rules;
}

QList<FirewallRule> FirewallRuleGenerator::generateDefaultRules() {
    QList<FirewallRule> rules;

    // Default deny rule
    FirewallRule defaultDeny;
    defaultDeny.ruleId = generateRuleId();
    defaultDeny.name = "Default Deny";
    defaultDeny.description = "Default deny all traffic";
    defaultDeny.action = "DROP";
    defaultDeny.priority = 1000;  // Lowest priority
    rules.append(defaultDeny);

    return rules;
}

QString FirewallRuleGenerator::generateRuleId() const {
    return QUuid::createUuid().toString();
}

