#include "ids_rules_engine.h"
#include <QUuid>
#include <QDebug>
#include <QRegularExpression>

IDSRulesEngine::IDSRulesEngine(QObject* parent)
    : QObject(parent)
{
}

IDSRulesEngine::~IDSRulesEngine() {
}

bool IDSRulesEngine::initialize() {
    return true;
}

bool IDSRulesEngine::addRule(const IDSRule& rule) {
    IDSRule ruleCopy = rule;
    if (ruleCopy.id.isEmpty()) {
        ruleCopy.id = generateRuleId();
    }
    
    m_rules[ruleCopy.id] = ruleCopy;
    return true;
}

bool IDSRulesEngine::removeRule(const QString& ruleId) {
    if (!m_rules.contains(ruleId)) {
        return false;
    }
    
    m_rules.remove(ruleId);
    return true;
}

IDSRule IDSRulesEngine::getRule(const QString& ruleId) const {
    return m_rules.value(ruleId, IDSRule());
}

QList<IDSRule> IDSRulesEngine::getAllRules() const {
    return m_rules.values();
}

QList<IDSRule> IDSRulesEngine::evaluatePacket(const QVariantMap& packetData) const {
    QList<IDSRule> matchedRules;
    
    for (const IDSRule& rule : m_rules) {
        if (!rule.enabled) {
            continue;
        }
        
        if (matchRule(rule, packetData)) {
            matchedRules.append(rule);
            emit ruleMatched(rule, packetData);
        }
    }
    
    return matchedRules;
}

bool IDSRulesEngine::setRuleEnabled(const QString& ruleId, bool enabled) {
    if (!m_rules.contains(ruleId)) {
        return false;
    }
    
    IDSRule& rule = m_rules[ruleId];
    rule.enabled = enabled;
    
    return true;
}

bool IDSRulesEngine::matchRule(const IDSRule& rule, const QVariantMap& packetData) const {
    // Match pattern against packet data
    QString pattern = rule.pattern;
    
    // Simple pattern matching (in production, would use more sophisticated matching)
    if (pattern.contains("port")) {
        // Extract port from pattern
        QRegularExpression portRegex(R"(\bport\s*[:=]\s*(\d+))");
        QRegularExpressionMatch match = portRegex.match(pattern);
        if (match.hasMatch()) {
            int port = match.captured(1).toInt();
            int destPort = packetData.value("destination_port").toInt();
            if (destPort == port) {
                return true;
            }
        }
    }
    
    // Check conditions
    if (!rule.conditions.isEmpty()) {
        for (auto it = rule.conditions.begin(); it != rule.conditions.end(); ++it) {
            QString key = it.key();
            QVariant expectedValue = it.value();
            QVariant actualValue = packetData.value(key);
            
            if (actualValue != expectedValue) {
                return false;
            }
        }
    }
    
    return false;
}

QString IDSRulesEngine::generateRuleId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

