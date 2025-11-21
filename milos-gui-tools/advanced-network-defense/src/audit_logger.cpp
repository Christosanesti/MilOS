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
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/network-defense-logs";
    QDir().mkpath(logDir);
    return true;
}

void AuditLogger::logThreatDetected(const QString& threatId, const QString& threatType) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] ML Threat detected: %3 (type: %4)")
                       .arg(timestamp, eventId, threatId, threatType);
    
    qDebug() << logEntry;
}

void AuditLogger::logForensicCapture(const QString& evidenceId, const QString& type) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Forensic evidence captured: %3 (type: %4)")
                       .arg(timestamp, eventId, evidenceId, type);
    
    qDebug() << logEntry;
}

void AuditLogger::logWorkflowExecuted(const QString& workflowId, const QString& threatId) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Workflow executed: %3 (threat: %4)")
                       .arg(timestamp, eventId, workflowId, threatId);
    
    qDebug() << logEntry;
}

QString AuditLogger::generateEventId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

