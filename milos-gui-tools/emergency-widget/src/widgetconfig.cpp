#include "widgetconfig.h"
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QTextStream>

WidgetConfig::WidgetConfig(QObject* parent)
    : QObject(parent)
    , m_showNetworkButton(true)
    , m_showLockButton(true)
    , m_showSecureModeButton(true)
    , m_showShutdownButton(true)
    , m_requireConfirmations(true)
    , m_keyboardShortcut("Alt+E")
    , m_settings(nullptr)
{
    setDefaults();
    loadConfig();
}

WidgetConfig::~WidgetConfig()
{
    if (m_settings) {
        delete m_settings;
    }
}

void WidgetConfig::setDefaults()
{
    m_showNetworkButton = true;
    m_showLockButton = true;
    m_showSecureModeButton = true;
    m_showShutdownButton = true;
    m_requireConfirmations = true;
    m_keyboardShortcut = "Alt+E";
}

QString WidgetConfig::getConfigPath() const
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos";
    QDir().mkpath(configDir);
    return configDir + "/emergency-widget.yaml";
}

void WidgetConfig::loadConfig()
{
    QString configPath = getConfigPath();
    
    // Use QSettings for simple key-value storage (YAML-like structure)
    // For full YAML support, would need yaml-cpp integration
    QString settingsPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos/emergency-widget.ini";
    m_settings = new QSettings(settingsPath, QSettings::IniFormat, this);
    
    m_settings->beginGroup("Widget");
    m_showNetworkButton = m_settings->value("showNetworkButton", true).toBool();
    m_showLockButton = m_settings->value("showLockButton", true).toBool();
    m_showSecureModeButton = m_settings->value("showSecureModeButton", true).toBool();
    m_showShutdownButton = m_settings->value("showShutdownButton", true).toBool();
    m_requireConfirmations = m_settings->value("requireConfirmations", true).toBool();
    m_keyboardShortcut = m_settings->value("keyboardShortcut", "Alt+E").toString();
    m_settings->endGroup();
    
    emit configLoaded();
}

void WidgetConfig::saveConfig()
{
    if (!m_settings) {
        QString settingsPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/milos/emergency-widget.ini";
        m_settings = new QSettings(settingsPath, QSettings::IniFormat, this);
    }
    
    m_settings->beginGroup("Widget");
    m_settings->setValue("showNetworkButton", m_showNetworkButton);
    m_settings->setValue("showLockButton", m_showLockButton);
    m_settings->setValue("showSecureModeButton", m_showSecureModeButton);
    m_settings->setValue("showShutdownButton", m_showShutdownButton);
    m_settings->setValue("requireConfirmations", m_requireConfirmations);
    m_settings->setValue("keyboardShortcut", m_keyboardShortcut);
    m_settings->endGroup();
    
    m_settings->sync();
    
    // Also save as YAML for compatibility
    QString yamlPath = getConfigPath();
    QFile yamlFile(yamlPath);
    if (yamlFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&yamlFile);
        out << "widget:\n";
        out << "  show_network_button: " << (m_showNetworkButton ? "true" : "false") << "\n";
        out << "  show_lock_button: " << (m_showLockButton ? "true" : "false") << "\n";
        out << "  show_secure_mode_button: " << (m_showSecureModeButton ? "true" : "false") << "\n";
        out << "  show_shutdown_button: " << (m_showShutdownButton ? "true" : "false") << "\n";
        out << "  require_confirmations: " << (m_requireConfirmations ? "true" : "false") << "\n";
        out << "  keyboard_shortcut: \"" << m_keyboardShortcut << "\"\n";
    }
    
    emit configSaved();
}

QVariantMap WidgetConfig::getConfig() const
{
    QVariantMap config;
    config["showNetworkButton"] = m_showNetworkButton;
    config["showLockButton"] = m_showLockButton;
    config["showSecureModeButton"] = m_showSecureModeButton;
    config["showShutdownButton"] = m_showShutdownButton;
    config["requireConfirmations"] = m_requireConfirmations;
    config["keyboardShortcut"] = m_keyboardShortcut;
    return config;
}

void WidgetConfig::setShowNetworkButton(bool show)
{
    if (m_showNetworkButton != show) {
        m_showNetworkButton = show;
        emit showNetworkButtonChanged();
    }
}

void WidgetConfig::setShowLockButton(bool show)
{
    if (m_showLockButton != show) {
        m_showLockButton = show;
        emit showLockButtonChanged();
    }
}

void WidgetConfig::setShowSecureModeButton(bool show)
{
    if (m_showSecureModeButton != show) {
        m_showSecureModeButton = show;
        emit showSecureModeButtonChanged();
    }
}

void WidgetConfig::setShowShutdownButton(bool show)
{
    if (m_showShutdownButton != show) {
        m_showShutdownButton = show;
        emit showShutdownButtonChanged();
    }
}

void WidgetConfig::setRequireConfirmations(bool require)
{
    if (m_requireConfirmations != require) {
        m_requireConfirmations = require;
        emit requireConfirmationsChanged();
    }
}

void WidgetConfig::setKeyboardShortcut(const QString& shortcut)
{
    if (m_keyboardShortcut != shortcut) {
        m_keyboardShortcut = shortcut;
        emit keyboardShortcutChanged();
    }
}

