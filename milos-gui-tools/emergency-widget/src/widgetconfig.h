#ifndef WIDGETCONFIG_H
#define WIDGETCONFIG_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QSettings>

/**
 * @brief Configuration manager for emergency widget
 * 
 * Manages widget configuration settings with YAML persistence
 */
class WidgetConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool showNetworkButton READ showNetworkButton WRITE setShowNetworkButton NOTIFY showNetworkButtonChanged)
    Q_PROPERTY(bool showLockButton READ showLockButton WRITE setShowLockButton NOTIFY showLockButtonChanged)
    Q_PROPERTY(bool showSecureModeButton READ showSecureModeButton WRITE setShowSecureModeButton NOTIFY showSecureModeButtonChanged)
    Q_PROPERTY(bool showShutdownButton READ showShutdownButton WRITE setShowShutdownButton NOTIFY showShutdownButtonChanged)
    Q_PROPERTY(bool requireConfirmations READ requireConfirmations WRITE setRequireConfirmations NOTIFY requireConfirmationsChanged)
    Q_PROPERTY(QString keyboardShortcut READ keyboardShortcut WRITE setKeyboardShortcut NOTIFY keyboardShortcutChanged)

public:
    explicit WidgetConfig(QObject* parent = nullptr);
    ~WidgetConfig();

    bool showNetworkButton() const { return m_showNetworkButton; }
    void setShowNetworkButton(bool show);
    
    bool showLockButton() const { return m_showLockButton; }
    void setShowLockButton(bool show);
    
    bool showSecureModeButton() const { return m_showSecureModeButton; }
    void setShowSecureModeButton(bool show);
    
    bool showShutdownButton() const { return m_showShutdownButton; }
    void setShowShutdownButton(bool show);
    
    bool requireConfirmations() const { return m_requireConfirmations; }
    void setRequireConfirmations(bool require);
    
    QString keyboardShortcut() const { return m_keyboardShortcut; }
    void setKeyboardShortcut(const QString& shortcut);

    Q_INVOKABLE void loadConfig();
    Q_INVOKABLE void saveConfig();
    Q_INVOKABLE QVariantMap getConfig() const;

signals:
    void showNetworkButtonChanged();
    void showLockButtonChanged();
    void showSecureModeButtonChanged();
    void showShutdownButtonChanged();
    void requireConfirmationsChanged();
    void keyboardShortcutChanged();
    void configLoaded();
    void configSaved();

private:
    QString getConfigPath() const;
    void setDefaults();

    bool m_showNetworkButton;
    bool m_showLockButton;
    bool m_showSecureModeButton;
    bool m_showShutdownButton;
    bool m_requireConfirmations;
    QString m_keyboardShortcut;
    QSettings* m_settings;
};

#endif // WIDGETCONFIG_H

