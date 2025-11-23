#include "fim_service.h"
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <systemd/sd-daemon.h>

static FIMService* g_service = nullptr;

void signalHandler(int signal) {
    if (g_service) {
        std::cout << "Received signal " << signal << ", shutting down..." << std::endl;
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

    if (!service.initialize()) {
        std::cerr << "Failed to initialize FIM service" << std::endl;
        return 1;
    }

    if (!service.start()) {
        std::cerr << "Failed to start FIM service" << std::endl;
        return 1;
    }

    // Notify systemd that service is ready
    sd_notify(0, "READY=1");

    std::cout << "MilOS File Integrity Monitoring Service started" << std::endl;

    // Main service loop
    while (service.isRunning()) {
        service.performHealthCheck();
        
        // Update systemd watchdog
        sd_notify(0, "WATCHDOG=1");
        
        // Sleep for health check interval
        sleep(30);
    }

    std::cout << "MilOS File Integrity Monitoring Service stopped" << std::endl;
    return 0;
}

