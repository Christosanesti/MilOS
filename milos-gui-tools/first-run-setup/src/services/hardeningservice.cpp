#include "hardeningservice.h"
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QSettings>

HardeningService::HardeningService(QObject *parent)
    : QObject(parent)
    , m_level("basic")
    , m_progress(0.0)
{
}

void HardeningService::checkStatus()
{
    // Check various hardening configurations and calculate progress
    int checksPassed = 0;
    int totalChecks = 0;
    
    // Check 1: ASLR (Address Space Layout Randomization)
    totalChecks++;
    QFile aslrFile("/proc/sys/kernel/randomize_va_space");
    if (aslrFile.exists()) {
        if (aslrFile.open(QIODevice::ReadOnly)) {
            QTextStream stream(&aslrFile);
            QString value = stream.readAll().trimmed();
            if (value == "2") {
                checksPassed++; // Full ASLR enabled
            }
            aslrFile.close();
        }
    }
    
    // Check 2: Kernel module loading restrictions
    totalChecks++;
    QFile modulesFile("/proc/sys/kernel/modules_disabled");
    if (modulesFile.exists()) {
        if (modulesFile.open(QIODevice::ReadOnly)) {
            QTextStream stream(&modulesFile);
            QString value = stream.readAll().trimmed();
            if (value == "1") {
                checksPassed++; // Module loading disabled
            }
            modulesFile.close();
        }
    }
    
    // Check 3: Core dumps disabled
    totalChecks++;
    QFile coreDumpFile("/proc/sys/fs/suid_dumpable");
    if (coreDumpFile.exists()) {
        if (coreDumpFile.open(QIODevice::ReadOnly)) {
            QTextStream stream(&coreDumpFile);
            QString value = stream.readAll().trimmed();
            if (value == "0") {
                checksPassed++; // Core dumps disabled
            }
            coreDumpFile.close();
        }
    }
    
    // Check 4: IP forwarding disabled (unless needed)
    totalChecks++;
    QFile ipForwardFile("/proc/sys/net/ipv4/ip_forward");
    if (ipForwardFile.exists()) {
        if (ipForwardFile.open(QIODevice::ReadOnly)) {
            QTextStream stream(&ipForwardFile);
            QString value = stream.readAll().trimmed();
            if (value == "0") {
                checksPassed++; // IP forwarding disabled
            }
            ipForwardFile.close();
        }
    }
    
    // Check 5: SYN cookies enabled (DDoS protection)
    totalChecks++;
    QFile synCookiesFile("/proc/sys/net/ipv4/tcp_syncookies");
    if (synCookiesFile.exists()) {
        if (synCookiesFile.open(QIODevice::ReadOnly)) {
            QTextStream stream(&synCookiesFile);
            QString value = stream.readAll().trimmed();
            if (value == "1") {
                checksPassed++; // SYN cookies enabled
            }
            synCookiesFile.close();
        }
    }
    
    // Check 6: Firewall active (basic check)
    totalChecks++;
    QProcess process;
    process.start("systemctl", QStringList() << "is-active" << "--quiet" << "firewalld");
    process.waitForFinished(1000);
    if (process.exitCode() == 0) {
        checksPassed++;
    } else {
        // Check ufw
        process.start("systemctl", QStringList() << "is-active" << "--quiet" << "ufw");
        process.waitForFinished(1000);
        if (process.exitCode() == 0) {
            checksPassed++;
        }
    }
    
    // Check 7: SELinux or AppArmor enabled
    totalChecks++;
    QFile selinuxFile("/etc/selinux/config");
    if (selinuxFile.exists()) {
        if (selinuxFile.open(QIODevice::ReadOnly)) {
            QTextStream stream(&selinuxFile);
            QString content = stream.readAll();
            if (content.contains("SELINUX=enforcing") || content.contains("SELINUX=permissive")) {
                checksPassed++;
            }
            selinuxFile.close();
        }
    } else {
        // Check AppArmor
        process.start("systemctl", QStringList() << "is-active" << "--quiet" << "apparmor");
        process.waitForFinished(1000);
        if (process.exitCode() == 0) {
            checksPassed++;
        }
    }
    
    // Check 8: Automatic security updates configured
    totalChecks++;
    QFile updateFile("/etc/systemd/system/timers.target.wants/unattended-upgrades.timer");
    if (updateFile.exists()) {
        checksPassed++;
    } else {
        // Check for other update mechanisms
        QFile pacmanFile("/etc/pacman.d/hooks/security-updates.hook");
        if (pacmanFile.exists()) {
            checksPassed++;
        }
    }
    
    // Calculate progress percentage
    m_progress = (static_cast<double>(checksPassed) / static_cast<double>(totalChecks)) * 100.0;
    
    // Determine level based on progress
    if (m_progress >= 87.5) { // 7/8 checks passed
        m_level = "maximum";
    } else if (m_progress >= 62.5) { // 5/8 checks passed
        m_level = "high";
    } else if (m_progress >= 37.5) { // 3/8 checks passed
        m_level = "standard";
    } else {
        m_level = "basic";
    }
    
    emit levelChanged();
    emit progressChanged();
}

QString HardeningService::getLevel()
{
    return m_level;
}

