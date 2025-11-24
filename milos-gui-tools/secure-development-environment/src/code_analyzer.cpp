#include "code_analyzer.h"
#include <QUuid>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

CodeAnalyzer::CodeAnalyzer(QObject* parent)
    : QObject(parent)
{
}

CodeAnalyzer::~CodeAnalyzer() {
}

bool CodeAnalyzer::initialize() {
    return true;
}

QList<SecurityIssue> CodeAnalyzer::analyzeFile(const QString& filePath) {
    QList<SecurityIssue> issues;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return issues;
    }
    
    QTextStream in(&file);
    int lineNumber = 0;
    
    while (!in.atEnd()) {
        lineNumber++;
        QString line = in.readLine();
        
        SecurityIssue issue = checkForVulnerability(line, filePath, lineNumber);
        if (!issue.id.isEmpty()) {
            issues.append(issue);
            m_issues.append(issue);
            emit securityIssueFound(issue);
        }
    }
    
    return issues;
}

QList<SecurityIssue> CodeAnalyzer::analyzeDirectory(const QString& directoryPath) {
    QList<SecurityIssue> allIssues;
    
    // In production, would recursively scan directory
    // For now, return empty list
    
    return allIssues;
}

QVariantMap CodeAnalyzer::getSecurityStatistics() const {
    QVariantMap stats;
    
    int lowCount = 0, mediumCount = 0, highCount = 0, criticalCount = 0;
    
    for (const SecurityIssue& issue : m_issues) {
        if (issue.severity == "low") lowCount++;
        else if (issue.severity == "medium") mediumCount++;
        else if (issue.severity == "high") highCount++;
        else if (issue.severity == "critical") criticalCount++;
    }
    
    stats["total_issues"] = m_issues.size();
    stats["low"] = lowCount;
    stats["medium"] = mediumCount;
    stats["high"] = highCount;
    stats["critical"] = criticalCount;
    
    return stats;
}

SecurityIssue CodeAnalyzer::checkForVulnerability(const QString& code, const QString& filePath, int line) const {
    SecurityIssue issue;
    
    // Check for common vulnerabilities
    if (code.contains("eval(") || code.contains("exec(")) {
        issue.id = generateIssueId();
        issue.severity = "critical";
        issue.type = "vulnerability";
        issue.file = filePath;
        issue.line = line;
        issue.description = "Use of eval() or exec() detected - code injection risk";
        issue.recommendation = "Avoid using eval() or exec(). Use safer alternatives.";
        return issue;
    }
    
    if (code.contains("password") && code.contains("=") && !code.contains("encrypt")) {
        issue.id = generateIssueId();
        issue.severity = "high";
        issue.type = "weakness";
        issue.file = filePath;
        issue.line = line;
        issue.description = "Potential hardcoded password";
        issue.recommendation = "Use secure password storage and never hardcode passwords.";
        return issue;
    }
    
    if (code.contains("SELECT") && code.contains("$") && !code.contains("prepare")) {
        issue.id = generateIssueId();
        issue.severity = "high";
        issue.type = "vulnerability";
        issue.file = filePath;
        issue.line = line;
        issue.description = "Potential SQL injection vulnerability";
        issue.recommendation = "Use prepared statements or parameterized queries.";
        return issue;
    }
    
    return issue;
}

QString CodeAnalyzer::generateIssueId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}






