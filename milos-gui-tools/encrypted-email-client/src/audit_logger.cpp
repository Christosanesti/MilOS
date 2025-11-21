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
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/email-logs";
    QDir().mkpath(logDir);
    return true;
}

void AuditLogger::logEmailSent(const QString& messageId, const QString& to) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Email sent: %3 to %4")
                       .arg(timestamp, eventId, messageId, to);
    
    qDebug() << logEntry;
}

void AuditLogger::logEmailReceived(const QString& messageId, const QString& from) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Email received: %3 from %4")
                       .arg(timestamp, eventId, messageId, from);
    
    qDebug() << logEntry;
}

void AuditLogger::logAccountAdded(const QString& accountId, const QString& email) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Account added: %3 (%4)")
                       .arg(timestamp, eventId, accountId, email);
    
    qDebug() << logEntry;
}

QString AuditLogger::generateEventId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

