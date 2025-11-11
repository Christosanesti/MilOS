#ifndef PACKAGE_MANAGER_H
#define PACKAGE_MANAGER_H

#include <memory>
#include <string>
#include <vector>
#include <map>

class ConfigParser;

/**
 * @brief Package Information Structure
 */
struct PackageInfo {
    std::string name;
    std::string version;
    std::string installed_version;
    std::string repository;
    std::string description;
    bool is_installed;
    bool has_update;
};

/**
 * @brief Package Management Abstraction
 * 
 * Provides unified interface for package management operations
 * across different package managers (pacman, APT).
 */
class PackageManager {
public:
    PackageManager();
    ~PackageManager();

    /**
     * @brief Initialize package manager
     * @param configParser Configuration parser instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(ConfigParser* configParser);

    /**
     * @brief Check for available updates
     * @return Vector of packages with available updates
     */
    std::vector<PackageInfo> checkUpdates();

    /**
     * @brief Get installed packages
     * @return Vector of installed packages
     */
    std::vector<PackageInfo> getInstalledPackages();

    /**
     * @brief Get package information
     * @param packageName Package name
     * @return Package information, nullptr if not found
     */
    std::unique_ptr<PackageInfo> getPackageInfo(const std::string& packageName);

    /**
     * @brief Download package (without installing)
     * @param packageName Package name
     * @param destinationPath Destination path for downloaded package
     * @return true if download successful, false otherwise
     */
    bool downloadPackage(const std::string& packageName, const std::string& destinationPath);

    /**
     * @brief Install package
     * @param packageName Package name
     * @return true if installation successful, false otherwise
     */
    bool installPackage(const std::string& packageName);

    /**
     * @brief Install packages from list
     * @param packageList List of package names
     * @return true if installation successful, false otherwise
     */
    bool installPackages(const std::vector<std::string>& packageList);

    /**
     * @brief Remove package
     * @param packageName Package name
     * @return true if removal successful, false otherwise
     */
    bool removePackage(const std::string& packageName);

    /**
     * @brief Refresh repository cache
     * @return true if refresh successful, false otherwise
     */
    bool refreshRepositories();

    /**
     * @brief Check if package manager is available
     * @return true if available, false otherwise
     */
    bool isAvailable() const;

    /**
     * @brief Get package manager type
     * @return Package manager type (pacman, apt)
     */
    std::string getType() const { return m_type; }

private:
    bool m_initialized;
    ConfigParser* m_configParser;
    std::string m_type;  // pacman, apt
    bool m_available;

    /**
     * @brief Detect package manager type
     * @return Package manager type (pacman, apt)
     */
    std::string detectPackageManagerType();

    /**
     * @brief Execute pacman command
     * @param args Command arguments
     * @param output Command output
     * @return true if command successful, false otherwise
     */
    bool executePacman(const std::vector<std::string>& args, std::string& output);

    /**
     * @brief Execute APT command
     * @param args Command arguments
     * @param output Command output
     * @return true if command successful, false otherwise
     */
    bool executeAPT(const std::vector<std::string>& args, std::string& output);

    /**
     * @brief Parse pacman package list
     * @param output Command output
     * @return Vector of package information
     */
    std::vector<PackageInfo> parsePacmanOutput(const std::string& output);

    /**
     * @brief Parse APT package list
     * @param output Command output
     * @return Vector of package information
     */
    std::vector<PackageInfo> parseAPTOutput(const std::string& output);
};

#endif // PACKAGE_MANAGER_H

