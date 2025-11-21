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
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/enterprise-logs";
    QDir().mkpath(logDir);
    return true;
}

void AuditLogger::logConfigChange(const QString& key, const QString& oldValue, const QString& newValue) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Config changed: %3 (%4 -> %5)")
                       .arg(timestamp, eventId, key, oldValue, newValue);
    
    qDebug() << logEntry;
}

void AuditLogger::logDeploymentCreated(const QString& deploymentId, const QString& name) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Deployment created: %3 (%4)")
                       .arg(timestamp, eventId, deploymentId, name);
    
    qDebug() << logEntry;
}

void AuditLogger::logDeploymentExecuted(const QString& deploymentId) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Deployment executed: %3")
                       .arg(timestamp, eventId, deploymentId);
    
    qDebug() << logEntry;
}

void AuditLogger::logSiteAdded(const QString& siteId, const QString& name) {
    QString eventId = generateEventId();
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QString logEntry = QString("[%1] [%2] Site added: %3 (%4)")
                       .arg(timestamp, eventId, siteId, name);
    
    qDebug() << logEntry;
}

QString AuditLogger::generateEventId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

