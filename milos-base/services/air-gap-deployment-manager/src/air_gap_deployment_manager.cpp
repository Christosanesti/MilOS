#include "air_gap_deployment_manager.h"
#include <milos/logging/logger.h>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDateTime>

AirGapDeploymentManager::AirGapDeploymentManager(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_isPackaging(false)
    , m_packageStoragePath("/var/lib/milos/air-gap-packages")
{
}

AirGapDeploymentManager::~AirGapDeploymentManager() {
}

bool AirGapDeploymentManager::initialize(const QString& packageStoragePath) {
    if (m_initialized) {
        return true;
    }

    m_packageStoragePath = packageStoragePath;

    // Create package storage directory
    QDir storageDir(m_packageStoragePath);
    if (!storageDir.exists()) {
        if (!storageDir.mkpath(".")) {
            LOG_ERROR(QString("Failed to create package storage directory: %1").arg(m_packageStoragePath));
            return false;
        }
    }

    // Load existing packages
    loadExistingPackages();

    m_initialized = true;
    LOG_INFO(QString("Air-Gap Deployment Manager initialized: %1").arg(m_packageStoragePath));
    return true;
}

QStringList AirGapDeploymentManager::availablePackages() const {
    QStringList packages;
    for (auto it = m_packages.begin(); it != m_packages.end(); ++it) {
        packages.append(it.key());
    }
    return packages;
}

QString AirGapDeploymentManager::createDeploymentPackage(const QString& packageName,
                                                         const QString& version,
                                                         const QStringList& includeServices,
                                                         bool includeDependencies) {
    if (m_isPackaging) {
        LOG_WARNING("Package creation already in progress");
        return QString();
    }

    m_isPackaging = true;
    emit packagingStatusChanged(true);

    // Generate package ID
    QString packageId = QString("%1-%2-%3")
                       .arg(packageName)
                       .arg(version)
                       .arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss"));

    emit packageProgress(10, "Collecting components...");

    // Collect components
    QStringList components = collectComponents(includeServices);
    if (components.isEmpty()) {
        LOG_ERROR("No components found to package");
        m_isPackaging = false;
        emit packagingStatusChanged(false);
        return QString();
    }

    emit packageProgress(30, "Resolving dependencies...");

    // Resolve dependencies if requested
    if (includeDependencies) {
        QStringList dependencies = resolveDependencies(components);
        components.append(dependencies);
    }

    emit packageProgress(50, "Creating package archive...");

    // Create package archive
    if (!createPackageArchive(packageId, components)) {
        LOG_ERROR("Failed to create package archive");
        m_isPackaging = false;
        emit packagingStatusChanged(false);
        return QString();
    }

    emit packageProgress(70, "Generating manifest...");

    // Generate manifest
    QString manifest = generatePackageManifest(packageId, components);
    QString manifestPath = QString("%1/%2.manifest.json").arg(m_packageStoragePath).arg(packageId);
    QFile manifestFile(manifestPath);
    if (!manifestFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        LOG_ERROR(QString("Failed to write manifest: %1").arg(manifestPath));
        m_isPackaging = false;
        emit packagingStatusChanged(false);
        return QString();
    }
    manifestFile.write(manifest.toUtf8());
    manifestFile.close();

    emit packageProgress(80, "Generating checksum...");

    // Generate checksum
    QString packagePath = QString("%1/%2.tar.gz").arg(m_packageStoragePath).arg(packageId);
    QString checksum = generatePackageChecksum(packagePath);

    emit packageProgress(90, "Signing package...");

    // Sign package
    if (!signPackage(packagePath)) {
        LOG_WARNING("Package signing failed, continuing without signature");
    }

    emit packageProgress(100, "Package creation complete");

    // Create package record
    AirGapPackage package;
    package.packageId = packageId;
    package.packageName = packageName;
    package.version = version;
    package.packagePath = packagePath;
    package.checksum = checksum;
    package.status = PackageStatus::Ready;
    package.createdAt = QDateTime::currentDateTime();
    package.size = QFileInfo(packagePath).size();
    package.dependencies = includeDependencies ? resolveDependencies(components) : QStringList();

    m_packages[packageId] = package;

    m_isPackaging = false;
    emit packagingStatusChanged(false);
    emit packageCreated(packageId, true);
    emit packagesChanged();

    LOG_INFO(QString("Deployment package created: %1").arg(packageId));
    return packageId;
}

bool AirGapDeploymentManager::verifyPackage(const QString& packageId) {
    if (!m_packages.contains(packageId)) {
        LOG_ERROR(QString("Package not found: %1").arg(packageId));
        return false;
    }

    AirGapPackage package = m_packages[packageId];

    // Verify package file exists
    if (!QFileInfo::exists(package.packagePath)) {
        LOG_ERROR(QString("Package file not found: %1").arg(package.packagePath));
        return false;
    }

    // Verify checksum
    QString calculatedChecksum = generatePackageChecksum(package.packagePath);
    if (calculatedChecksum != package.checksum) {
        LOG_ERROR(QString("Package checksum mismatch: %1").arg(packageId));
        return false;
    }

    // Verify signature if present
    if (!package.signature.isEmpty()) {
        if (!verifyPackageSignature(packageId)) {
            LOG_ERROR(QString("Package signature invalid: %1").arg(packageId));
            return false;
        }
    }

    // Update verification timestamp
    package.verifiedAt = QDateTime::currentDateTime();
    package.status = PackageStatus::Ready;
    m_packages[packageId] = package;

    emit packageVerified(packageId, true);
    LOG_INFO(QString("Package verified: %1").arg(packageId));
    return true;
}

AirGapPackage AirGapDeploymentManager::getPackageInfo(const QString& packageId) const {
    if (m_packages.contains(packageId)) {
        return m_packages[packageId];
    }
    return AirGapPackage();
}

bool AirGapDeploymentManager::deployPackage(const QString& packageId, const QString& targetPath) {
    if (!m_packages.contains(packageId)) {
        LOG_ERROR(QString("Package not found: %1").arg(packageId));
        return false;
    }

    AirGapPackage package = m_packages[packageId];

    // Verify package before deployment
    if (!verifyPackage(packageId)) {
        LOG_ERROR(QString("Package verification failed: %1").arg(packageId));
        return false;
    }

    LOG_INFO(QString("Deploying package %1 to %2").arg(packageId).arg(targetPath));

    // Extract package
    QProcess tarProcess;
    tarProcess.setProgram("tar");
    tarProcess.setArguments({"-xzf", package.packagePath, "-C", targetPath});
    tarProcess.start();
    
    if (!tarProcess.waitForFinished(300000)) {  // 5 minute timeout
        LOG_ERROR("Package extraction timeout");
        return false;
    }

    if (tarProcess.exitCode() != 0) {
        LOG_ERROR(QString("Package extraction failed: %1").arg(QString::fromUtf8(tarProcess.readAllStandardError())));
        return false;
    }

    // Update package status
    package.status = PackageStatus::Deployed;
    m_packages[packageId] = package;

    LOG_INFO(QString("Package deployed successfully: %1").arg(packageId));
    return true;
}

QString AirGapDeploymentManager::createUpdatePackage(const QString& fromVersion,
                                                      const QString& toVersion,
                                                      const QStringList& updateComponents) {
    // Create update package (delta update)
    QString packageName = QString("milos-update-%1-to-%2").arg(fromVersion).arg(toVersion);
    return createDeploymentPackage(packageName, toVersion, updateComponents, true);
}

bool AirGapDeploymentManager::verifyPackageSignature(const QString& packageId) const {
    if (!m_packages.contains(packageId)) {
        return false;
    }

    AirGapPackage package = m_packages[packageId];
    QString sigPath = QString("%1.sig").arg(package.packagePath);

    if (!QFileInfo::exists(sigPath)) {
        return false;  // No signature file
    }

    // Verify GPG signature
    QProcess gpgProcess;
    gpgProcess.setProgram("gpg");
    gpgProcess.setArguments({"--verify", sigPath, package.packagePath});
    gpgProcess.start();
    
    if (!gpgProcess.waitForFinished(30000)) {  // 30 second timeout
        return false;
    }

    return gpgProcess.exitCode() == 0;
}

QString AirGapDeploymentManager::generatePackageChecksum(const QString& packagePath) const {
    QFile file(packagePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }

    QCryptographicHash hash(QCryptographicHash::Sha256);
    if (!hash.addData(&file)) {
        return QString();
    }

    return QString::fromLatin1(hash.result().toHex());
}

QStringList AirGapDeploymentManager::collectComponents(const QStringList& includeServices) const {
    QStringList components;

    // Base system components
    QString basePath = "/usr/bin/milos-";
    QDir baseDir("/usr/bin");
    QStringList filters;
    filters << "milos-*";
    QFileInfoList files = baseDir.entryInfoList(filters, QDir::Files | QDir::Executable);
    for (const QFileInfo& file : files) {
        components.append(file.absoluteFilePath());
    }

    // Service executables
    QString servicesPath = "/usr/bin";
    if (includeServices.isEmpty()) {
        // Include all services
        components.append(QString("%1/milos-data-guard-daemon").arg(servicesPath));
        components.append(QString("%1/milos-audit-daemon").arg(servicesPath));
        // Add other services as needed
    } else {
        // Include specified services only
        for (const QString& service : includeServices) {
            components.append(QString("%1/milos-%1-daemon").arg(servicesPath).arg(service));
        }
    }

    // Configuration files
    components.append("/etc/milos/data-guard/config.yaml");
    // Add other config files as needed

    // Libraries
    QString libPath = "/usr/lib";
    QDir libDir(libPath);
    QStringList libFilters;
    libFilters << "libmilos*.so*";
    QFileInfoList libs = libDir.entryInfoList(libFilters, QDir::Files);
    for (const QFileInfo& lib : libs) {
        components.append(lib.absoluteFilePath());
    }

    // Filter out non-existent files
    QStringList validComponents;
    for (const QString& component : components) {
        if (QFileInfo::exists(component)) {
            validComponents.append(component);
        }
    }

    return validComponents;
}

QStringList AirGapDeploymentManager::resolveDependencies(const QStringList& components) const {
    QStringList dependencies;

    // Use ldd to find shared library dependencies
    for (const QString& component : components) {
        QFileInfo info(component);
        if (info.isExecutable() || info.suffix() == "so") {
            QProcess lddProcess;
            lddProcess.setProgram("ldd");
            lddProcess.setArguments({component});
            lddProcess.start();
            
            if (lddProcess.waitForFinished(5000)) {
                QByteArray output = lddProcess.readAllStandardOutput();
                QStringList lines = QString::fromUtf8(output).split('\n');
                for (const QString& line : lines) {
                    // Parse ldd output to extract library paths
                    // Format: "libname => /path/to/lib (0x...)"
                    if (line.contains("=>")) {
                        QStringList parts = line.split("=>");
                        if (parts.size() == 2) {
                            QString libPath = parts[1].split(' ').first().trimmed();
                            if (QFileInfo::exists(libPath) && !dependencies.contains(libPath)) {
                                dependencies.append(libPath);
                            }
                        }
                    }
                }
            }
        }
    }

    return dependencies;
}

bool AirGapDeploymentManager::createPackageArchive(const QString& packageId, const QStringList& components) {
    QString packagePath = QString("%1/%2.tar.gz").arg(m_packageStoragePath).arg(packageId);

    // Create temporary directory for packaging
    QString tempDir = QString("%1/%2-temp").arg(m_packageStoragePath).arg(packageId);
    QDir().mkpath(tempDir);

    // Copy components to temp directory preserving structure
    for (const QString& component : components) {
        QFileInfo info(component);
        QString destPath = QString("%1%2").arg(tempDir).arg(component);
        QDir().mkpath(QFileInfo(destPath).absolutePath());
        
        if (!QFile::copy(component, destPath)) {
            LOG_ERROR(QString("Failed to copy component: %1").arg(component));
            // Clean up
            QDir(tempDir).removeRecursively();
            return false;
        }
    }

    // Create tar.gz archive
    QProcess tarProcess;
    tarProcess.setProgram("tar");
    tarProcess.setArguments({"-czf", packagePath, "-C", tempDir, "."});
    tarProcess.start();
    
    if (!tarProcess.waitForFinished(600000)) {  // 10 minute timeout
        LOG_ERROR("Package archive creation timeout");
        QDir(tempDir).removeRecursively();
        return false;
    }

    if (tarProcess.exitCode() != 0) {
        LOG_ERROR(QString("Package archive creation failed: %1")
                 .arg(QString::fromUtf8(tarProcess.readAllStandardError())));
        QDir(tempDir).removeRecursively();
        return false;
    }

    // Clean up temp directory
    QDir(tempDir).removeRecursively();

    return true;
}

QString AirGapDeploymentManager::generatePackageManifest(const QString& packageId, const QStringList& components) const {
    QJsonObject manifest;
    manifest["package_id"] = packageId;
    manifest["created_at"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    manifest["format_version"] = "1.0";
    
    QJsonArray componentsArray;
    for (const QString& component : components) {
        QFileInfo info(component);
        QJsonObject compObj;
        compObj["path"] = component;
        compObj["size"] = static_cast<qint64>(info.size());
        compObj["modified"] = info.lastModified().toString(Qt::ISODate);
        compObj["checksum"] = generatePackageChecksum(component);
        componentsArray.append(compObj);
    }
    manifest["components"] = componentsArray;

    QJsonDocument doc(manifest);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
}

bool AirGapDeploymentManager::signPackage(const QString& packagePath) const {
    QString sigPath = QString("%1.sig").arg(packagePath);

    // Sign with GPG (requires GPG key to be configured)
    QProcess gpgProcess;
    gpgProcess.setProgram("gpg");
    gpgProcess.setArguments({"--detach-sign", "--armor", packagePath});
    gpgProcess.start();
    
    if (!gpgProcess.waitForFinished(30000)) {  // 30 second timeout
        return false;
    }

    return gpgProcess.exitCode() == 0;
}

void AirGapDeploymentManager::loadExistingPackages() {
    // Load package metadata from storage directory
    QDir storageDir(m_packageStoragePath);
    QStringList filters;
    filters << "*.manifest.json";
    QFileInfoList manifests = storageDir.entryInfoList(filters, QDir::Files);

    for (const QFileInfo& manifestFile : manifests) {
        QFile file(manifestFile.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            continue;
        }

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            continue;
        }

        QJsonObject manifest = doc.object();
        QString packageId = manifest["package_id"].toString();
        
        if (!packageId.isEmpty()) {
            AirGapPackage package;
            package.packageId = packageId;
            package.packageName = manifest["package_name"].toString();
            package.version = manifest["version"].toString();
            package.packagePath = QString("%1/%2.tar.gz").arg(m_packageStoragePath).arg(packageId);
            package.status = QFileInfo::exists(package.packagePath) ? PackageStatus::Ready : PackageStatus::Failed;
            package.createdAt = QDateTime::fromString(manifest["created_at"].toString(), Qt::ISODate);
            
            m_packages[packageId] = package;
        }
    }
}

