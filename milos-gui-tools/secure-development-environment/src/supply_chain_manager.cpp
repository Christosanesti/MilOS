#include "supply_chain_manager.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

SupplyChainManager::SupplyChainManager(QObject* parent)
    : QObject(parent)
{
}

SupplyChainManager::~SupplyChainManager() {
}

bool SupplyChainManager::initialize() {
    return true;
}

QList<Dependency> SupplyChainManager::scanDependencies(const QString& projectPath) {
    QList<Dependency> dependencies;
    
    QDir projectDir(projectPath);
    
    // Check for package.json (Node.js)
    QString packageJsonPath = projectDir.filePath("package.json");
    if (QFile::exists(packageJsonPath)) {
        dependencies.append(parsePackageJson(packageJsonPath));
    }
    
    // Check for requirements.txt (Python)
    QString requirementsPath = projectDir.filePath("requirements.txt");
    if (QFile::exists(requirementsPath)) {
        dependencies.append(parseRequirementsTxt(requirementsPath));
    }
    
    // Check for Cargo.toml (Rust)
    QString cargoTomlPath = projectDir.filePath("Cargo.toml");
    if (QFile::exists(cargoTomlPath)) {
        dependencies.append(parseCargoToml(cargoTomlPath));
    }
    
    // Check for vulnerabilities
    for (Dependency& dep : dependencies) {
        dep.isVulnerable = checkVulnerability(dep);
        if (dep.isVulnerable) {
            emit vulnerabilityFound(dep);
        }
    }
    
    m_dependencies.append(dependencies);
    
    return dependencies;
}

bool SupplyChainManager::checkVulnerability(const Dependency& dependency) {
    // In production, would check against vulnerability databases (CVE, etc.)
    // For now, simple placeholder check
    
    // Simulate some known vulnerable packages
    if (dependency.name.contains("old") || dependency.version.startsWith("0.")) {
        return true;
    }
    
    return false;
}

QVariantMap SupplyChainManager::getDependencyReport(const QString& projectPath) const {
    QVariantMap report;
    
    int totalDeps = 0;
    int vulnerableDeps = 0;
    
    for (const Dependency& dep : m_dependencies) {
        totalDeps++;
        if (dep.isVulnerable) {
            vulnerableDeps++;
        }
    }
    
    report["total_dependencies"] = totalDeps;
    report["vulnerable_dependencies"] = vulnerableDeps;
    report["secure_dependencies"] = totalDeps - vulnerableDeps;
    
    return report;
}

QList<Dependency> SupplyChainManager::parsePackageJson(const QString& filePath) const {
    QList<Dependency> deps;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return deps;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull()) {
        return deps;
    }
    
    QJsonObject obj = doc.object();
    QJsonObject depsObj = obj.value("dependencies").toObject();
    
    for (auto it = depsObj.begin(); it != depsObj.end(); ++it) {
        Dependency dep;
        dep.name = it.key();
        dep.version = it.value().toString();
        dep.source = "npm";
        deps.append(dep);
    }
    
    return deps;
}

QList<Dependency> SupplyChainManager::parseRequirementsTxt(const QString& filePath) const {
    QList<Dependency> deps;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return deps;
    }
    
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith("#")) {
            continue;
        }
        
        Dependency dep;
        QStringList parts = line.split("==");
        if (parts.size() == 2) {
            dep.name = parts[0];
            dep.version = parts[1];
        } else {
            dep.name = line;
            dep.version = "unknown";
        }
        dep.source = "pip";
        deps.append(dep);
    }
    
    return deps;
}

QList<Dependency> SupplyChainManager::parseCargoToml(const QString& filePath) const {
    QList<Dependency> deps;
    
    // In production, would properly parse TOML
    // For now, return empty list
    
    return deps;
}

