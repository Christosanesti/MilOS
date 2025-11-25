#ifndef KEYBOARDSHORTCUT_H
#define KEYBOARDSHORTCUT_H

#include <QObject>
#include <QString>
#include <QKeySequence>
#include <QShortcut>

/**
 * @brief Global keyboard shortcut handler for emergency widget
 * 
 * Registers and manages global keyboard shortcuts using KDE Plasma framework
 */
class KeyboardShortcut : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString shortcut READ shortcut WRITE setShortcut NOTIFY shortcutChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

public:
    explicit KeyboardShortcut(QObject* parent = nullptr);
    ~KeyboardShortcut();

    QString shortcut() const { return m_shortcut; }
    void setShortcut(const QString& shortcut);
    
    bool enabled() const { return m_enabled; }
    void setEnabled(bool enabled);

    Q_INVOKABLE void registerShortcut();
    Q_INVOKABLE void unregisterShortcut();

signals:
    void shortcutChanged();
    void enabledChanged();
    void activated();
    void error(const QString& errorMessage);

private slots:
    void onShortcutActivated();

private:
    QString m_shortcut;
    bool m_enabled;
    QShortcut* m_qtShortcut;
    
    void createXenonFlash();
};

#endif // KEYBOARDSHORTCUT_H

