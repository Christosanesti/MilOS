#ifndef SUPPLY_CHAIN_MANAGER_H
#define SUPPLY_CHAIN_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

/**
 * @brief Dependency
 */
struct Dependency {
    QString name;
    QString version;
    QString source;  // "npm", "pip", "cargo", etc.
    bool isVulnerable;
    QStringList vulnerabilities;
};

/**
 * @brief Supply Chain Manager
 * 
 * Manages software supply chain security.
 */
class SupplyChainManager : public QObject {
    Q_OBJECT

public:
    explicit SupplyChainManager(QObject* parent = nullptr);
    ~SupplyChainManager();

    /**
     * @brief Initialize supply chain manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Scan dependencies
     * @param projectPath Project path
     * @return List of dependencies
     */
    QList<Dependency> scanDependencies(const QString& projectPath);

    /**
     * @brief Check for vulnerabilities
     * @param dependency Dependency to check
     * @return true if vulnerable, false otherwise
     */
    bool checkVulnerability(const Dependency& dependency);

    /**
     * @brief Get dependency report
     * @param projectPath Project path
     * @return Dependency report
     */
    QVariantMap getDependencyReport(const QString& projectPath) const;

Q_SIGNALS:
    /**
     * @brief Emitted when vulnerability is found
     */
    void vulnerabilityFound(const Dependency& dependency);

private:
    QList<Dependency> m_dependencies;
    
    QList<Dependency> parsePackageJson(const QString& filePath) const;
    QList<Dependency> parseRequirementsTxt(const QString& filePath) const;
    QList<Dependency> parseCargoToml(const QString& filePath) const;
};

Q_DECLARE_METATYPE(Dependency)

#endif // SUPPLY_CHAIN_MANAGER_H





