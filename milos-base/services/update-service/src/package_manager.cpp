#include "package_manager.h"
#include "config_parser.h"
#include <milos/logging/logger.h>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <algorithm>

PackageManager::PackageManager()
    : m_initialized(false)
    , m_configParser(nullptr)
    , m_type("")
    , m_available(false)
{
}

PackageManager::~PackageManager() {
}

bool PackageManager::initialize(ConfigParser* configParser) {
    if (m_initialized) {
        return true;
    }

    m_configParser = configParser;

    // Get package manager type from configuration
    m_type = configParser->getString("package_management.type");
    if (m_type.empty()) {
        m_type = detectPackageManagerType();
    }

    // Check if package manager is available
    m_available = isAvailable();

    if (!m_available) {
        LOG_WARNING("Package manager not available (graceful degradation enabled)");
        // Continue with graceful degradation
    }

    m_initialized = true;
    return true;
}

std::string PackageManager::detectPackageManagerType() {
    // Check for pacman (Arch Linux)
    if (system("which pacman > /dev/null 2>&1") == 0) {
        return "pacman";
    }
    
    // Check for APT (Debian/Ubuntu)
    if (system("which apt > /dev/null 2>&1") == 0) {
        return "apt";
    }
    
    return "unknown";
}

bool PackageManager::isAvailable() const {
    if (m_type == "pacman") {
        return system("which pacman > /dev/null 2>&1") == 0;
    } else if (m_type == "apt") {
        return system("which apt > /dev/null 2>&1") == 0;
    }
    return false;
}

bool PackageManager::executePacman(const std::vector<std::string>& args, std::string& output) {
    if (!isAvailable()) {
        return false;
    }

    std::ostringstream command;
    command << "pacman";
    for (const auto& arg : args) {
        command << " " << arg;
    }
    command << " 2>&1";

    FILE* pipe = popen(command.str().c_str(), "r");
    if (!pipe) {
        return false;
    }

    char buffer[128];
    output.clear();
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }

    int status = pclose(pipe);
    return status == 0;
}

bool PackageManager::executeAPT(const std::vector<std::string>& args, std::string& output) {
    if (!isAvailable()) {
        return false;
    }

    std::ostringstream command;
    command << "apt";
    for (const auto& arg : args) {
        command << " " << arg;
    }
    command << " 2>&1";

    FILE* pipe = popen(command.str().c_str(), "r");
    if (!pipe) {
        return false;
    }

    char buffer[128];
    output.clear();
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }

    int status = pclose(pipe);
    return status == 0;
}

std::vector<PackageInfo> PackageManager::checkUpdates() {
    std::vector<PackageInfo> updates;

    if (!m_available || !m_initialized) {
        return updates;
    }

    if (m_type == "pacman") {
        std::string output;
        if (executePacman({"-Qu"}, output)) {
            // Parse output for packages with updates
            std::istringstream stream(output);
            std::string line;
            while (std::getline(stream, line)) {
                if (line.empty()) continue;
                
                // Format: package_name old_version -> new_version
                size_t arrowPos = line.find(" -> ");
                if (arrowPos != std::string::npos) {
                    PackageInfo info;
                    size_t spacePos = line.find(' ');
                    if (spacePos != std::string::npos) {
                        info.name = line.substr(0, spacePos);
                        info.installed_version = line.substr(spacePos + 1, arrowPos - spacePos - 1);
                        info.version = line.substr(arrowPos + 4);
                        info.has_update = true;
                        info.is_installed = true;
                        updates.push_back(info);
                    }
                }
            }
        }
    } else if (m_type == "apt") {
        // Refresh package lists first
        refreshRepositories();
        
        std::string output;
        if (executeAPT({"list", "--upgradable"}, output)) {
            // Parse output for upgradable packages
            std::istringstream stream(output);
            std::string line;
            while (std::getline(stream, line)) {
                if (line.empty() || line.find("Listing...") != std::string::npos) continue;
                
                // Format: package_name/now version [upgradable from: version]
                size_t spacePos = line.find(' ');
                if (spacePos != std::string::npos) {
                    PackageInfo info;
                    size_t slashPos = line.find('/');
                    if (slashPos != std::string::npos) {
                        info.name = line.substr(0, slashPos);
                        size_t versionStart = spacePos + 1;
                        size_t versionEnd = line.find(' ', versionStart);
                        if (versionEnd == std::string::npos) {
                            versionEnd = line.length();
                        }
                        info.version = line.substr(versionStart, versionEnd - versionStart);
                        info.has_update = true;
                        info.is_installed = true;
                        updates.push_back(info);
                    }
                }
            }
        }
    }

    return updates;
}

