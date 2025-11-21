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
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/satcom-logs";
    QDir().mkpath(logDir);
    return true;
}

void AuditLogger::logMessageSent(const QString& messageId, const QString& protocol) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Satellite message sent: %3 (protocol: %4)")
                       .arg(timestamp, eventId, messageId, protocol);
    
    qDebug() << logEntry;
}

void AuditLogger::logBandwidthOptimized(double originalSize, double optimizedSize) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    double ratio = (originalSize > 0) ? (optimizedSize / originalSize) : 0.0;
    QString logEntry = QString("[%1] [%2] Bandwidth optimized: %3 MB -> %4 MB (ratio: %5)")
                       .arg(timestamp, eventId)
                       .arg(originalSize, 0, 'f', 2)
                       .arg(optimizedSize, 0, 'f', 2)
                       .arg(ratio, 0, 'f', 2);
    
    qDebug() << logEntry;
}

QString AuditLogger::generateEventId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

