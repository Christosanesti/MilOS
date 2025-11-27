#include "secrets_service.h"
#include <milos/logging/logger.h>
#include <signal.h>
#include <unistd.h>
#include <systemd/sd-daemon.h>
#include <sstream>

static SecretsService* g_service = nullptr;

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

    // Initialize Logger
    if (!milos::logging::Logger::instance().initialize()) {
        // Logger initialization failed, but continue
    }

    // Create and initialize service
    SecretsService service;
    g_service = &service;

    if (!service.initialize()) {
        LOG_ERROR("Failed to initialize Secrets Management service");
        return 1;
    }

    if (!service.start()) {
        LOG_ERROR("Failed to start Secrets Management service");
        return 1;
    }

    // Notify systemd that service is ready
    sd_notify(0, "READY=1");

    LOG_INFO("MilOS Secrets Management Service started");

    // Main service loop
    while (service.isRunning()) {
        service.performHealthCheck();
        
        // Update systemd watchdog
        sd_notify(0, "WATCHDOG=1");
        
        // Sleep for health check interval
        sleep(30);
    }

    LOG_INFO("MilOS Secrets Management Service stopped");
    return 0;
}

