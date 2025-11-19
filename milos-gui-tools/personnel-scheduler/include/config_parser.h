#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <QString>
#include <QVariantMap>
#include <memory>

/**
 * @brief Configuration Parser
 * 
 * Parses YAML configuration files for device settings.
 */
class ConfigParser {
public:
    /**
     * @brief Load configuration from file
     * @param filePath Configuration file path
     * @return true if load successful, false otherwise
     */
    bool load(const QString& filePath);

    /**
     * @brief Get device configuration
     * @param deviceId Device ID
     * @return Device configuration map
     */
    QVariantMap getDeviceConfig(const QString& deviceId) const;

    /**
     * @brief Get all device configurations
     * @return Map of device configurations
     */
    QMap<QString, QVariantMap> getAllDeviceConfigs() const;

    /**
     * @brief Set device configuration
     * @param deviceId Device ID
     * @param config Configuration map
     */
    void setDeviceConfig(const QString& deviceId, const QVariantMap& config);

    /**
     * @brief Save configuration to file
     * @param filePath Configuration file path
     * @return true if save successful, false otherwise
     */
    bool save(const QString& filePath) const;

private:
    QMap<QString, QVariantMap> m_deviceConfigs;
    QString m_configFilePath;
};

#endif // CONFIG_PARSER_H


