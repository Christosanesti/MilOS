#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <string>
#include <map>

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
     * @param configPath Configuration file path
     * @return true if load successful, false otherwise
     */
    bool loadConfig(const std::string& configPath);

    /**
     * @brief Get configuration value
     * @param key Configuration key (dot-separated path)
     * @param defaultValue Default value if not found
     * @return Configuration value, or default
     */
    std::string getConfig(const std::string& key, const std::string& defaultValue = "") const;

    /**
     * @brief Get integer configuration value
     * @param key Configuration key
     * @param defaultValue Default value if not found
     * @return Configuration value, or default
     */
    int getConfigInt(const std::string& key, int defaultValue = 0) const;

    /**
     * @brief Get boolean configuration value
     * @param key Configuration key
     * @param defaultValue Default value if not found
     * @return Configuration value, or default
     */
    bool getConfigBool(const std::string& key, bool defaultValue = false) const;

private:
    std::map<std::string, std::string> m_config;
    bool m_loaded;

    /**
     * @brief Parse YAML file (simplified)
     * @param configPath Configuration file path
     * @return true if parse successful, false otherwise
     */
    bool parseYAML(const std::string& configPath);
};

#endif // CONFIG_PARSER_H

