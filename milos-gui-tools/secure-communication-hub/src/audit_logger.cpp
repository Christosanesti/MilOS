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
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/communication-hub-logs";
    QDir().mkpath(logDir);
    return true;
}

void AuditLogger::logMessageSent(const QString& messageId, const QString& type, const QString& to) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Message sent: %3 (type: %4, to: %5)")
                       .arg(timestamp, eventId, messageId, type, to);
    
    qDebug() << logEntry;
}

void AuditLogger::logMessageReceived(const QString& messageId, const QString& type) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Message received: %3 (type: %4)")
                       .arg(timestamp, eventId, messageId, type);
    
    qDebug() << logEntry;
}

void AuditLogger::logMessageExpired(const QString& messageId) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Message expired: %3")
                       .arg(timestamp, eventId, messageId);
    
    qDebug() << logEntry;
}

QString AuditLogger::generateEventId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

