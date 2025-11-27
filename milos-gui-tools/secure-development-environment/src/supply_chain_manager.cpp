#include "supply_chain_manager.h"
#include "milos/logging/logger.h"
#include "milos/logging/cve_client.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>

SupplyChainManager::SupplyChainManager(QObject* parent)
    : QObject(parent)
    , m_cveClient(nullptr)
{
}

SupplyChainManager::~SupplyChainManager() {
}

bool SupplyChainManager::initialize() {
    // Initialize logger
    if (!Logger::instance()->isInitialized()) {
        Logger::instance()->initialize("supply-chain-manager");
    }
    
    // Initialize CVE client
    m_cveClient = new CVEClient(this);
    if (!m_cveClient->initialize(true, true, true)) {
        LOG_WARNING("CVE client initialization failed, using fallback vulnerability detection");
    }
    
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
    // Check against local vulnerability cache first
    if (m_vulnerabilityCache.contains(dependency.name)) {
        const QList<QString>& vulnerableVersions = m_vulnerabilityCache[dependency.name];
        for (const QString& vulnVersion : vulnerableVersions) {
            if (isVersionVulnerable(dependency.version, vulnVersion)) {
                LOG_WARNING(QString("Vulnerability found in cache: %1 %2").arg(dependency.name, dependency.version));
                return true;
            }
        }
    }
    
    // Use CVE client if available
    if (m_cveClient) {
        QString ecosystem = dependency.source;  // "npm", "pip", "cargo", etc.
        QList<CVEClient::Vulnerability> vulns = m_cveClient->checkPackage(
            dependency.name, 
            dependency.version, 
            ecosystem
        );
        
        if (!vulns.isEmpty()) {
            LOG_WARNING(QString("CVE vulnerabilities found for %1 %2: %3 vulnerabilities")
                       .arg(dependency.name, dependency.version, QString::number(vulns.size())));
            
            // Cache vulnerabilities
            QStringList cveIds;
            for (const CVEClient::Vulnerability& vuln : vulns) {
                cveIds.append(vuln.cveId);
                if (!m_vulnerabilityCache.contains(dependency.name)) {
                    m_vulnerabilityCache[dependency.name] = QStringList();
                }
                m_vulnerabilityCache[dependency.name].append(vuln.affectedVersions);
            }
            
            return true;
        }
    }
    
    // Fallback: Check common vulnerable patterns
    QString nameLower = dependency.name.toLower();
    QString versionLower = dependency.version.toLower();
    
    // Known vulnerable patterns (expand with real CVE data)
    if (nameLower.contains("log4j") && versionLower.startsWith("2.") && versionLower < "2.17.0") {
        LOG_WARNING("Log4Shell vulnerability detected");
        return true;  // Log4Shell vulnerability
    }
    
    if (nameLower.contains("openssl") && versionLower < "3.0.0") {
        // Check for specific vulnerable OpenSSL versions
        if (versionLower.startsWith("1.0.") || versionLower.startsWith("1.1.")) {
            LOG_WARNING("Potentially vulnerable OpenSSL version detected");
            return true;
        }
    }
    
    // Check for development/pre-release versions in production
    if (versionLower.contains("alpha") || versionLower.contains("beta") || 
        versionLower.contains("rc") || versionLower.contains("dev") ||
        versionLower.contains("snapshot")) {
        LOG_WARNING(QString("Development/pre-release version detected: %1 %2")
                   .arg(dependency.name, dependency.version));
        // Warn but don't mark as vulnerable - depends on context
    }
    
    return false;
}

bool SupplyChainManager::isVersionVulnerable(const QString& currentVersion, const QString& vulnerableVersionRange) const {
    // Simple version range checking
    // In production, use proper semantic versioning library
    
    // Check if version matches range pattern (e.g., "<2.0.0", ">=1.0.0,<2.0.0")
    if (vulnerableVersionRange.contains("<")) {
        // Extract version from range
        QRegularExpression versionRegex(R"((\d+\.\d+\.\d+))");
        QRegularExpressionMatch match = versionRegex.match(vulnerableVersionRange);
        if (match.hasMatch()) {
            QString maxVersion = match.captured(1);
            return compareVersions(currentVersion, maxVersion) < 0;
        }
    }
    
    return false;
}

