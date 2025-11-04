#ifndef ENCRYPTIONMANAGER_INTERFACE_H
#define ENCRYPTIONMANAGER_INTERFACE_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/**
 * @brief D-Bus interface definition for org.milos.EncryptionManager
 * 
 * This interface provides encryption operations for single and batch file operations.
 * Interface name: org.milos.EncryptionManager
 */
class OrgMilosEncryptionManagerInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.milos.EncryptionManager"; }

public:
    OrgMilosEncryptionManagerInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr)
        : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
    {
    }

    ~OrgMilosEncryptionManagerInterface() = default;

public Q_SLOTS: // METHODS
    /**
     * @brief Encrypt a single file
     * @param path File path to encrypt
     * @param algorithm Encryption algorithm identifier
     * @param key_id UUID of encryption key
     * @return Operation ID for tracking the encryption operation
     */
    inline QDBusPendingReply<QString> EncryptFile(const QString &path, const QString &algorithm, const QString &key_id)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(path) << QVariant::fromValue(algorithm) << QVariant::fromValue(key_id);
        return asyncCallWithArgumentList(QStringLiteral("EncryptFile"), argumentList);
    }

    /**
     * @brief Encrypt multiple files (batch operation)
     * @param paths Array of file paths (up to 5 files)
     * @param algorithm Encryption algorithm identifier
     * @param key_id UUID of encryption key
     * @return Operation ID for tracking the batch encryption operation
     */
    inline QDBusPendingReply<QString> EncryptFiles(const QStringList &paths, const QString &algorithm, const QString &key_id)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(paths) << QVariant::fromValue(algorithm) << QVariant::fromValue(key_id);
        return asyncCallWithArgumentList(QStringLiteral("EncryptFiles"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    /**
     * @brief Emitted when encryption operation starts
     * @param operation_id Operation identifier
     */
    void EncryptionStarted(const QString &operation_id);

    /**
     * @brief Emitted during encryption operation progress
     * @param operation_id Operation identifier
     * @param percentage Progress percentage (0-100)
     */
    void EncryptionProgress(const QString &operation_id, int percentage);

    /**
     * @brief Emitted when encryption operation completes
     * @param operation_id Operation identifier
     * @param status Operation status (SUCCESS, FAILED, CANCELLED)
     */
    void EncryptionCompleted(const QString &operation_id, const QString &status);

    /**
     * @brief Emitted when encryption operation encounters an error
     * @param operation_id Operation identifier
     * @param error Error message
     */
    void EncryptionError(const QString &operation_id, const QString &error);

    /**
     * @brief Emitted when batch encryption operation starts
     * @param operation_id Operation identifier for batch operation
     */
    void BatchEncryptionStarted(const QString &operation_id);

    /**
     * @brief Emitted during batch encryption operation progress
     * @param operation_id Operation identifier for batch operation
     * @param file_index Index of current file being processed (0-based)
     * @param percentage Progress percentage for current file (0-100)
     */
    void BatchEncryptionProgress(const QString &operation_id, int file_index, int percentage);

    /**
     * @brief Emitted when batch encryption operation completes
     * @param operation_id Operation identifier for batch operation
     * @param status Operation status (SUCCESS, FAILED, CANCELLED)
     */
    void BatchEncryptionCompleted(const QString &operation_id, const QString &status);
};

namespace org {
  namespace milos {
    typedef ::OrgMilosEncryptionManagerInterface EncryptionManager;
  }
}
#endif // ENCRYPTIONMANAGER_INTERFACE_H

