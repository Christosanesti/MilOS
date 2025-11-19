#include "entry_exit_logger.h"
#include <QDebug>

EntryExitLogger::EntryExitLogger(QObject* parent)
    : QObject(parent)
{
}

EntryExitLogger::~EntryExitLogger() {
}

bool EntryExitLogger::logEntry(const QString& personnelId, const QString& location, const QDateTime& timestamp) {
    if (personnelId.isEmpty()) {
        return false;
    }
    
    // Check if already present
    if (m_personnelPresence.contains(personnelId)) {
        qWarning() << "Personnel already present:" << personnelId;
        return false;
    }
    
    m_personnelPresence[personnelId] = timestamp;
    emit entryLogged(personnelId, location, timestamp);
    
    return true;
}

bool EntryExitLogger::logExit(const QString& personnelId, const QString& location, const QDateTime& timestamp) {
    if (personnelId.isEmpty()) {
        return false;
    }
    
    // Check if present
    if (!m_personnelPresence.contains(personnelId)) {
        qWarning() << "Personnel not present:" << personnelId;
        return false;
    }
    
    m_personnelPresence.remove(personnelId);
    emit exitLogged(personnelId, location, timestamp);
    
    return true;
}

QDateTime EntryExitLogger::getEntryTimestamp(const QString& personnelId) const {
    return m_personnelPresence.value(personnelId, QDateTime());
}

bool EntryExitLogger::isPersonnelPresent(const QString& personnelId) const {
    return m_personnelPresence.contains(personnelId);
}

QMap<QString, QDateTime> EntryExitLogger::getPresentPersonnel() const {
    return m_personnelPresence;
}

