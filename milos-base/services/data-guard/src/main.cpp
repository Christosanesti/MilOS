#include "dataguard_service.h"
#include <systemd/sd-daemon.h>
#include <iostream>
#include <csignal>
#include <unistd.h>

static DataGuardService* g_service = nullptr;

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
    DataGuardService service;
    g_service = &service;

    if (!service.initialize()) {
        std::cerr << "Failed to initialize Data Transmission Guard Service" << std::endl;
        sd_notify(0, "STATUS=Failed to initialize\n");
        return 1;
    }

    // Start service
    if (!service.start()) {
        std::cerr << "Failed to start Data Transmission Guard Service" << std::endl;
        sd_notify(0, "STATUS=Failed to start\n");
        return 1;
    }

    // Main service loop
    while (service.isRunning() && service.isHealthy()) {
        sleep(1);
    }

    // Service stopped or unhealthy
    if (!service.isHealthy()) {
        std::cerr << "Service became unhealthy, exiting" << std::endl;
        sd_notify(0, "STATUS=Service unhealthy\n");
        return 1;
    }

    return 0;
}

