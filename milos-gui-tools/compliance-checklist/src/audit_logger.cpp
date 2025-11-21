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
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/compliance-logs";
    QDir().mkpath(logDir);
    return true;
}

void AuditLogger::logChecklistCreated(const QString& checklistId, const QString& standard) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Checklist created: %3 (standard: %4)")
                       .arg(timestamp, eventId, checklistId, standard);
    
    qDebug() << logEntry;
}

void AuditLogger::logItemStatusChanged(const QString& checklistId, const QString& itemId, const QString& status) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Item status changed: %3/%4 -> %5")
                       .arg(timestamp, eventId, checklistId, itemId, status);
    
    qDebug() << logEntry;
}

void AuditLogger::logReportGenerated(const QString& checklistId, const QString& format) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Report generated: %3 (format: %4)")
                       .arg(timestamp, eventId, checklistId, format);
    
    qDebug() << logEntry;
}

QString AuditLogger::generateEventId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

