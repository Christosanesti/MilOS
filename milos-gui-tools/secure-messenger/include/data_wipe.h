#ifndef DATA_WIPE_H
#define DATA_WIPE_H

#include <QObject>
#include <QString>
#include <QStringList>

/**
 * @brief Wipe Method
 */
enum class WipeMethod {
    Cryptographic,
    Overwrite,
    SecureDelete
};

/**
 * @brief Data Wipe
 * 
 * Provides secure data wipe procedures using cryptographic erasure.
 */
class DataWipe : public QObject {
    Q_OBJECT

public:
    explicit DataWipe(QObject* parent = nullptr);
    ~DataWipe();

    /**
     * @brief Initialize data wipe
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Wipe file
     * @param filePath File path
     * @param method Wipe method
     * @return true if wipe successful, false otherwise
     */
    bool wipeFile(const QString& filePath, WipeMethod method = WipeMethod::Cryptographic);

    /**
     * @brief Wipe directory
     * @param dirPath Directory path
     * @param method Wipe method
     * @return true if wipe successful, false otherwise
     */
    bool wipeDirectory(const QString& dirPath, WipeMethod method = WipeMethod::Cryptographic);

    /**
     * @brief Wipe database
     * @param dbPath Database path
     * @param method Wipe method
     * @return true if wipe successful, false otherwise
     */
    bool wipeDatabase(const QString& dbPath, WipeMethod method = WipeMethod::Cryptographic);

    /**
     * @brief Verify wipe
     * @param filePath File path
     * @return true if verified, false otherwise
     */
    bool verifyWipe(const QString& filePath) const;

Q_SIGNALS:
    /**
     * @brief Emitted when wipe is started
     */
    void wipeStarted(const QString& path);

    /**
     * @brief Emitted when wipe is completed
     */
    void wipeCompleted(const QString& path);

    /**
     * @brief Emitted when wipe fails
     */
    void wipeFailed(const QString& path, const QString& error);

private:
    bool cryptographicWipe(const QString& filePath);
    bool overwriteWipe(const QString& filePath, int passes = 3);
    bool secureDelete(const QString& filePath);
    QByteArray generateRandomData(int size) const;
};

#endif // DATA_WIPE_H

