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
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/ai-monitoring-logs";
    QDir().mkpath(logDir);
    return true;
}

void AuditLogger::logThreatDetected(const QString& threatId, const QString& threatType) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Threat detected: %3 (type: %4)")
                       .arg(timestamp, eventId, threatId, threatType);
    
    qDebug() << logEntry;
}

void AuditLogger::logThreatContained(const QString& threatId) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Threat contained: %3")
                       .arg(timestamp, eventId, threatId);
    
    qDebug() << logEntry;
}

void AuditLogger::logAnomalyDetected(const QString& type, const QString& details) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Anomaly detected: %3 (%4)")
                       .arg(timestamp, eventId, type, details);
    
    qDebug() << logEntry;
}

QString AuditLogger::generateEventId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

