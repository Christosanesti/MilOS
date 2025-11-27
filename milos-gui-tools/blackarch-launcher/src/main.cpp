#include "launcher.h"
#include <milos/logging/logger.h>
#include <milos/ui/error_handler.h>
#include <milos/ui/crash_handler.h>

int main(int argc, char* argv[]) {
    // Initialize Logger
    if (!milos::logging::Logger::instance().initialize()) {
        // Logger initialization failed, but continue
    }

    // Initialize CrashHandler
    milos::ui::CrashHandler* crashHandler = milos::ui::CrashHandler::instance();
    crashHandler->initialize("blackarch-launcher", "1.0.0");
    crashHandler->installCrashHandlers();

    // Initialize ErrorHandler
    milos::ui::ErrorHandler* errorHandler = milos::ui::ErrorHandler::instance();
    errorHandler->initialize();

    Launcher launcher;
    
    if (!launcher.initialize()) {
        LOG_ERROR("Failed to initialize BlackArch Launcher");
        errorHandler->handleError("Initialization failed",
                                  "Failed to initialize BlackArch Launcher",
                                  milos::ui::ErrorCategory::System,
                                  milos::ui::ErrorSeverity::Critical);
        return 1;
    }

    return launcher.run();
}

