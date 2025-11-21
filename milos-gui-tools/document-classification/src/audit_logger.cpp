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
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/classification-logs";
    QDir().mkpath(logDir);
    return true;
}

void AuditLogger::logClassification(const QString& filePath, const QString& level) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Document classified: %3 -> %4")
                       .arg(timestamp, eventId, filePath, level);
    
    qDebug() << logEntry;
}

void AuditLogger::logOperationBlocked(const QString& filePath, const QString& operation, const QString& reason) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Operation blocked: %3 on %4 (reason: %5)")
                       .arg(timestamp, eventId, operation, filePath, reason);
    
    qDebug() << logEntry;
}

QString AuditLogger::generateEventId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