std::vector<PackageInfo> PackageManager::getInstalledPackages() {
    std::vector<PackageInfo> packages;

    if (!m_available || !m_initialized) {
        return packages;
    }

    if (m_type == "pacman") {
        std::string output;
        if (executePacman({"-Q"}, output)) {
            packages = parsePacmanOutput(output);
        }
    } else if (m_type == "apt") {
        std::string output;
        if (executeAPT({"list", "--installed"}, output)) {
            packages = parseAPTOutput(output);
        }
    }

    return packages;
}

std::unique_ptr<PackageInfo> PackageManager::getPackageInfo(const std::string& packageName) {
    if (!m_available || !m_initialized) {
        return nullptr;
    }

    // Check if package is installed
    std::vector<PackageInfo> installed = getInstalledPackages();
    for (const auto& pkg : installed) {
        if (pkg.name == packageName) {
            return std::make_unique<PackageInfo>(pkg);
        }
    }

    return nullptr;
}

bool PackageManager::downloadPackage(const std::string& packageName, const std::string& destinationPath) {
    if (!m_available || !m_initialized) {
        return false;
    }

    if (m_type == "pacman") {
        // pacman -Sw downloads package without installing
        std::string output;
        return executePacman({"-Sw", "--noconfirm", packageName}, output);
    } else if (m_type == "apt") {
        // apt download downloads package
        std::string output;
        std::ostringstream command;
        command << "cd " << destinationPath << " && apt download " << packageName << " 2>&1";
        FILE* pipe = popen(command.str().c_str(), "r");
        if (!pipe) {
            return false;
        }
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            // Read output
        }
        int status = pclose(pipe);
        return status == 0;
    }

    return false;
}

bool PackageManager::installPackage(const std::string& packageName) {
    return installPackages({packageName});
}

bool PackageManager::installPackages(const std::vector<std::string>& packageList) {
    if (!m_available || !m_initialized || packageList.empty()) {
        return false;
    }

    if (m_type == "pacman") {
        std::vector<std::string> args = {"-S", "--noconfirm", "--needed"};
        args.insert(args.end(), packageList.begin(), packageList.end());
        std::string output;
        return executePacman(args, output);
    } else if (m_type == "apt") {
        std::vector<std::string> args = {"install", "-y"};
        args.insert(args.end(), packageList.begin(), packageList.end());
        std::string output;
        return executeAPT(args, output);
    }

    return false;
}

bool PackageManager::removePackage(const std::string& packageName) {
    if (!m_available || !m_initialized) {
        return false;
    }

    if (m_type == "pacman") {
        std::string output;
        return executePacman({"-R", "--noconfirm", packageName}, output);
    } else if (m_type == "apt") {
        std::string output;
        return executeAPT({"remove", "-y", packageName}, output);
    }

    return false;
}

bool PackageManager::refreshRepositories() {
    if (!m_available || !m_initialized) {
        return false;
    }

    if (m_type == "pacman") {
        std::string output;
        return executePacman({"-Sy"}, output);
    } else if (m_type == "apt") {
        std::string output;
        return executeAPT({"update"}, output);
    }

    return false;
}

std::vector<PackageInfo> PackageManager::parsePacmanOutput(const std::string& output) {
    std::vector<PackageInfo> packages;
    std::istringstream stream(output);
    std::string line;
    
    while (std::getline(stream, line)) {
        if (line.empty()) continue;
        
        PackageInfo info;
        size_t spacePos = line.find(' ');
        if (spacePos != std::string::npos) {
            info.name = line.substr(0, spacePos);
            size_t versionStart = spacePos + 1;
            size_t versionEnd = line.find(' ', versionStart);
            if (versionEnd == std::string::npos) {
                versionEnd = line.length();
            }
            info.version = line.substr(versionStart, versionEnd - versionStart);
            info.installed_version = info.version;
            info.is_installed = true;
            packages.push_back(info);
        }
    }
    
    return packages;
}

std::vector<PackageInfo> PackageManager::parseAPTOutput(const std::string& output) {
    std::vector<PackageInfo> packages;
    std::istringstream stream(output);
    std::string line;
    
    while (std::getline(stream, line)) {
        if (line.empty() || line.find("Listing...") != std::string::npos) continue;
        
        PackageInfo info;
        size_t spacePos = line.find(' ');
        if (spacePos != std::string::npos) {
            size_t slashPos = line.find('/');
            if (slashPos != std::string::npos) {
                info.name = line.substr(0, slashPos);
                size_t versionStart = spacePos + 1;
                size_t versionEnd = line.find(' ', versionStart);
                if (versionEnd == std::string::npos) {
                    versionEnd = line.length();
                }
                info.version = line.substr(versionStart, versionEnd - versionStart);
                info.installed_version = info.version;
                info.is_installed = true;
                packages.push_back(info);
            }
        }
    }
    
    return packages;
}

