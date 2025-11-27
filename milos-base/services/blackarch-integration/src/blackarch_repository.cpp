#include "blackarch_repository.h"
#include <milos/logging/logger.h>
#include <QProcess>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>

BlackArchRepository::BlackArchRepository(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_repositoryConfigured(false)
    , m_repositoryUrl("https://blackarch.org/repo/")
    , m_mirrorPath("/var/milos/blackarch-mirrors/")
{
}

BlackArchRepository::~BlackArchRepository() {
}

bool BlackArchRepository::initialize() {
    if (m_initialized) {
        return true;
    }

    if (!checkPacmanAvailable()) {
        LOG_ERROR("pacman is not available");
        return false;
    }

    // Check if repository is already configured
    m_repositoryConfigured = isRepositoryConfigured();

    m_initialized = true;
    return true;
}

bool BlackArchRepository::checkPacmanAvailable() const {
    QProcess process;
    process.start("which", QStringList() << "pacman");
    process.waitForFinished();
    return process.exitCode() == 0;
}

bool BlackArchRepository::isRepositoryConfigured() const {
    // Check if BlackArch repository is in pacman.conf
    QFile pacmanConf("/etc/pacman.conf");
    if (!pacmanConf.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&pacmanConf);
    QString content = in.readAll();
    pacmanConf.close();

    return content.contains("[blackarch]");
}

bool BlackArchRepository::configureRepository() {
    if (m_repositoryConfigured) {
        return true;
    }

    // Install BlackArch repository keyring
    QStringList keyringArgs;
    keyringArgs << "-S" << "--noconfirm" << "blackarch-keyring";
    
    if (!executePacman(keyringArgs)) {
        LOG_ERROR("Failed to install BlackArch keyring");
        return false;
    }

    // Add BlackArch repository to pacman.conf
    QFile pacmanConf("/etc/pacman.conf");
    if (!pacmanConf.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)) {
        LOG_ERROR("Failed to open pacman.conf");
        return false;
    }

    QTextStream out(&pacmanConf);
    out << "\n[blackarch]\n";
    out << "Include = /etc/pacman.d/blackarch-mirrorlist\n";
    pacmanConf.close();

    // Create mirrorlist if it doesn't exist
    QDir mirrorDir("/etc/pacman.d");
    if (!mirrorDir.exists()) {
        mirrorDir.mkpath(".");
    }

    QFile mirrorlist("/etc/pacman.d/blackarch-mirrorlist");
    if (!mirrorlist.exists()) {
        if (mirrorlist.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream mirrorOut(&mirrorlist);
            mirrorOut << "Server = " << m_repositoryUrl << "$repo/os/$arch\n";
            mirrorlist.close();
        }
    }

    // Update package database
    QStringList updateArgs;
    updateArgs << "-Sy";
    if (!executePacman(updateArgs)) {
        LOG_ERROR("Failed to update package database");
        return false;
    }

    m_repositoryConfigured = true;
    return true;
}

QStringList BlackArchRepository::getAvailableTools() const {
    QStringList tools;
    
    // Query BlackArch packages
    QString output;
    QStringList args;
    args << "-Ss" << "blackarch";
    
    if (executePacman(args, &output)) {
        // Parse output to extract tool names
        QStringList lines = output.split('\n');
        for (const QString& line : lines) {
            if (line.startsWith("blackarch/")) {
                QString toolName = line.split('/').value(1).split(' ').value(0);
                if (!toolName.isEmpty() && !tools.contains(toolName)) {
                    tools.append(toolName);
                }
            }
        }
    }

    return tools;
}

bool BlackArchRepository::isToolInstalled(const QString& toolName) const {
    QStringList args;
    args << "-Q" << toolName;
    
    QString output;
    if (executePacman(args, &output)) {
        return !output.isEmpty();
    }
    
    return false;
}

bool BlackArchRepository::installTool(const QString& toolName) {
    emit toolInstallationStarted(toolName);

    QStringList args;
    args << "-S" << "--noconfirm" << toolName;

    bool success = executePacman(args);
    
    emit toolInstallationCompleted(toolName, success);
    return success;
}

bool BlackArchRepository::installTools(const QStringList& toolNames) {
    bool allSuccess = true;
    
    for (const QString& toolName : toolNames) {
        if (!installTool(toolName)) {
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

bool BlackArchRepository::uninstallTool(const QString& toolName) {
    QStringList args;
    args << "-R" << "--noconfirm" << toolName;
    return executePacman(args);
}

bool BlackArchRepository::updateTool(const QString& toolName) {
    QStringList args;
    args << "-S" << "--noconfirm" << toolName;
    return executePacman(args);
}

bool BlackArchRepository::updateAllTools() {
    QStringList args;
    args << "-Syu" << "--noconfirm";
    return executePacman(args);
}

bool BlackArchRepository::executePacman(const QStringList& arguments, QString* output) {
    QProcess process;
    process.setProgram("pacman");
    process.setArguments(arguments);
    
    if (output) {
        process.start();
        process.waitForFinished();
        *output = QString::fromUtf8(process.readAllStandardOutput());
        return process.exitCode() == 0;
    } else {
        process.start();
        process.waitForFinished();
        return process.exitCode() == 0;
    }
}

