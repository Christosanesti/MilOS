#ifndef CLIPBOARD_SERVICE_H
#define CLIPBOARD_SERVICE_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <memory>
#include <vector>
#include <cstdint>

// Forward declarations
class ClipboardEncryption;
class ClipboardHistoryManager;
class ClipboardIntegration;

/**
 * @brief Clipboard Service
 * 
 * Main service class for secure clipboard management.
 */
class ClipboardService : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentClipboard READ currentClipboard NOTIFY clipboardChanged)
    Q_PROPERTY(bool isEncrypted READ isEncrypted NOTIFY encryptionStatusChanged)
    Q_PROPERTY(uint32_t timeoutSeconds READ timeoutSeconds NOTIFY timeoutChanged)

public:
    explicit ClipboardService(QObject* parent = nullptr);
    ~ClipboardService();

    /**
     * @brief Initialize clipboard service
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Start clipboard service
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop clipboard service
     */
    void stop();

    // Property getters
    QString currentClipboard() const { return m_currentClipboard; }
    bool isEncrypted() const { return m_isEncrypted; }
    uint32_t timeoutSeconds() const { return m_timeoutSeconds; }

    /**
     * @brief Copy data to clipboard
     * @param data Data to copy
     * @return true if copy successful, false otherwise
     */
    Q_INVOKABLE bool copyToClipboard(const QString& data);

    /**
     * @brief Get clipboard data
     * @return Clipboard data
     */
    Q_INVOKABLE QString getClipboard();

    /**
     * @brief Clear clipboard
     * @return true if clear successful, false otherwise
     */
    Q_INVOKABLE bool clearClipboard();

    /**
     * @brief Get clipboard history
     * @return JSON string with clipboard history
     */
    Q_INVOKABLE QString getClipboardHistory() const;

Q_SIGNALS:
    /**
     * @brief Emitted when clipboard changes
     */
    void clipboardChanged();

    /**
     * @brief Emitted when encryption status changes
     */
    void encryptionStatusChanged();

    /**
     * @brief Emitted when timeout changes
     */
    void timeoutChanged();

    /**
     * @brief Emitted when clipboard is cleared due to timeout
     */
    void clipboardTimeout();

private slots:
    /**
     * @brief Handle clipboard timeout
     */
    void onTimeout();

    /**
     * @brief Monitor system clipboard
     */
    void monitorSystemClipboard();

private:
    bool m_initialized;
    bool m_running;
    QString m_currentClipboard;
    bool m_isEncrypted;
    uint32_t m_timeoutSeconds;

    std::unique_ptr<ClipboardEncryption> m_encryption;
    std::unique_ptr<ClipboardHistoryManager> m_history;
    std::unique_ptr<ClipboardIntegration> m_integration;
    QTimer* m_timeoutTimer;
    QTimer* m_monitorTimer;

    /**
     * @brief Update clipboard from system
     */
    void updateClipboardFromSystem();

    /**
     * @brief Update system clipboard
     */
    void updateSystemClipboard(const QString& data);
};

#endif // CLIPBOARD_SERVICE_H

