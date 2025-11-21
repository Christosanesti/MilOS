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
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/threat-intel-logs";
    QDir().mkpath(logDir);
    return true;
}

void AuditLogger::logFeedUpdate(const QString& feedId, int indicatorCount) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Feed updated: %3 (indicators: %4)")
                       .arg(timestamp, eventId, feedId, QString::number(indicatorCount));
    
    qDebug() << logEntry;
}

void AuditLogger::logIndicatorBlocked(const QString& indicatorId, const QString& reason) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Indicator blocked: %3 (reason: %4)")
                       .arg(timestamp, eventId, indicatorId, reason);
    
    qDebug() << logEntry;
}

void AuditLogger::logCorrelationDetected(const QString& correlationId) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Correlation detected: %3")
                       .arg(timestamp, eventId, correlationId);
    
    qDebug() << logEntry;
}

QString AuditLogger::generateEventId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

