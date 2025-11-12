#ifndef DBUS_INTERFACE_H
#define DBUS_INTERFACE_H

#include <QObject>
#include <QString>

// Forward declaration
class ClipboardService;

/**
 * @brief D-Bus Interface for Clipboard Manager
 * 
 * Implements org.milos.ClipboardManager D-Bus interface.
 */
class DBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.ClipboardManager")

public:
    DBusInterface(QObject* parent = nullptr);
    ~DBusInterface();

    /**
     * @brief Initialize D-Bus interface
     * @param clipboardService Clipboard service instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(ClipboardService* clipboardService);

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

public Q_SLOTS:
    // D-Bus methods
    /**
     * @brief Copy data to clipboard
     * @param data Data to copy
     * @return true if copy successful, false otherwise
     */
    bool CopyToClipboard(const QString& data);

    /**
     * @brief Get clipboard data
     * @return Clipboard data
     */
    QString GetClipboard();

    /**
     * @brief Clear clipboard
     * @return true if clear successful, false otherwise
     */
    bool ClearClipboard();

    /**
     * @brief Get clipboard history
     * @return JSON string with clipboard history
     */
    QString GetClipboardHistory();

Q_SIGNALS:
    /**
     * @brief Signal emitted when clipboard changes
     */
    void ClipboardChanged(const QString& data);

    /**
     * @brief Signal emitted when clipboard is cleared
     */
    void ClipboardCleared();
};

#endif // DBUS_INTERFACE_H

