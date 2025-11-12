#ifndef CLIPBOARD_INTEGRATION_H
#define CLIPBOARD_INTEGRATION_H

#include <QObject>
#include <QString>
#include <string>

/**
 * @brief Clipboard Integration
 * 
 * Handles integration with system clipboard (X11/Wayland).
 */
class ClipboardIntegration : public QObject {
    Q_OBJECT

public:
    explicit ClipboardIntegration(QObject* parent = nullptr);
    ~ClipboardIntegration();

    /**
     * @brief Initialize clipboard integration
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get clipboard text from system
     * @return Clipboard text
     */
    QString getClipboardText() const;

    /**
     * @brief Set clipboard text to system
     * @param text Text to set
     * @return true if set successful, false otherwise
     */
    bool setClipboardText(const QString& text);

    /**
     * @brief Clear system clipboard
     * @return true if clear successful, false otherwise
     */
    bool clearClipboard();

    /**
     * @brief Check if clipboard integration is available
     * @return true if available, false otherwise
     */
    bool isAvailable() const { return m_available; }

Q_SIGNALS:
    /**
     * @brief Emitted when system clipboard changes
     */
    void clipboardChanged(const QString& text);

private:
    bool m_initialized;
    bool m_available;
};

#endif // CLIPBOARD_INTEGRATION_H

