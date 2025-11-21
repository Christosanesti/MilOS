#ifndef EMAIL_STORAGE_H
#define EMAIL_STORAGE_H

#include "email_client.h"
#include <QObject>
#include <QString>
#include <QSqlDatabase>
#include <QList>

/**
 * @brief Email Storage
 * 
 * Stores emails in encrypted SQLite database.
 */
class EmailStorage : public QObject {
    Q_OBJECT

public:
    explicit EmailStorage(QObject* parent = nullptr);
    ~EmailStorage();

    /**
     * @brief Initialize email storage
     * @param dbPath Database file path
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const QString& dbPath);

    /**
     * @brief Store email
     * @param message Email message
     * @param encryptedData Encrypted email data
     * @return true if store successful, false otherwise
     */
    bool storeEmail(const EmailMessage& message, const QByteArray& encryptedData);

    /**
     * @brief Retrieve email
     * @param messageId Message ID
     * @return Encrypted email data or empty if not found
     */
    QByteArray retrieveEmail(const QString& messageId) const;

    /**
     * @brief Get all email IDs
     * @param accountId Account ID (optional)
     * @return List of email IDs
     */
    QList<QString> getAllEmailIds(const QString& accountId = QString()) const;

private:
    QSqlDatabase m_database;
    bool createTables();
};

#endif // EMAIL_STORAGE_H

