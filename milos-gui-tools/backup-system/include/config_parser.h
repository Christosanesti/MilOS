#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <string>

/**
 * @brief Configuration Parser
 * 
 * Parses YAML configuration files.
 */
class ConfigParser {
public:
    ConfigParser();
    ~ConfigParser();

    /**
     * @brief Load configuration from file
     * @param configPath Path to configuration file
     * @return true if load successful, false otherwise
     */
    bool load(const std::string& configPath);

    /**
     * @brief Get configuration value
     * @param key Configuration key (dot-separated path)
     * @return Configuration value as string, empty if not found
     */
    std::string getString(const std::string& key) const;

    /**
     * @brief Get configuration value as boolean
     * @param key Configuration key
     * @param defaultValue Default value if not found
     * @return Configuration value as boolean
     */
    bool getBool(const std::string& key, bool defaultValue = false) const;

    /**
     * @brief Get configuration value as integer
     * @param key Configuration key
     * @param defaultValue Default value if not found
     * @return Configuration value as integer
     */
    int getInt(const std::string& key, int defaultValue = 0) const;

    /**
     * @brief Check if configuration is loaded
     * @return true if loaded, false otherwise
     */
    bool isLoaded() const { return m_loaded; }

private:
    bool m_loaded;
    std::string m_configPath;
    void* m_yamlRoot;  // Pointer to YAML::Node

    /**
     * @brief Parse YAML file
     */
    bool parseYaml(const std::string& configPath);
};

#endif // CONFIG_PARSER_H