int SupplyChainManager::compareVersions(const QString& v1, const QString& v2) const {
    // Simple version comparison (assumes semantic versioning)
    QStringList parts1 = v1.split(".");
    QStringList parts2 = v2.split(".");
    
    int maxParts = qMax(parts1.size(), parts2.size());
    for (int i = 0; i < maxParts; ++i) {
        int part1 = (i < parts1.size()) ? parts1[i].toInt() : 0;
        int part2 = (i < parts2.size()) ? parts2[i].toInt() : 0;
        
        if (part1 < part2) return -1;
        if (part1 > part2) return 1;
    }
    
    return 0;
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
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_WARNING(QString("Failed to open Cargo.toml: %1").arg(filePath));
        return deps;
    }
    
    QTextStream in(&file);
    bool inDependenciesSection = false;
    bool inDevDependenciesSection = false;
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        
        // Skip empty lines and comments
        if (line.isEmpty() || line.startsWith("#")) {
            continue;
        }
        
        // Check for section headers
        if (line == "[dependencies]") {
            inDependenciesSection = true;
            inDevDependenciesSection = false;
            continue;
        } else if (line == "[dev-dependencies]") {
            inDependenciesSection = false;
            inDevDependenciesSection = true;
            continue;
        } else if (line.startsWith("[")) {
            // New section started
            inDependenciesSection = false;
            inDevDependenciesSection = false;
            continue;
        }
        
        // Parse dependency line
        if (inDependenciesSection || inDevDependenciesSection) {
            // Format: name = "version" or name = { version = "x.y.z", features = [...] }
            int equalsPos = line.indexOf('=');
            if (equalsPos > 0) {
                QString name = line.left(equalsPos).trimmed();
                QString value = line.mid(equalsPos + 1).trimmed();
                
                Dependency dep;
                dep.name = name;
                dep.source = "cargo";
                
                // Handle version string: "x.y.z" or { version = "x.y.z" }
                if (value.startsWith('"')) {
                    // Simple version string
                    int endQuote = value.indexOf('"', 1);
                    if (endQuote > 0) {
                        dep.version = value.mid(1, endQuote - 1);
                    }
                } else if (value.startsWith('{')) {
                    // Table format - extract version
                    QRegularExpression versionRegex(R"(version\s*=\s*"([^"]+)")");
                    QRegularExpressionMatch match = versionRegex.match(value);
                    if (match.hasMatch()) {
                        dep.version = match.captured(1);
                    } else {
                        dep.version = "unknown";
                    }
                } else {
                    dep.version = "unknown";
                }
                
                if (!dep.name.isEmpty()) {
                    deps.append(dep);
                }
            }
        }
    }
    
    file.close();
    LOG_INFO(QString("Parsed %1 dependencies from Cargo.toml").arg(deps.size()));
    return deps;
}







    
    int maxParts = qMax(parts1.size(), parts2.size());
    for (int i = 0; i < maxParts; ++i) {
        int part1 = (i < parts1.size()) ? parts1[i].toInt() : 0;
        int part2 = (i < parts2.size()) ? parts2[i].toInt() : 0;
        
        if (part1 < part2) return -1;
        if (part1 > part2) return 1;
    }
    
    return 0;
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
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_WARNING(QString("Failed to open Cargo.toml: %1").arg(filePath));
        return deps;
    }
    
    QTextStream in(&file);
    bool inDependenciesSection = false;
    bool inDevDependenciesSection = false;
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        
        // Skip empty lines and comments
        if (line.isEmpty() || line.startsWith("#")) {
            continue;
        }
        
        // Check for section headers
        if (line == "[dependencies]") {
            inDependenciesSection = true;
            inDevDependenciesSection = false;
            continue;
        } else if (line == "[dev-dependencies]") {
            inDependenciesSection = false;
            inDevDependenciesSection = true;
            continue;
        } else if (line.startsWith("[")) {
            // New section started
            inDependenciesSection = false;
            inDevDependenciesSection = false;
            continue;
        }
        
        // Parse dependency line
        if (inDependenciesSection || inDevDependenciesSection) {
            // Format: name = "version" or name = { version = "x.y.z", features = [...] }
            int equalsPos = line.indexOf('=');
            if (equalsPos > 0) {
                QString name = line.left(equalsPos).trimmed();
                QString value = line.mid(equalsPos + 1).trimmed();
                
                Dependency dep;
                dep.name = name;
                dep.source = "cargo";
                
                // Handle version string: "x.y.z" or { version = "x.y.z" }
                if (value.startsWith('"')) {
                    // Simple version string
                    int endQuote = value.indexOf('"', 1);
                    if (endQuote > 0) {
                        dep.version = value.mid(1, endQuote - 1);
                    }
                } else if (value.startsWith('{')) {
                    // Table format - extract version
                    QRegularExpression versionRegex(R"(version\s*=\s*"([^"]+)")");
                    QRegularExpressionMatch match = versionRegex.match(value);
                    if (match.hasMatch()) {
                        dep.version = match.captured(1);
                    } else {
                        dep.version = "unknown";
                    }
                } else {
                    dep.version = "unknown";
                }
                
                if (!dep.name.isEmpty()) {
                    deps.append(dep);
                }
            }
        }
    }
    
    file.close();
    LOG_INFO(QString("Parsed %1 dependencies from Cargo.toml").arg(deps.size()));
    return deps;
}






