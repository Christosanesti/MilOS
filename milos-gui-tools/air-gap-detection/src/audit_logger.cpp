#include "audit_logger.h"
#include <QUuid>
#include <QDebug>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>

AuditLogger::AuditLogger(QObject* parent)
    : QObject(parent)
{
}

AuditLogger::~AuditLogger() {
}

bool AuditLogger::initialize() {
    // Create log directory if it doesn't exist
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/air-gap-logs";
    QDir().mkpath(logDir);
    
    return true;
}

void AuditLogger::logStatusChange(const QString& oldStatus, const QString& newStatus) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Status changed: %3 -> %4")
                       .arg(timestamp, eventId, oldStatus, newStatus);
    
    qDebug() << logEntry;
    
    // In production, would write to audit service
}

void AuditLogger::logPolicyChange(const QString& oldLevel, const QString& newLevel) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Policy changed: %3 -> %4")
                       .arg(timestamp, eventId, oldLevel, newLevel);
    
    qDebug() << logEntry;
}

void AuditLogger::logBlockedOperation(const QString& operation, const QString& reason) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Operation blocked: %3 (reason: %4)")
                       .arg(timestamp, eventId, operation, reason);
    
    qDebug() << logEntry;
}

QString AuditLogger::generateEventId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

