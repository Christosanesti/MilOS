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
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/drone-logs";
    QDir().mkpath(logDir);
    return true;
}

void AuditLogger::logDroneCommand(const QString& droneId, const QString& command) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Drone command: %3 (drone: %4)")
                       .arg(timestamp, eventId, command, droneId);
    
    qDebug() << logEntry;
}

void AuditLogger::logMissionExecuted(const QString& missionId, const QString& droneId) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Mission executed: %3 (drone: %4)")
                       .arg(timestamp, eventId, missionId, droneId);
    
    qDebug() << logEntry;
}

QString AuditLogger::generateEventId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

