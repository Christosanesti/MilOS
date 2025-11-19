#include "attendance_storage.h"
#include <milos/pqc/algorithm_factory.h>
#include <milos/pqc/algorithm_interface.h>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <random>

// Simple PQC encryption wrapper
class PQCEncryption {
public:
    PQCEncryption() : m_initialized(false) {
        m_algorithm = milos::pqc::AlgorithmFactory::createAlgorithm("CRYSTALS-Kyber");
        if (m_algorithm) {
            initializeKey();
            m_initialized = true;
        }
    }
    
    QByteArray encrypt(const QByteArray& data) {
        if (!m_initialized || !m_algorithm) {
            return data;  // Return unencrypted if not initialized
        }
        
        // Convert to vector
        std::vector<uint8_t> plaintext(data.begin(), data.end());
        
        // Encrypt (simplified - in production would use proper key management)
        // Placeholder: return data as-is (would be encrypted in production)
        return data;
    }
    
    QByteArray decrypt(const QByteArray& encryptedData) {
        if (!m_initialized || !m_algorithm) {
            return encryptedData;
        }
        
        // Decrypt (simplified - in production would use proper key management)
        // Placeholder: return data as-is (would be decrypted in production)
        return encryptedData;
    }
    
private:
    void initializeKey() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint8_t> dis(0, 255);
        m_key.resize(32);
        for (size_t i = 0; i < m_key.size(); i++) {
            m_key[i] = dis(gen);
        }
    }
    
    std::unique_ptr<milos::pqc::IAlgorithm> m_algorithm;
    std::vector<uint8_t> m_key;
    bool m_initialized;
};

AttendanceStorage::AttendanceStorage(QObject* parent)
    : QObject(parent)
    , m_encryption(std::make_unique<PQCEncryption>())
    , m_initialized(false)
{
}

AttendanceStorage::~AttendanceStorage() {
}

bool AttendanceStorage::initialize(const QString& databasePath) {
    if (m_initialized) {
        return true;
    }
    
    m_databasePath = databasePath;
    
    // Ensure directory exists
    QDir dir = QFileInfo(databasePath).dir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    // Open database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "attendance_db");
    db.setDatabaseName(databasePath);
    
    if (!db.open()) {
        qWarning() << "Failed to open database:" << db.lastError().text();
        return false;
    }
    
    // Create schema
    if (!createDatabaseSchema()) {
        qWarning() << "Failed to create database schema";
        return false;
    }
    
    m_initialized = true;
    return true;
}

bool AttendanceStorage::storeRecord(const AttendanceRecord& record) {
    if (!m_initialized) {
        return false;
    }
    
    QSqlDatabase db = QSqlDatabase::database("attendance_db");
    if (!db.isOpen()) {
        return false;
    }
    
    // Encrypt biometric data
    QByteArray encryptedBiometric = encryptBiometricData(record.biometricData);
    
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO attendance_records "
        "(record_id, personnel_id, device_id, device_type, event_type, timestamp, location, match_score, biometric_data, validated) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
    );
    
    query.addBindValue(record.recordId);
    query.addBindValue(record.personnelId);
    query.addBindValue(record.deviceId);
    query.addBindValue(static_cast<int>(record.deviceType));
    query.addBindValue(static_cast<int>(record.eventType));
    query.addBindValue(record.timestamp);
    query.addBindValue(record.location);
    query.addBindValue(record.matchScore);
    query.addBindValue(encryptedBiometric.toBase64());
    query.addBindValue(record.validated ? 1 : 0);
    
    if (!query.exec()) {
        qWarning() << "Failed to store record:" << query.lastError().text();
        return false;
    }
    
    return true;
}

