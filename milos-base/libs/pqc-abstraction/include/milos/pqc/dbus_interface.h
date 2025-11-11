#ifndef DBUS_INTERFACE_H
#define DBUS_INTERFACE_H

#include <QObject>
#include <QString>
#include <QStringList>

class AlgorithmFactory;
class AlgorithmRegistry;
class HybridEncryption;
class FallbackManager;

/**
 * @brief D-Bus Interface for PQC Abstraction Layer
 * 
 * Implements org.milos.PQCAbstraction D-Bus interface with methods
 * for algorithm management and encryption operations.
 * Uses Qt D-Bus adaptor pattern.
 */
class DBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.PQCAbstraction")

public:
    DBusInterface(QObject* parent = nullptr);
    ~DBusInterface();

    /**
     * @brief Initialize D-Bus interface
     * @param algorithmFactory Algorithm factory instance
     * @param algorithmRegistry Algorithm registry instance
     * @param hybridEncryption Hybrid encryption instance
     * @param fallbackManager Fallback manager instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(
        AlgorithmFactory* algorithmFactory,
        AlgorithmRegistry* algorithmRegistry,
        HybridEncryption* hybridEncryption,
        FallbackManager* fallbackManager
    );

    /**
     * @brief Start D-Bus interface
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop D-Bus interface
     */
    void stop();

    /**
     * @brief Check if D-Bus interface is running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return m_running; }

    /**
     * @brief Check if D-Bus interface is healthy
     * @return true if healthy, false otherwise
     */
    bool isHealthy() const;

public Q_SLOTS:
    // D-Bus methods
    /**
     * @brief Get list of available algorithms
     * @return JSON string with available algorithms
     */
    QString GetAvailableAlgorithms();

    /**
     * @brief Select algorithm for operations
     * @param algorithmId Algorithm ID
     * @return true if selection successful, false otherwise
     */
    bool SelectAlgorithm(const QString& algorithmId);

    /**
     * @brief Get algorithm information
     * @param algorithmId Algorithm ID
     * @return JSON string with algorithm information
     */
    QString GetAlgorithmInfo(const QString& algorithmId);

    /**
     * @brief Encrypt data
     * @param data Data to encrypt (base64 encoded)
     * @param algorithmId Algorithm ID (optional, uses selected algorithm if empty)
     * @return Encrypted data (base64 encoded JSON string)
     */
    QString Encrypt(const QString& data, const QString& algorithmId = QString());

    /**
     * @brief Decrypt data
     * @param encryptedData Encrypted data (base64 encoded JSON string)
     * @param algorithmId Algorithm ID (optional, uses selected algorithm if empty)
     * @return Decrypted data (base64 encoded)
     */
    QString Decrypt(const QString& encryptedData, const QString& algorithmId = QString());

private:
    bool m_running;
    bool m_initialized;

    AlgorithmFactory* m_algorithmFactory;
    AlgorithmRegistry* m_algorithmRegistry;
    HybridEncryption* m_hybridEncryption;
    FallbackManager* m_fallbackManager;
    
    QString m_selectedAlgorithm;  // Currently selected algorithm

    /**
     * @brief Register D-Bus interface
     * @return true if registration successful, false otherwise
     */
    bool registerInterface();

    /**
     * @brief Unregister D-Bus interface
     */
    void unregisterInterface();
};

#endif // DBUS_INTERFACE_H

