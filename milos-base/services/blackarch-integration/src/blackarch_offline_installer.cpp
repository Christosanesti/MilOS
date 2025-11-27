#include "blackarch_offline_installer.h"
#include <milos/logging/logger.h>
#include <QProcess>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

BlackArchOfflineInstaller::BlackArchOfflineInstaller(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_defaultMirrorPath("/var/milos/blackarch-mirrors/")
{
}

BlackArchOfflineInstaller::~BlackArchOfflineInstaller() {
}

bool BlackArchOfflineInstaller::initialize() {
    if (m_initialized) {
        return true;
    }

    // Create default mirror directory
    QDir mirrorDir(m_defaultMirrorPath);
    if (!mirrorDir.exists()) {
        if (!mirrorDir.mkpath(".")) {
            LOG_ERROR(QString("Failed to create mirror directory: %1").arg(m_defaultMirrorPath).toStdString());
            return false;
        }
    }

    m_initialized = true;
    return true;
}

bool BlackArchOfflineInstaller::createMirror(const QStringList& toolNames, const QString& mirrorPath) {
    emit mirrorCreationStarted(mirrorPath);

    // Create mirror directory
    QDir mirrorDir(mirrorPath);
    if (!mirrorDir.exists()) {
        if (!mirrorDir.mkpath(".")) {
            std::cerr << "Failed to create mirror directory: " << mirrorPath.toStdString() << std::endl;
            emit mirrorCreationCompleted(mirrorPath, false);
            return false;
        }
    }

    // Download packages using pacman
    // Note: In production, this would use rsync or similar to sync repository
    // For now, we'll use pacman to download packages
    
    QStringList downloadArgs;
    downloadArgs << "-Sw" << "--noconfirm";
    downloadArgs << toolNames;

    QProcess process;
    process.setProgram("pacman");
    process.setArguments(downloadArgs);
    process.start();
    process.waitForFinished();

    if (process.exitCode() != 0) {
        std::cerr << "Failed to download packages" << std::endl;
        emit mirrorCreationCompleted(mirrorPath, false);
        return false;
    }

    // Copy downloaded packages to mirror directory
    // Packages are typically in /var/cache/pacman/pkg/
    QString cachePath = "/var/cache/pacman/pkg/";
    QDir cacheDir(cachePath);
    
    if (cacheDir.exists()) {
        QStringList filters;
        filters << "*.pkg.tar.*";
        QFileInfoList files = cacheDir.entryInfoList(filters, QDir::Files);
        
        for (const QFileInfo& fileInfo : files) {
            // Check if this package is one of the requested tools
            QString fileName = fileInfo.fileName();
            bool isRequested = false;
            for (const QString& toolName : toolNames) {
                if (fileName.contains(toolName)) {
                    isRequested = true;
                    break;
                }
            }
            
            if (isRequested) {
                QString destPath = mirrorPath + "/" + fileName;
                if (QFile::copy(fileInfo.absoluteFilePath(), destPath)) {
                    // Package copied successfully
                }
            }
        }
    }

    emit mirrorCreationCompleted(mirrorPath, true);
    return true;
}

bool BlackArchOfflineInstaller::installFromMirror(const QString& mirrorPath, const QStringList& toolNames) {
    if (!mirrorExists(mirrorPath)) {
        std::cerr << "Mirror does not exist: " << mirrorPath.toStdString() << std::endl;
        return false;
    }

    // Create local repository database
    QStringList repoArgs;
    repoArgs << "-b" << mirrorPath << mirrorPath;
    
    QProcess repoProcess;
    repoProcess.setProgram("repo-add");
    repoProcess.setArguments(repoArgs);
    repoProcess.start();
    repoProcess.waitForFinished();

    if (repoProcess.exitCode() != 0) {
        std::cerr << "Failed to create repository database" << std::endl;
        return false;
    }

    // Install packages from local repository
    QStringList installArgs;
    installArgs << "-U" << "--noconfirm";
    
    QDir mirrorDir(mirrorPath);
    QStringList filters;
    filters << "*.pkg.tar.*";
    QFileInfoList files = mirrorDir.entryInfoList(filters, QDir::Files);
    
    for (const QFileInfo& fileInfo : files) {
        QString fileName = fileInfo.fileName();
        bool isRequested = false;
        for (const QString& toolName : toolNames) {
            if (fileName.contains(toolName)) {
                isRequested = true;
                break;
            }
        }
        
        if (isRequested) {
            installArgs << fileInfo.absoluteFilePath();
        }
    }

    QProcess installProcess;
    installProcess.setProgram("pacman");
    installProcess.setArguments(installArgs);
    installProcess.start();
    installProcess.waitForFinished();

    return installProcess.exitCode() == 0;
}

bool BlackArchOfflineInstaller::mirrorExists(const QString& mirrorPath) const {
    QDir mirrorDir(mirrorPath);
    return mirrorDir.exists();
}

qint64 BlackArchOfflineInstaller::getMirrorSize(const QString& mirrorPath) const {
    if (!mirrorExists(mirrorPath)) {
        return -1;
    }

    qint64 totalSize = 0;
    QDir mirrorDir(mirrorPath);
    QFileInfoList files = mirrorDir.entryInfoList(QDir::Files);
    
    for (const QFileInfo& fileInfo : files) {
        totalSize += fileInfo.size();
    }
    
    return totalSize;
}

bool BlackArchOfflineInstaller::executeRsync(const QStringList& arguments) {
    QProcess process;
    process.setProgram("rsync");
    process.setArguments(arguments);
    process.start();
    process.waitForFinished();
    return process.exitCode() == 0;
}

bool BlackArchOfflineInstaller::executePacmanLocal(const QStringList& arguments, const QString& mirrorPath) {
    QProcess process;
    process.setProgram("pacman");
    process.setArguments(arguments);
    process.setWorkingDirectory(mirrorPath);
    process.start();
    process.waitForFinished();
    return process.exitCode() == 0;
}

