#ifndef AIR_GAP_DEPLOYMENT_MANAGER_H
#define AIR_GAP_DEPLOYMENT_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QDateTime>

/**
 * @brief Air-Gap Deployment Package Status
 */
enum class PackageStatus {
    Pending,
    Packaging,
    Verifying,
    Ready,
    Failed,
    Deployed
};

/**
 * @brief Air-Gap Deployment Package
 */
struct AirGapPackage {
    QString packageId;
    QString packageName;
    QString version;
    QString packagePath;
    QString checksum;  // SHA-256 checksum
    QString signature;  // GPG signature (if signed)
    PackageStatus status;
    QDateTime createdAt;
    QDateTime verifiedAt;
    QVariantMap metadata;
    QStringList dependencies;
    qint64 size;  // Package size in bytes
};

/**
 * @brief Air-Gap Deployment Manager
 * 
 * Manages packaging, verification, and deployment of MilOS in air-gapped environments.
 * Required for military tactical systems that must operate offline.
 * 
 * Features:
 * - Package entire system for air-gap deployment
 * - Verify integrity of offline packages
 * - Manage updates in air-gapped environments
 * - Offline dependency resolution
 * - Integrity verification (checksums, signatures)
 */
class AirGapDeploymentManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isPackaging READ isPackaging NOTIFY packagingStatusChanged)
    Q_PROPERTY(QStringList availablePackages READ availablePackages NOTIFY packagesChanged)

public:
    explicit AirGapDeploymentManager(QObject* parent = nullptr);
    ~AirGapDeploymentManager();

    /**
     * @brief Initialize deployment manager
     * @param packageStoragePath Path to store deployment packages
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const QString& packageStoragePath = "/var/lib/milos/air-gap-packages");

    /**
     * @brief Check if currently packaging
     * @return true if packaging in progress
     */
    bool isPackaging() const { return m_isPackaging; }

    /**
     * @brief Load existing packages from storage
     */
    void loadExistingPackages();

    /**
     * @brief Get list of available packages
     * @return List of package IDs
     */
    QStringList availablePackages() const;

    /**
     * @brief Create air-gap deployment package
     * @param packageName Name of the package
     * @param version Package version
     * @param includeServices List of services to include (empty = all)
     * @param includeDependencies Include all dependencies
     * @return Package ID if successful, empty string otherwise
     */
    QString createDeploymentPackage(const QString& packageName,
                                   const QString& version,
                                   const QStringList& includeServices = QStringList(),
                                   bool includeDependencies = true);

    /**
     * @brief Verify package integrity
     * @param packageId Package ID to verify
     * @return true if package is valid, false otherwise
     */
    bool verifyPackage(const QString& packageId);

    /**
     * @brief Get package information
     * @param packageId Package ID
     * @return Package information or empty if not found
     */
    AirGapPackage getPackageInfo(const QString& packageId) const;

    /**
     * @brief Deploy package to air-gapped system
     * @param packageId Package ID to deploy
     * @param targetPath Target deployment path
     * @return true if deployment successful, false otherwise
     */
    bool deployPackage(const QString& packageId, const QString& targetPath = "/");

    /**
     * @brief Create update package for air-gapped system
     * @param fromVersion Source version
     * @param toVersion Target version
     * @param updateComponents List of components to update
     * @return Package ID if successful, empty string otherwise
     */
    QString createUpdatePackage(const QString& fromVersion,
                               const QString& toVersion,
                               const QStringList& updateComponents = QStringList());

    /**
     * @brief Verify package signature
     * @param packageId Package ID
     * @return true if signature valid, false otherwise
     */
    bool verifyPackageSignature(const QString& packageId) const;

    /**
     * @brief Generate package checksum
     * @param packagePath Path to package file
     * @return SHA-256 checksum as hex string
     */
    QString generatePackageChecksum(const QString& packagePath) const;

Q_SIGNALS:
    /**
     * @brief Emitted when packaging status changes
     */
    void packagingStatusChanged(bool isPackaging);

    /**
     * @brief Emitted when package list changes
     */
    void packagesChanged();

    /**
     * @brief Emitted when package creation progress updates
     * @param progress Progress percentage (0-100)
     * @param message Progress message
     */
    void packageProgress(int progress, const QString& message);

    /**
     * @brief Emitted when package creation completes
     * @param packageId Package ID
     * @param success Whether creation was successful
     */
    void packageCreated(const QString& packageId, bool success);

    /**
     * @brief Emitted when package verification completes
     * @param packageId Package ID
     * @param valid Whether package is valid
     */
    void packageVerified(const QString& packageId, bool valid);

private:
    bool m_initialized;
    bool m_isPackaging;
    QString m_packageStoragePath;
    QMap<QString, AirGapPackage> m_packages;

    /**
     * @brief Collect all system components for packaging
     * @param includeServices List of services to include
     * @return List of component paths
     */
    QStringList collectComponents(const QStringList& includeServices) const;

    /**
     * @brief Resolve dependencies for components
     * @param components List of components
     * @return List of dependency paths
     */
    QStringList resolveDependencies(const QStringList& components) const;

    /**
     * @brief Create package archive
     * @param packageId Package ID
     * @param components List of components to package
     * @return true if successful, false otherwise
     */
    bool createPackageArchive(const QString& packageId, const QStringList& components);

    /**
     * @brief Generate package manifest
     * @param packageId Package ID
     * @param components List of components
     * @return Manifest JSON string
     */
    QString generatePackageManifest(const QString& packageId, const QStringList& components) const;

    /**
     * @brief Sign package with GPG
     * @param packagePath Path to package file
     * @return true if signing successful, false otherwise
     */
    bool signPackage(const QString& packagePath) const;
};

#endif // AIR_GAP_DEPLOYMENT_MANAGER_H

