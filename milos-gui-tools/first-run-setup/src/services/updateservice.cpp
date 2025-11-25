#include "updateservice.h"
#include <QProcess>
#include <QTimer>
#include <QDebug>
#include <QRegularExpression>

UpdateService::UpdateService(QObject *parent)
    : QObject(parent)
    , m_status("up_to_date")
    , m_checking(false)
{
}

void UpdateService::checkForUpdates()
{
    if (m_checking) {
        return;
    }
    
    m_checking = true;
    m_status = "checking";
    emit checkingChanged();
    emit statusChanged();
    
    // Check for updates using pacman (Arch-based systems)
    QProcess *process = new QProcess(this);
    process->setProcessChannelMode(QProcess::MergedChannels);
    
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this, process](int exitCode, QProcess::ExitStatus exitStatus) {
        m_checking = false;
        emit checkingChanged();
        
        if (exitStatus == QProcess::NormalExit && exitCode == 0) {
            QString output = process->readAllStandardOutput();
            
            // Parse pacman output to determine if updates are available
            // pacman -Qu returns non-empty output if updates are available
            if (output.trimmed().isEmpty()) {
                m_status = "up_to_date";
            } else {
                // Count number of packages with updates
                int updateCount = output.split('\n').count() - 1; // Subtract 1 for empty line
                if (updateCount > 0) {
                    m_status = "updates_available";
                    qDebug() << "Updates available for" << updateCount << "packages";
                } else {
                    m_status = "up_to_date";
                }
            }
        } else {
            // Check if pacman is available, if not try other package managers
            QProcess checkProcess;
            checkProcess.start("which", QStringList() << "pacman");
            checkProcess.waitForFinished(1000);
            
            if (checkProcess.exitCode() == 0) {
                // pacman exists but check failed
                m_status = "error";
                qWarning() << "Failed to check for updates:" << process->readAllStandardError();
            } else {
                // Try apt (Debian/Ubuntu)
                QProcess aptProcess;
                aptProcess.start("apt", QStringList() << "list" << "--upgradable");
                aptProcess.waitForFinished(5000);
                
                if (aptProcess.exitCode() == 0) {
                    QString aptOutput = aptProcess.readAllStandardOutput();
                    // apt list --upgradable shows header line, so count lines > 1
                    int lineCount = aptOutput.split('\n').count();
                    if (lineCount > 1) {
                        m_status = "updates_available";
                    } else {
                        m_status = "up_to_date";
                    }
                } else {
                    // Try dnf/yum (Fedora/RHEL)
                    QProcess dnfProcess;
                    dnfProcess.start("dnf", QStringList() << "check-update" << "--quiet");
                    dnfProcess.waitForFinished(10000);
                    
                    if (dnfProcess.exitCode() == 0) {
                        m_status = "up_to_date";
                    } else if (dnfProcess.exitCode() == 100) {
                        // Exit code 100 means updates available
                        m_status = "updates_available";
                    } else {
                        m_status = "error";
                        qWarning() << "Could not determine update status";
                    }
                }
            }
        }
        
        emit statusChanged();
        process->deleteLater();
    });
    
    // Start pacman check
    process->start("pacman", QStringList() << "-Qu");
    
    // Set timeout (30 seconds)
    QTimer::singleShot(30000, [this, process]() {
        if (process->state() == QProcess::Running) {
            process->kill();
            m_checking = false;
            m_status = "error";
            emit checkingChanged();
            emit statusChanged();
            qWarning() << "Update check timed out";
        }
    });
}

bool UpdateService::hasUpdates()
{
    return m_status == "updates_available";
}

