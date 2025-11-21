#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QMap>

/**
 * @brief Configuration Manager
 * 
 * Centralized configuration management for multi-site deployments.
 */
class ConfigManager : public QObject {
    Q_OBJECT

public:
    explicit ConfigManager(QObject* parent = nullptr);
    ~ConfigManager();

    /**
     * @brief Initialize configuration manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get configuration value
     * @param key Configuration key
     * @param defaultValue Default value if not found
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
     * @brief Get all configurations
     * @return Configuration map
     */
    QVariantMap getAllConfigs() const;

    /**
     * @brief Export configuration
     * @param filePath Export file path
     * @return true if export successful, false otherwise
     */
    bool exportConfig(const QString& filePath) const;

    /**
     * @brief Import configuration
     * @param filePath Import file path
     * @return true if import successful, false otherwise
     */
    bool importConfig(const QString& filePath);

Q_SIGNALS:
    /**
     * @brief Emitted when configuration changes
     */
    void configChanged(const QString& key, const QVariant& value);

private:
    QMap<QString, QVariant> m_configs;
    QString m_configFilePath;
    bool loadConfig();
    bool saveConfig() const;
};

#endif // CONFIG_MANAGER_H

