#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QSettings>

/**
 * @brief System Configuration
 * 
 * Provides system configuration and policy management interface.
 */
class SystemConfig : public QObject {
    Q_OBJECT

public:
    explicit SystemConfig(QObject* parent = nullptr);
    ~SystemConfig();

    /**
     * @brief Initialize system configuration
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get configuration value
     * @param key Configuration key
     * @param defaultValue Default value
     * @return Configuration value
     */
    QVariant getConfig(const QString& key, const QVariant& defaultValue = QVariant()) const;

    /**
     * @brief Set configuration value
     * @param key Configuration key
     * @param value Configuration value
     * @return true if set successful, false otherwise
     */
    bool setConfig(const QString& key, const QVariant& value);

    /**
     * @brief Get all configuration
     * @return All configuration data
     */
    QVariantMap getAllConfig() const;

    /**
     * @brief Load configuration from file
     * @param filePath Configuration file path
     * @return true if load successful, false otherwise
     */
    bool loadConfig(const QString& filePath);

    /**
     * @brief Save configuration to file
     * @param filePath Configuration file path
     * @return true if save successful, false otherwise
     */
    bool saveConfig(const QString& filePath);

Q_SIGNALS:
    /**
     * @brief Emitted when configuration is changed
     */
    void configChanged(const QString& key, const QVariant& value);

private:
    QSettings* m_settings;
    QVariantMap m_config;
};

#endif // SYSTEM_CONFIG_H

