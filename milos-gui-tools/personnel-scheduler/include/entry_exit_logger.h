#ifndef ENTRY_EXIT_LOGGER_H
#define ENTRY_EXIT_LOGGER_H

#include "attendance_tracker.h"
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QMap>

/**
 * @brief Entry/Exit Logger
 * 
 * Manages entry/exit state and logging.
 */
class EntryExitLogger : public QObject {
    Q_OBJECT

public:
    explicit EntryExitLogger(QObject* parent = nullptr);
    ~EntryExitLogger();

    /**
     * @brief Log entry event
     * @param personnelId Personnel ID
     * @param location Location/area
     * @param timestamp Timestamp
     * @return true if logged successfully, false otherwise
     */
    bool logEntry(const QString& personnelId, const QString& location, const QDateTime& timestamp);

    /**
     * @brief Log exit event
     * @param personnelId Personnel ID
     * @param location Location/area
     * @param timestamp Timestamp
     * @return true if logged successfully, false otherwise
     */
    bool logExit(const QString& personnelId, const QString& location, const QDateTime& timestamp);

    /**
     * @brief Get entry timestamp for personnel
     * @param personnelId Personnel ID
     * @return Entry timestamp or invalid QDateTime if not present
     */
    QDateTime getEntryTimestamp(const QString& personnelId) const;

    /**
     * @brief Check if personnel is present
     * @param personnelId Personnel ID
     * @return true if present, false otherwise
     */
    bool isPersonnelPresent(const QString& personnelId) const;

    /**
     * @brief Get all present personnel
     * @return Map of personnel ID to entry timestamp
     */
    QMap<QString, QDateTime> getPresentPersonnel() const;

Q_SIGNALS:
    /**
     * @brief Emitted when entry is logged
     */
    void entryLogged(const QString& personnelId, const QString& location, const QDateTime& timestamp);

    /**
     * @brief Emitted when exit is logged
     */
    void exitLogged(const QString& personnelId, const QString& location, const QDateTime& timestamp);

private:
    QMap<QString, QDateTime> m_personnelPresence;  // personnelId -> entry timestamp
};

#endif // ENTRY_EXIT_LOGGER_H

