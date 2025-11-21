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
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/radio-logs";
    QDir().mkpath(logDir);
    return true;
}

void AuditLogger::logMessageSent(const QString& messageId, const QString& protocol) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Radio message sent: %3 (protocol: %4)")
                       .arg(timestamp, eventId, messageId, protocol);
    
    qDebug() << logEntry;
}

void AuditLogger::logMessageReceived(const QString& messageId, const QString& protocol) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Radio message received: %3 (protocol: %4)")
                       .arg(timestamp, eventId, messageId, protocol);
    
    qDebug() << logEntry;
}

QString AuditLogger::generateEventId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