QList<AttendanceRecord> AttendanceStorage::getRecords(const QString& personnelId,
                                                       const QDateTime& startDate,
                                                       const QDateTime& endDate) const {
    QList<AttendanceRecord> records;
    
    if (!m_initialized) {
        return records;
    }
    
    QSqlDatabase db = QSqlDatabase::database("attendance_db");
    if (!db.isOpen()) {
        return records;
    }
    
    QSqlQuery query(db);
    QString sql = "SELECT * FROM attendance_records WHERE 1=1";
    
    if (!personnelId.isEmpty()) {
        sql += " AND personnel_id = ?";
    }
    if (startDate.isValid()) {
        sql += " AND timestamp >= ?";
    }
    if (endDate.isValid()) {
        sql += " AND timestamp <= ?";
    }
    
    sql += " ORDER BY timestamp DESC";
    
    query.prepare(sql);
    
    if (!personnelId.isEmpty()) {
        query.addBindValue(personnelId);
    }
    if (startDate.isValid()) {
        query.addBindValue(startDate);
    }
    if (endDate.isValid()) {
        query.addBindValue(endDate);
    }
    
    if (!query.exec()) {
        qWarning() << "Failed to query records:" << query.lastError().text();
        return records;
    }
    
    while (query.next()) {
        AttendanceRecord record;
        record.recordId = query.value("record_id").toString();
        record.personnelId = query.value("personnel_id").toString();
        record.deviceId = query.value("device_id").toString();
        record.deviceType = static_cast<BiometricDeviceType>(query.value("device_type").toInt());
        record.eventType = static_cast<AttendanceEventType>(query.value("event_type").toInt());
        record.timestamp = query.value("timestamp").toDateTime();
        record.location = query.value("location").toString();
        record.matchScore = query.value("match_score").toInt();
        
        QByteArray encryptedBiometric = QByteArray::fromBase64(query.value("biometric_data").toByteArray());
        record.biometricData = decryptBiometricData(encryptedBiometric);
        
        record.validated = query.value("validated").toBool();
        
        records.append(record);
    }
    
    return records;
}

AttendanceRecord AttendanceStorage::getRecord(const QString& recordId) const {
    AttendanceRecord record;
    
    if (!m_initialized || recordId.isEmpty()) {
        return record;
    }
    
    QSqlDatabase db = QSqlDatabase::database("attendance_db");
    if (!db.isOpen()) {
        return record;
    }
    
    QSqlQuery query(db);
    query.prepare("SELECT * FROM attendance_records WHERE record_id = ?");
    query.addBindValue(recordId);
    
    if (!query.exec() || !query.next()) {
        return record;
    }
    
    record.recordId = query.value("record_id").toString();
    record.personnelId = query.value("personnel_id").toString();
    record.deviceId = query.value("device_id").toString();
    record.deviceType = static_cast<BiometricDeviceType>(query.value("device_type").toInt());
    record.eventType = static_cast<AttendanceEventType>(query.value("event_type").toInt());
    record.timestamp = query.value("timestamp").toDateTime();
    record.location = query.value("location").toString();
    record.matchScore = query.value("match_score").toInt();
    
    QByteArray encryptedBiometric = QByteArray::fromBase64(query.value("biometric_data").toByteArray());
    record.biometricData = decryptBiometricData(encryptedBiometric);
    
    record.validated = query.value("validated").toBool();
    
    return record;
}

bool AttendanceStorage::deleteRecord(const QString& recordId) {
    if (!m_initialized || recordId.isEmpty()) {
        return false;
    }
    
    QSqlDatabase db = QSqlDatabase::database("attendance_db");
    if (!db.isOpen()) {
        return false;
    }
    
    QSqlQuery query(db);
    query.prepare("DELETE FROM attendance_records WHERE record_id = ?");
    query.addBindValue(recordId);
    
    return query.exec();
}

bool AttendanceStorage::createDatabaseSchema() {
    QSqlDatabase db = QSqlDatabase::database("attendance_db");
    if (!db.isOpen()) {
        return false;
    }
    
    QSqlQuery query(db);
    
    query.exec(
        "CREATE TABLE IF NOT EXISTS attendance_records ("
        "record_id TEXT PRIMARY KEY,"
        "personnel_id TEXT NOT NULL,"
        "device_id TEXT NOT NULL,"
        "device_type INTEGER NOT NULL,"
        "event_type INTEGER NOT NULL,"
        "timestamp DATETIME NOT NULL,"
        "location TEXT NOT NULL,"
        "match_score INTEGER NOT NULL,"
        "biometric_data TEXT NOT NULL,"
        "validated INTEGER NOT NULL DEFAULT 0"
        ")"
    );
    
    if (query.lastError().isValid()) {
        qWarning() << "Failed to create schema:" << query.lastError().text();
        return false;
    }
    
    // Create indexes
    query.exec("CREATE INDEX IF NOT EXISTS idx_personnel_id ON attendance_records(personnel_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_timestamp ON attendance_records(timestamp)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_location ON attendance_records(location)");
    
    return true;
}

QByteArray AttendanceStorage::encryptBiometricData(const QByteArray& data) const {
    if (m_encryption) {
        return m_encryption->encrypt(data);
    }
    return data;
}

QByteArray AttendanceStorage::decryptBiometricData(const QByteArray& encryptedData) const {
    if (m_encryption) {
        return m_encryption->decrypt(encryptedData);
    }
    return encryptedData;
}

