#ifndef HELP_MANAGER_H
#define HELP_MANAGER_H

#include <string>
#include <vector>
#include <map>

/**
 * @brief Help Manager for CLI Tools
 * 
 * Provides enhanced help text, error messages, and context-sensitive help
 */
class HelpManager {
public:
    struct HelpText {
        std::string description;
        std::vector<std::string> examples;
        std::vector<std::string> usagePatterns;
        std::map<std::string, std::string> options;
    };
    
    struct ErrorMessage {
        std::string error;
        std::string suggestion;
        std::vector<std::string> troubleshootingTips;
        std::string fix;
    };
    
    static HelpManager& getInstance();
    
    // Help text management
    void registerHelpText(const std::string& toolName, const std::string& command, const HelpText& help);
    HelpText getHelpText(const std::string& toolName, const std::string& command = "");
    std::string formatHelpText(const std::string& toolName, const std::string& command = "");
    
    // Error message management
    void registerErrorMessage(const std::string& errorCode, const ErrorMessage& message);
    ErrorMessage getErrorMessage(const std::string& errorCode);
    std::string formatErrorMessage(const std::string& errorCode, const std::map<std::string, std::string>& context = {});
    
    // Context-sensitive help
    std::string getContextHelp(const std::string& toolName, const std::string& command, const std::string& subcommand = "");
    
    // Quick reference
    std::string getQuickReference(const std::string& toolName = "");

private:
    HelpManager() = default;
    ~HelpManager() = default;
    HelpManager(const HelpManager&) = delete;
    HelpManager& operator=(const HelpManager&) = delete;
    
    std::map<std::string, std::map<std::string, HelpText>> m_helpTexts;
    std::map<std::string, ErrorMessage> m_errorMessages;
    
    void loadDefaultHelpTexts();
    void loadDefaultErrorMessages();
};

#endif // HELP_MANAGER_H

