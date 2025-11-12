#ifndef CLIPBOARD_MANAGER_H
#define CLIPBOARD_MANAGER_H

#include <QObject>
#include <memory>

// Forward declarations
class ClipboardService;
class DBusInterface;
class ConfigParser;

/**
 * @brief Clipboard Manager
 * 
 * Main application class for Secure Clipboard Manager.
 */
class ClipboardManager : public QObject {
    Q_OBJECT

public:
    explicit ClipboardManager(QObject* parent = nullptr);
    ~ClipboardManager();

    /**
     * @brief Initialize clipboard manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Start clipboard manager
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop clipboard manager
     */
    void stop();

    /**
     * @brief Get Clipboard Service instance
     * @return Clipboard Service instance
     */
    ClipboardService* getClipboardService() const { return m_clipboardService.get(); }

private:
    bool m_initialized;
    bool m_running;

    std::unique_ptr<ConfigParser> m_configParser;
    std::unique_ptr<ClipboardService> m_clipboardService;
    std::unique_ptr<DBusInterface> m_dbusInterface;
};

#endif // CLIPBOARD_MANAGER_H

