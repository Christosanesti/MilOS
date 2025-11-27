#include "tpm_service.h"
#include <milos/logging/logger.h>
#include <signal.h>
#include <unistd.h>
#include <systemd/sd-daemon.h>
#include <sstream>

static TPMService* g_service = nullptr;

void signalHandler(int signal) {
    if (g_service) {
        std::stringstream ss;
        ss << "Received signal " << signal << ", shutting down...";
        LOG_INFO(ss.str());
        g_service->stop();
    }
}

int main(int argc, char* argv[]) {
    // Register signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Create and initialize service
    TPMService service;
    g_service = &service;

    // Initialize Logger
    if (!milos::logging::Logger::instance().initialize()) {
        // Logger initialization failed, but continue
    }

    if (!service.initialize()) {
        LOG_ERROR("Failed to initialize TPM service");
        return 1;
    }

    if (!service.start()) {
        LOG_ERROR("Failed to start TPM service");
        return 1;
    }

    // Notify systemd that service is ready
    sd_notify(0, "READY=1");

    LOG_INFO("MilOS TPM Integration Service started");

    // Main service loop
    while (service.isRunning()) {
        service.performHealthCheck();
        
        // Update systemd watchdog
        sd_notify(0, "WATCHDOG=1");
        
        // Sleep for health check interval
        sleep(30);
    }

    LOG_INFO("MilOS TPM Integration Service stopped");
    return 0;
}

