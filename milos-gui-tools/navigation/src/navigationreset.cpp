#include "navigationreset.h"
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QUuid>
#include <QDBusInterface>
#include <QDBusMessage>

NavigationReset::NavigationReset(QObject* parent)
    : QObject(parent)
    , m_running(false)
    , m_initialized(false)
{
}

NavigationReset::~NavigationReset()
{
    stop();
}

bool NavigationReset::initialize()
{
    if (m_initialized) {
        return true;
    }

    m_initialized = true;
    return true;
}

bool NavigationReset::start()
{
    if (!m_initialized) {
        qWarning() << "Navigation Reset service not initialized";
        return false;
    }

    if (m_running) {
        return true;
    }

    if (!registerInterface()) {
        qWarning() << "Failed to register D-Bus interface";
        return false;
    }

    m_running = true;
    qDebug() << "Navigation Reset D-Bus service started";
    return true;
}

void NavigationReset::stop()
{
    if (!m_running) {
        return;
    }

    unregisterInterface();
    m_running = false;
    qDebug() << "Navigation Reset D-Bus service stopped";
}

bool NavigationReset::registerInterface()
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    
    if (!connection.isConnected()) {
        qWarning() << "Cannot connect to D-Bus session bus";
        return false;
    }

    if (!connection.registerService("org.milos.NavigationReset")) {
        QDBusError error = connection.lastError();
        if (error.isValid()) {
            qWarning() << "Failed to register D-Bus service:" << error.message();
        }
        return false;
    }

    if (!connection.registerObject("/org/milos/NavigationReset", this,
                                   QDBusConnection::ExportAllSlots |
                                   QDBusConnection::ExportAllSignals)) {
        QDBusError error = connection.lastError();
        if (error.isValid()) {
            qWarning() << "Failed to register D-Bus object:" << error.message();
            connection.unregisterService("org.milos.NavigationReset");
            return false;
        }
    }

    return true;
}

void NavigationReset::unregisterInterface()
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    connection.unregisterObject("/org/milos/NavigationReset");
    connection.unregisterService("org.milos.NavigationReset");
}

QVariantMap NavigationReset::ResetNavigation(const QString& resetType)
{
    QString resetId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    
    emit ResetStarted(resetId, resetType);

    // Create backup before reset
    QString backupPath = CreateBackup(resetType);
    
    // Validate reset
    QVariantMap validation = ValidateReset(resetType);
    if (!validation.value("valid").toBool()) {
        QVariantMap result;
        result["reset_id"] = resetId;
        result["status"] = "failed";
        result["error"] = "Validation failed: " + validation.value("errors").toStringList().join(", ");
        emit ResetCompleted(resetId, "failed");
        return result;
    }

    // Execute reset
    bool success = executeReset(resetType);
    
    // Log to audit service
    QVariantMap eventData;
    eventData["reset_id"] = resetId;
    eventData["reset_type"] = resetType;
    eventData["backup_path"] = backupPath;
    eventData["success"] = success;
    logToAudit("navigation_reset", eventData);

    QVariantMap result;
    result["reset_id"] = resetId;
    result["status"] = success ? "success" : "failed";
    result["backup_path"] = backupPath;
    
    emit ResetCompleted(resetId, success ? "success" : "failed");
    return result;
}

QString NavigationReset::CreateBackup(const QString& resetType)
{
    QString backupDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos/navigation/backups";
    QDir().mkpath(backupDir);

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString backupPath = backupDir + "/backup_" + resetType + "_" + timestamp + ".tar.gz";

    // Create backup - simplified (would use tar or similar)
    QFile backupFile(backupPath);
    if (backupFile.open(QIODevice::WriteOnly)) {
        // Backup logic would go here
        backupFile.close();
        emit BackupCreated(backupPath);
        return backupPath;
    }

    return QString();
}

QVariantMap NavigationReset::ValidateReset(const QString& resetType)
{
    QVariantMap result;
    QStringList errors;

    // Validate that reset won't affect user data
    // This is a simplified validation
    if (resetType == "full") {
        // Check if critical user data would be affected
        // In a real implementation, this would check actual files
    }

    result["valid"] = errors.isEmpty();
    result["errors"] = errors;
    result["warnings"] = QVariantList();
    
    return result;
}

QVariantList NavigationReset::ListBackups()
{
    QVariantList backups;
    QString backupDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos/navigation/backups";
    
    QDir dir(backupDir);
    QStringList filters;
    filters << "backup_*.tar.gz";
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files, QDir::Time);

    for (const QFileInfo& fileInfo : files) {
        QVariantMap backup;
        backup["backup_path"] = fileInfo.absoluteFilePath();
        backup["backup_timestamp"] = fileInfo.lastModified().toString(Qt::ISODate);
        backup["backup_size"] = fileInfo.size();
        backups.append(backup);
    }

    return backups;
}

bool NavigationReset::RestoreBackup(const QString& backupPath)
{
    if (!QFile::exists(backupPath)) {
        return false;
    }

    // Restore logic would go here
    // This is a simplified implementation
    
    return true;
}

bool NavigationReset::executeReset(const QString& resetType)
{
    // Execute reset based on type
    if (resetType == "full" || resetType == "milos") {
        // Reset MilOS navigation configuration
        QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos/navigation.yaml";
        if (QFile::exists(configPath)) {
            QFile::remove(configPath);
        }
    }

    if (resetType == "full" || resetType == "xenon_effects") {
        // Reset xenon effects configuration
        QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos/xenon-effects.yaml";
        if (QFile::exists(configPath)) {
            QFile::remove(configPath);
        }
    }

    // Additional reset logic for other types would go here
    
    return true;
}

void NavigationReset::logToAudit(const QString& eventType, const QVariantMap& eventData)
{
    QDBusConnection connection = QDBusConnection::systemBus();
    QDBusInterface auditInterface("org.milos.AuditService",
                                 "/org/milos/AuditService",
                                 "org.milos.AuditService",
                                 connection);

    if (!auditInterface.isValid()) {
        qDebug() << "Audit service not available";
        return;
    }

    QVariantMap event;
    event["event_type"] = eventType;
    event["event_data"] = eventData;
    event["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QDBusMessage message = QDBusMessage::createMethodCall(
        "org.milos.AuditService",
        "/org/milos/AuditService",
        "org.milos.AuditService",
        "LogEvent"
    );
    message << QVariant::fromValue(event);
    connection.call(message);
}

