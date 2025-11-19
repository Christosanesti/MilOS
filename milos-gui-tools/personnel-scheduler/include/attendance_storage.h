#ifndef ATTENDANCE_STORAGE_H
#define ATTENDANCE_STORAGE_H

#include "attendance_tracker.h"
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>
#include <memory>
#include <QSqlDatabase>

/**
 * @brief Attendance Storage
 * 
 * Stores attendance records with encryption.
 */
class AttendanceStorage : public QObject {
    Q_OBJECT

public:
    explicit AttendanceStorage(QObject* parent = nullptr);
    ~AttendanceStorage();

    /**
     * @brief Initialize storage
     * @param databasePath Database file path
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const QString& databasePath);

    /**
     * @brief Store attendance record
     * @param record Attendance record
     * @return true if stored successfully, false otherwise
     */
    bool storeRecord(const AttendanceRecord& record);

    /**
     * @brief Get attendance records
     * @param personnelId Optional personnel ID filter
     * @param startDate Optional start date filter
     * @param endDate Optional end date filter
     * @return List of attendance records
     */
    QList<AttendanceRecord> getRecords(const QString& personnelId = QString(),
                                       const QDateTime& startDate = QDateTime(),
                                       const QDateTime& endDate = QDateTime()) const;

    /**
     * @brief Get attendance record by ID
     * @param recordId Record ID
     * @return Attendance record or empty record if not found
     */
    AttendanceRecord getRecord(const QString& recordId) const;

    /**
     * @brief Delete attendance record
     * @param recordId Record ID
     * @return true if deleted successfully, false otherwise
     */
    bool deleteRecord(const QString& recordId);

private:
    bool createDatabaseSchema();
    QByteArray encryptBiometricData(const QByteArray& data) const;
    QByteArray decryptBiometricData(const QByteArray& encryptedData) const;
    
    QString m_databasePath;
    std::unique_ptr<PQCEncryption> m_encryption;
    bool m_initialized;
};

#endif // ATTENDANCE_STORAGE_H

