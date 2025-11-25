#include "help_manager.h"
#include <sstream>
#include <algorithm>

HelpManager& HelpManager::getInstance() {
    static HelpManager instance;
    static bool initialized = false;
    if (!initialized) {
        instance.loadDefaultHelpTexts();
        instance.loadDefaultErrorMessages();
        initialized = true;
    }
    return instance;
}

void HelpManager::registerHelpText(const std::string& toolName, const std::string& command, const HelpText& help) {
    m_helpTexts[toolName][command] = help;
}

HelpManager::HelpText HelpManager::getHelpText(const std::string& toolName, const std::string& command) {
    if (m_helpTexts.find(toolName) != m_helpTexts.end()) {
        if (command.empty()) {
            // Return general help
            if (m_helpTexts[toolName].find("") != m_helpTexts[toolName].end()) {
                return m_helpTexts[toolName][""];
            }
        } else {
            if (m_helpTexts[toolName].find(command) != m_helpTexts[toolName].end()) {
                return m_helpTexts[toolName][command];
            }
        }
    }
    return HelpText();
}

std::string HelpManager::formatHelpText(const std::string& toolName, const std::string& command) {
    HelpText help = getHelpText(toolName, command);
    if (help.description.empty()) {
        return "No help available for " + toolName + (command.empty() ? "" : " " + command);
    }
    
    std::ostringstream oss;
    oss << "DESCRIPTION:\n";
    oss << "  " << help.description << "\n\n";
    
    if (!help.usagePatterns.empty()) {
        oss << "USAGE:\n";
        for (const auto& pattern : help.usagePatterns) {
            oss << "  " << pattern << "\n";
        }
        oss << "\n";
    }
    
    if (!help.examples.empty()) {
        oss << "EXAMPLES:\n";
        for (const auto& example : help.examples) {
            oss << "  " << example << "\n";
        }
        oss << "\n";
    }
    
    if (!help.options.empty()) {
        oss << "OPTIONS:\n";
        for (const auto& opt : help.options) {
            oss << "  " << opt.first << "\n";
            oss << "    " << opt.second << "\n";
        }
    }
    
    return oss.str();
}

void HelpManager::registerErrorMessage(const std::string& errorCode, const ErrorMessage& message) {
    m_errorMessages[errorCode] = message;
}

HelpManager::ErrorMessage HelpManager::getErrorMessage(const std::string& errorCode) {
    if (m_errorMessages.find(errorCode) != m_errorMessages.end()) {
        return m_errorMessages[errorCode];
    }
    return ErrorMessage();
}

std::string HelpManager::formatErrorMessage(const std::string& errorCode, const std::map<std::string, std::string>& context) {
    ErrorMessage msg = getErrorMessage(errorCode);
    if (msg.error.empty()) {
        return "Error: " + errorCode;
    }
    
    std::ostringstream oss;
    oss << "ERROR: " << msg.error << "\n";
    
    if (!msg.suggestion.empty()) {
        oss << "SUGGESTION: " << msg.suggestion << "\n";
    }
    
    if (!msg.troubleshootingTips.empty()) {
        oss << "TROUBLESHOOTING:\n";
        for (const auto& tip : msg.troubleshootingTips) {
            oss << "  - " << tip << "\n";
        }
    }
    
    if (!msg.fix.empty()) {
        oss << "FIX: " << msg.fix << "\n";
    }
    
    return oss.str();
}

std::string HelpManager::getContextHelp(const std::string& toolName, const std::string& command, const std::string& subcommand) {
    if (!subcommand.empty()) {
        return formatHelpText(toolName, command + " " + subcommand);
    }
    return formatHelpText(toolName, command);
}

std::string HelpManager::getQuickReference(const std::string& toolName) {
    if (toolName.empty()) {
        // General quick reference
        std::ostringstream oss;
        oss << "MilOS CLI Tools Quick Reference\n";
        oss << "==============================\n\n";
        oss << "Common Commands:\n";
        oss << "  milos-audit <command>     - Audit service management\n";
        oss << "  milos-data-guard <command> - Data transmission guard\n";
        oss << "  milos-fim <command>        - File integrity monitoring\n";
        oss << "  milos-tpm <command>        - TPM integration\n";
        oss << "  milos-update <command>     - Update service\n";
        oss << "\n";
        oss << "For help on a specific tool, use: <tool> --help\n";
        oss << "For help on a command, use: <tool> <command> --help\n";
        return oss.str();
    } else {
        return formatHelpText(toolName, "");
    }
}

void HelpManager::loadDefaultHelpTexts() {
    // Add default help texts for common tools
    // This can be extended with tool-specific help
}

void HelpManager::loadDefaultErrorMessages() {
    // Service connection errors
    ErrorMessage serviceError;
    serviceError.error = "Cannot connect to service";
    serviceError.suggestion = "Make sure the service is running";
    serviceError.troubleshootingTips = {
        "Check service status: systemctl status <service-name>",
        "Start the service: systemctl start <service-name>",
        "Check D-Bus connection: dbus-send --system --print-reply --dest=org.milos.<Service> /org/milos/<Service> org.freedesktop.DBus.Properties.GetAll string:''"
    };
    serviceError.fix = "systemctl start <service-name>";
    registerErrorMessage("SERVICE_CONNECTION_ERROR", serviceError);
    
    // Validation errors
    ErrorMessage validationError;
    validationError.error = "Invalid input parameter";
    validationError.suggestion = "Check the parameter format and requirements";
    validationError.troubleshootingTips = {
        "Review the command syntax: <command> --help",
        "Check parameter types and constraints",
        "Verify required parameters are provided"
    };
    validationError.fix = "Use --help to see correct usage";
    registerErrorMessage("VALIDATION_ERROR", validationError);
    
    // Permission errors
    ErrorMessage permissionError;
    permissionError.error = "Permission denied";
    permissionError.suggestion = "This operation requires elevated privileges";
    permissionError.troubleshootingTips = {
        "Run the command with sudo if required",
        "Check user permissions and group membership",
        "Verify service permissions in systemd unit file"
    };
    permissionError.fix = "Use sudo or ensure proper permissions";
    registerErrorMessage("PERMISSION_ERROR", permissionError);
}

