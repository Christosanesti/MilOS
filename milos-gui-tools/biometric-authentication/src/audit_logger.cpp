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
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/biometric-logs";
    QDir().mkpath(logDir);
    return true;
}

void AuditLogger::logUserEnrolled(const QString& userId, const QString& biometricType) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] User enrolled: %3 (type: %4)")
                       .arg(timestamp, eventId, userId, biometricType);
    
    qDebug() << logEntry;
}

void AuditLogger::logAuthenticationAttempt(const QString& userId, bool success) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Authentication: %3 (%4)")
                       .arg(timestamp, eventId, userId, success ? "SUCCESS" : "FAILED");
    
    qDebug() << logEntry;
}

void AuditLogger::logMFAAuthentication(const QString& userId, bool success) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] MFA Authentication: %3 (%4)")
                       .arg(timestamp, eventId, userId, success ? "SUCCESS" : "FAILED");
    
    qDebug() << logEntry;
}

QString AuditLogger::generateEventId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

