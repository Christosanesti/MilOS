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
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/jamming-logs";
    QDir().mkpath(logDir);
    return true;
}

void AuditLogger::logJammingStarted(const QString& deviceId, const QString& patternId) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Jamming started: device=%3, pattern=%4")
                       .arg(timestamp, eventId, deviceId, patternId);
    
    qDebug() << logEntry;
}

void AuditLogger::logJammingStopped(const QString& deviceId) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Jamming stopped: device=%3")
                       .arg(timestamp, eventId, deviceId);
    
    qDebug() << logEntry;
}

void AuditLogger::logDeviceConnected(const QString& deviceId) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Device connected: %3")
                       .arg(timestamp, eventId, deviceId);
    
    qDebug() << logEntry;
}

void AuditLogger::logDeviceDisconnected(const QString& deviceId) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Device disconnected: %3")
                       .arg(timestamp, eventId, deviceId);
    
    qDebug() << logEntry;
}

QString AuditLogger::generateEventId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

