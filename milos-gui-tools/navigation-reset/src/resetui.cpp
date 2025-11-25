#include "resetui.h"
#include <QDebug>
#include <QTimer>

NavigationResetUI::NavigationResetUI(QObject *parent)
    : QObject(parent)
    , m_resetInProgress(false)
    , m_resetProgress(0.0)
    , m_resetInterface(nullptr)
{
    // Connect to D-Bus signals
    QDBusConnection sessionBus = QDBusConnection::sessionBus();
    sessionBus.connect("org.milos.NavigationReset",
                       "/org/milos/NavigationReset",
                       "org.milos.NavigationReset",
                       "ResetStarted",
                       this,
                       SLOT(onResetStarted(QString, QString)));
    
    sessionBus.connect("org.milos.NavigationReset",
                       "/org/milos/NavigationReset",
                       "org.milos.NavigationReset",
                       "ResetCompleted",
                       this,
                       SLOT(onResetCompleted(QString, QString)));
    
    sessionBus.connect("org.milos.NavigationReset",
                       "/org/milos/NavigationReset",
                       "org.milos.NavigationReset",
                       "BackupCreated",
                       this,
                       SLOT(onBackupCreated(QString)));
    
    loadBackups();
}

void NavigationResetUI::previewReset(const QString &resetType)
{
    QDBusInterface* iface = getResetInterface();
    if (!iface || !iface->isValid()) {
        emit resetError("Navigation reset service not available");
        return;
    }
    
    QDBusReply<QVariantMap> reply = iface->call("ValidateReset", resetType);
    if (reply.isValid()) {
        m_resetPreview = reply.value();
        emit resetPreviewChanged();
    } else {
        emit resetError(QString("Failed to preview reset: %1").arg(reply.error().message()));
    }
}

void NavigationResetUI::executeReset(const QString &resetType)
{
    if (m_resetInProgress) {
        emit resetError("Reset already in progress");
        return;
    }
    
    QDBusInterface* iface = getResetInterface();
    if (!iface || !iface->isValid()) {
        emit resetError("Navigation reset service not available");
        return;
    }
    
    m_resetInProgress = true;
    m_resetProgress = 0.0;
    emit resetInProgressChanged();
    emit resetProgressChanged();
    
    // Start reset operation
    QDBusReply<QVariantMap> reply = iface->call("ResetNavigation", resetType);
    if (reply.isValid()) {
        QVariantMap result = reply.value();
        m_currentResetId = result.value("reset_id").toString();
        
        // Simulate progress updates
        QTimer* progressTimer = new QTimer(this);
        connect(progressTimer, &QTimer::timeout, [this, progressTimer]() {
            if (m_resetInProgress && m_resetProgress < 90.0) {
                m_resetProgress += 10.0;
                emit resetProgressChanged();
            } else {
                progressTimer->stop();
                progressTimer->deleteLater();
            }
        });
        progressTimer->start(500);
    } else {
        m_resetInProgress = false;
        emit resetInProgressChanged();
        emit resetError(QString("Failed to start reset: %1").arg(reply.error().message()));
    }
}

void NavigationResetUI::cancelReset()
{
    // Note: The D-Bus service doesn't support cancellation, but we can stop progress updates
    if (m_resetInProgress) {
        m_resetInProgress = false;
        m_resetProgress = 0.0;
        emit resetInProgressChanged();
        emit resetProgressChanged();
    }
}

void NavigationResetUI::loadBackups()
{
    QDBusInterface* iface = getResetInterface();
    if (!iface || !iface->isValid()) {
        return;
    }
    
    QDBusReply<QVariantList> reply = iface->call("ListBackups");
    if (reply.isValid()) {
        m_backups = reply.value();
        emit backupsChanged();
    }
}

QVariantMap NavigationResetUI::validateBackup(const QString &backupPath)
{
    QVariantMap result;
    result["valid"] = false;
    result["error"] = "";
    
    // Check if backup file exists
    QFile file(backupPath);
    if (!file.exists()) {
        result["error"] = "Backup file does not exist";
        return result;
    }
    
    // Check file size (should be > 0)
    if (file.size() == 0) {
        result["error"] = "Backup file is empty";
        return result;
    }
    
    // Try to read backup metadata (simplified validation)
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray header = file.read(1024);
        file.close();
        
        // Check if it looks like a valid backup (contains expected markers)
        if (header.contains("backup") || header.contains("navigation") || header.contains("config")) {
            result["valid"] = true;
            result["size"] = file.size();
        } else {
            result["error"] = "Backup file format invalid";
        }
    } else {
        result["error"] = "Cannot read backup file";
    }
    
    return result;
}

void NavigationResetUI::restoreBackup(const QString &backupPath)
{
    QDBusInterface* iface = getResetInterface();
    if (!iface || !iface->isValid()) {
        emit backupRestoreError("Navigation reset service not available");
        return;
    }
    
    // Validate backup first
    QVariantMap validation = validateBackup(backupPath);
    if (!validation.value("valid").toBool()) {
        emit backupRestoreError(QString("Backup validation failed: %1").arg(validation.value("error").toString()));
        return;
    }
    
    QDBusReply<bool> reply = iface->call("RestoreBackup", backupPath);
    if (reply.isValid() && reply.value()) {
        emit backupRestored(backupPath);
        loadBackups(); // Refresh backup list
    } else {
        emit backupRestoreError(QString("Failed to restore backup: %1").arg(reply.isValid() ? "Unknown error" : reply.error().message()));
    }
}

void NavigationResetUI::deleteBackup(const QString &backupPath)
{
    // Delete backup file directly (service doesn't have delete method)
    QFile file(backupPath);
    if (file.exists()) {
        if (file.remove()) {
            loadBackups(); // Refresh backup list
        } else {
            emit resetError(QString("Failed to delete backup: %1").arg(backupPath));
        }
    }
}

void NavigationResetUI::onResetStarted(const QString &resetId, const QString &resetType)
{
    if (resetId == m_currentResetId) {
        m_resetProgress = 10.0;
        emit resetProgressChanged();
    }
}

void NavigationResetUI::onResetCompleted(const QString &resetId, const QString &status)
{
    if (resetId == m_currentResetId) {
        m_resetInProgress = false;
        m_resetProgress = 100.0;
        emit resetInProgressChanged();
        emit resetProgressChanged();
        
        // Get backup path from service
        QString backupPath = "";
        QDBusInterface* iface = getResetInterface();
        if (iface && iface->isValid()) {
            QDBusReply<QVariantList> reply = iface->call("ListBackups");
            if (reply.isValid() && !reply.value().isEmpty()) {
                QVariantMap latestBackup = reply.value().last().toMap();
                backupPath = latestBackup.value("backup_path").toString();
            }
        }
        
        emit resetCompleted(status, backupPath);
        loadBackups(); // Refresh backup list
    }
}

void NavigationResetUI::onBackupCreated(const QString &backupPath)
{
    loadBackups(); // Refresh backup list
}

QDBusInterface* NavigationResetUI::getResetInterface()
{
    if (!m_resetInterface) {
        m_resetInterface = new QDBusInterface("org.milos.NavigationReset",
                                             "/org/milos/NavigationReset",
                                             "org.milos.NavigationReset",
                                             QDBusConnection::sessionBus(),
                                             this);
    }
    return m_resetInterface;
}

void NavigationResetUI::updateResetProgress()
{
    // Progress is updated via timer in executeReset
}

