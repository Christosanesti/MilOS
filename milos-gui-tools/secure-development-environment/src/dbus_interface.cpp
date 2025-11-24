#include "dbus_interface.h"
#include "secure_dev_system.h"
#include "code_analyzer.h"
#include "supply_chain_manager.h"
#include <QDebug>
#include <QDBusConnection>

SecureDevDBusInterface::SecureDevDBusInterface(QObject* parent)
    : QObject(parent)
    , m_system(nullptr)
{
}

SecureDevDBusInterface::~SecureDevDBusInterface() {
}

bool SecureDevDBusInterface::initialize() {
    if (!QDBusConnection::sessionBus().registerService("org.milos.SecureDevelopment")) {
        qWarning() << "Failed to register D-Bus service";
        return false;
    }
    
    if (!QDBusConnection::sessionBus().registerObject("/org/milos/SecureDevelopment", this,
                                                        QDBusConnection::ExportAllSlots |
                                                        QDBusConnection::ExportAllSignals)) {
        qWarning() << "Failed to register D-Bus object";
        return false;
    }
    
    return true;
}

void SecureDevDBusInterface::setSystem(SecureDevSystem* system) {
    m_system = system;
}

QVariantMap SecureDevDBusInterface::AnalyzeFile(const QString& filePath) {
    QVariantMap result;
    
    if (!m_system || !m_system->getCodeAnalyzer()) {
        return result;
    }
    
    QList<SecurityIssue> issues = m_system->getCodeAnalyzer()->analyzeFile(filePath);
    
    QVariantList issueList;
    for (const SecurityIssue& issue : issues) {
        QVariantMap issueMap;
        issueMap["id"] = issue.id;
        issueMap["severity"] = issue.severity;
        issueMap["type"] = issue.type;
        issueMap["file"] = issue.file;
        issueMap["line"] = issue.line;
        issueMap["description"] = issue.description;
        issueMap["recommendation"] = issue.recommendation;
        issueList.append(issueMap);
    }
    
    result["issues"] = issueList;
    result["count"] = issues.size();
    
    return result;
}

QVariantMap SecureDevDBusInterface::AnalyzeDirectory(const QString& directoryPath) {
    QVariantMap result;
    
    if (!m_system || !m_system->getCodeAnalyzer()) {
        return result;
    }
    
    QList<SecurityIssue> issues = m_system->getCodeAnalyzer()->analyzeDirectory(directoryPath);
    
    QVariantList issueList;
    for (const SecurityIssue& issue : issues) {
        QVariantMap issueMap;
        issueMap["id"] = issue.id;
        issueMap["severity"] = issue.severity;
        issueMap["type"] = issue.type;
        issueMap["file"] = issue.file;
        issueMap["line"] = issue.line;
        issueMap["description"] = issue.description;
        issueList.append(issueMap);
    }
    
    result["issues"] = issueList;
    result["count"] = issues.size();
    
    return result;
}

QVariantMap SecureDevDBusInterface::ScanDependencies(const QString& projectPath) {
    QVariantMap result;
    
    if (!m_system || !m_system->getSupplyChainManager()) {
        return result;
    }
    
    QList<Dependency> deps = m_system->getSupplyChainManager()->scanDependencies(projectPath);
    
    QVariantList depList;
    for (const Dependency& dep : deps) {
        QVariantMap depMap;
        depMap["name"] = dep.name;
        depMap["version"] = dep.version;
        depMap["source"] = dep.source;
        depMap["is_vulnerable"] = dep.isVulnerable;
        depMap["vulnerabilities"] = dep.vulnerabilities;
        depList.append(depMap);
    }
    
    result["dependencies"] = depList;
    result["report"] = m_system->getSupplyChainManager()->getDependencyReport(projectPath);
    
    return result;
}

QVariantMap SecureDevDBusInterface::GetSecurityStatistics() {
    QVariantMap result;
    
    if (!m_system || !m_system->getCodeAnalyzer()) {
        return result;
    }
    
    return m_system->getCodeAnalyzer()->getSecurityStatistics();
}






