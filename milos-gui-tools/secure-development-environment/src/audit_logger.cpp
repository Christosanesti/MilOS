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
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/secure-dev-logs";
    QDir().mkpath(logDir);
    return true;
}

void AuditLogger::logSecurityIssue(const QString& issueId, const QString& severity) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Security issue: %3 (severity: %4)")
                       .arg(timestamp, eventId, issueId, severity);
    
    qDebug() << logEntry;
}

void AuditLogger::logVulnerabilityFound(const QString& dependency, const QString& version) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Vulnerability found: %3@%4")
                       .arg(timestamp, eventId, dependency, version);
    
    qDebug() << logEntry;
}

QString AuditLogger::generateEventId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

