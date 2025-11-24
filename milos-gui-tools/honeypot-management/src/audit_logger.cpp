#include "audit_logger.h"
#include <QUuid>
#include <QDebug>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>

AuditLogger::AuditLogger(QObject* parent)
    : QObject(parent)
{
}

AuditLogger::~AuditLogger() {
}

bool AuditLogger::initialize() {
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/honeypot-logs";
    QDir().mkpath(logDir);
    return true;
}

void AuditLogger::logHoneypotDeployed(const QString& honeypotId, const QString& type) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Honeypot deployed: %3 (type: %4)")
                       .arg(timestamp, eventId, honeypotId, type);
    
    qDebug() << logEntry;
}

void AuditLogger::logThreatDetected(const QString& honeypotId, const QString& threatType) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Threat detected: %3 (honeypot: %4)")
                       .arg(timestamp, eventId, threatType, honeypotId);
    
    qDebug() << logEntry;
}

QString AuditLogger::generateEventId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}






