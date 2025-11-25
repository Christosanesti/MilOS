#include "packagemanager.h"
#include <QProcess>
#include <QTimer>
#include <QRegularExpression>
#include <QDebug>

PackageManager::PackageManager(QObject *parent)
    : QObject(parent)
    , m_installing(false)
    , m_installationProgress(0.0)
    , m_packagesInstalled(0)
    , m_totalPackages(100)
    , m_currentPackage("")
    , m_process(nullptr)
    , m_paused(false)
{
}

void PackageManager::startInstallation()
{
    if (m_installing) {
        return;
    }
    
    m_installing = true;
    m_installationProgress = 0.0;
    m_packagesInstalled = 0;
    m_paused = false;
    emit installingChanged();
    emit installationProgressChanged();
    emit packagesInstalledChanged();
    
    // Detect package manager
    detectPackageManager();
    
    if (m_packageManager.isEmpty()) {
        emit installationError("No supported package manager found (pacman or apt required)");
        m_installing = false;
        emit installingChanged();
        return;
    }
    
    // Get package list (for Arch, this would be from a package list file)
    // For now, we'll install base system packages
    if (m_packageManager == "pacman") {
        m_packageList = QStringList() << "base" << "base-devel" << "linux" << "linux-firmware";
    } else if (m_packageManager == "apt") {
        m_packageList = QStringList() << "ubuntu-minimal" << "linux-generic";
    }
    
    m_totalPackages = m_packageList.size();
    emit totalPackagesChanged();
    
    // Start installation process
    if (!m_process) {
        m_process = new QProcess(this);
        connect(m_process, &QProcess::readyReadStandardOutput, this, &PackageManager::updateProgress);
        connect(m_process, &QProcess::readyReadStandardError, this, &PackageManager::updateProgress);
        connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                [this](int exitCode, QProcess::ExitStatus exitStatus) {
            if (exitStatus == QProcess::NormalExit && exitCode == 0) {
                m_installationProgress = 100.0;
                m_packagesInstalled = m_totalPackages;
                emit installationProgressChanged();
                emit packagesInstalledChanged();
                m_installing = false;
                emit installingChanged();
                emit installationComplete();
            } else {
                emit installationError(QString("Installation failed with exit code %1").arg(exitCode));
                m_installing = false;
                emit installingChanged();
            }
        });
    }
    
    // Start package installation
    if (m_packageManager == "pacman") {
        m_process->start("pacman", QStringList() << "-S" << "--noconfirm" << m_packageList);
    } else if (m_packageManager == "apt") {
        m_process->start("apt", QStringList() << "install" << "-y" << m_packageList);
    }
}

void PackageManager::pauseInstallation()
{
    if (!m_installing || m_paused || !m_process) {
        return;
    }
    
    // Pause the process (send SIGSTOP)
    m_process->kill(); // Note: QProcess doesn't support pause directly, so we'll need to handle this differently
    m_paused = true;
    // For a real implementation, we'd need to use system signals or process groups
}

void PackageManager::resumeInstallation()
{
    if (!m_installing || !m_paused || !m_process) {
        return;
    }
    
    // Resume the process (send SIGCONT)
    // For a real implementation, we'd need to use system signals
    m_paused = false;
    // Since QProcess doesn't support pause/resume directly, we'd need to restart from checkpoint
}

void PackageManager::cancelInstallation()
{
    if (!m_installing) {
        return;
    }
    
    // Cancel ongoing installation
    if (m_process && m_process->state() == QProcess::Running) {
        m_process->kill();
        m_process->waitForFinished(5000);
    }
    
    m_installing = false;
    m_installationProgress = 0.0;
    m_packagesInstalled = 0;
    m_paused = false;
    emit installingChanged();
    emit installationProgressChanged();
    emit packagesInstalledChanged();
}

QString PackageManager::estimatedTimeRemaining()
{
    if (m_packagesInstalled == 0 || m_totalPackages == 0) {
        return "Calculating...";
    }
    
    // Estimate based on progress
    double remainingPackages = m_totalPackages - m_packagesInstalled;
    double avgTimePerPackage = 30.0; // seconds (rough estimate)
    double secondsRemaining = remainingPackages * avgTimePerPackage;
    
    int minutes = static_cast<int>(secondsRemaining / 60);
    int seconds = static_cast<int>(secondsRemaining) % 60;
    
    if (minutes > 0) {
        return QString("%1 minute%2 %3 second%4")
               .arg(minutes)
               .arg(minutes != 1 ? "s" : "")
               .arg(seconds)
               .arg(seconds != 1 ? "s" : "");
    } else {
        return QString("%1 second%2").arg(seconds).arg(seconds != 1 ? "s" : "");
    }
}

void PackageManager::detectPackageManager()
{
    QProcess process;
    process.start("which", QStringList() << "pacman");
    process.waitForFinished(1000);
    if (process.exitCode() == 0) {
        m_packageManager = "pacman";
        return;
    }
    
    process.start("which", QStringList() << "apt");
    process.waitForFinished(1000);
    if (process.exitCode() == 0) {
        m_packageManager = "apt";
        return;
    }
    
    m_packageManager = "";
}

void PackageManager::parsePackageList()
{
    // Parse package list from output
    // This would parse the package manager output to extract package names
}

void PackageManager::updateProgress()
{
    if (!m_process) {
        return;
    }
    
    QString output = m_process->readAllStandardOutput();
    
    // Parse progress from output
    // For pacman: ":: Installing package-name..."
    // For apt: "Setting up package-name..."
    QRegularExpression pacmanRegex(R"(:: Installing (\S+))");
    QRegularExpression aptRegex(R"(Setting up (\S+))");
    
    QRegularExpressionMatch match;
    if (m_packageManager == "pacman") {
        match = pacmanRegex.match(output);
    } else if (m_packageManager == "apt") {
        match = aptRegex.match(output);
    }
    
    if (match.hasMatch()) {
        m_currentPackage = match.captured(1);
        emit currentPackageChanged();
        m_packagesInstalled++;
        m_installationProgress = (static_cast<double>(m_packagesInstalled) / static_cast<double>(m_totalPackages)) * 100.0;
        emit packagesInstalledChanged();
        emit installationProgressChanged();
    }
}

