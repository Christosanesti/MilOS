#include "fim_service.h"
#include <milos/logging/logger.h>
#include <signal.h>
#include <unistd.h>
#include <systemd/sd-daemon.h>

static FIMService* g_service = nullptr;

void signalHandler(int signal) {
    if (g_service) {
        LOG_INFO(QString("Received signal %1, shutting down...").arg(signal));
        g_service->stop();
    }
}

int main(int argc, char* argv[]) {
    // Register signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Create and initialize service
    FIMService service;
    g_service = &service;

    // Initialize logger
    Logger::instance()->initialize("milos-file-integrity-monitoring",
                                   "org.milos.AuditService",
                                   "/org/milos/AuditService",
                                   Logger::Info,
                                   true);

    if (!service.initialize()) {
        LOG_ERROR("Failed to initialize FIM service");
        return 1;
    }

    if (!service.start()) {
        LOG_ERROR("Failed to start FIM service");
        return 1;
    }

    // Notify systemd that service is ready
    sd_notify(0, "READY=1");

    LOG_INFO("MilOS File Integrity Monitoring Service started");

    // Main service loop
    while (service.isRunning()) {
        service.performHealthCheck();
        
        // Update systemd watchdog
        sd_notify(0, "WATCHDOG=1");
        
        // Sleep for health check interval
        sleep(30);
    }

    LOG_INFO("MilOS File Integrity Monitoring Service stopped");
    return 0;
}

