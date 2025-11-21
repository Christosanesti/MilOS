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
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/location-logs";
    QDir().mkpath(logDir);
    return true;
}

void AuditLogger::logLocationUpdate(const QString& personnelId, const QString& location) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Location updated: %3 -> %4")
                       .arg(timestamp, eventId, personnelId, location);
    
    qDebug() << logEntry;
}

void AuditLogger::logTrackingStarted(const QString& personnelId) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Tracking started: %3")
                       .arg(timestamp, eventId, personnelId);
    
    qDebug() << logEntry;
}

void AuditLogger::logTrackingStopped(const QString& personnelId) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Tracking stopped: %3")
                       .arg(timestamp, eventId, personnelId);
    
    qDebug() << logEntry;
}

QString AuditLogger::generateEventId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

