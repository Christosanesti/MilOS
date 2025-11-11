#include "audit_service.h"
#include <QCoreApplication>
#include <QTimer>
#include <systemd/sd-daemon.h>
#include <iostream>
#include <csignal>
#include <unistd.h>

static AuditService* g_service = nullptr;

void signalHandler(int signal) {
    if (g_service) {
        std::cout << "Received signal " << signal << ", shutting down..." << std::endl;
        g_service->stop();
    }
    if (QCoreApplication::instance()) {
        QCoreApplication::instance()->quit();
    }
}

int main(int argc, char* argv[]) {
    // Create QCoreApplication for Qt D-Bus
    QCoreApplication app(argc, argv);
    app.setApplicationName("milos-audit-service-daemon");
    app.setOrganizationName("MilOS");

    // Register signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Create and initialize service
    AuditService service;
    g_service = &service;

    if (!service.initialize()) {
        std::cerr << "Failed to initialize Audit Service" << std::endl;
        sd_notify(0, "STATUS=Failed to initialize\n");
        return 1;
    }

    // Start service
    if (!service.start()) {
        std::cerr << "Failed to start Audit Service" << std::endl;
        sd_notify(0, "STATUS=Failed to start\n");
        return 1;
    }

    // Set up periodic health check timer (every 30 seconds)
    QTimer* healthCheckTimer = new QTimer(&app);
    QObject::connect(healthCheckTimer, &QTimer::timeout, [&service]() {
        service.performHealthCheck();
    });
    healthCheckTimer->start(30000);  // 30 seconds

    // Run Qt event loop (required for D-Bus)
    int result = app.exec();

    // Service stopped
    if (!service.isHealthy()) {
        std::cerr << "Service became unhealthy, exiting" << std::endl;
        sd_notify(0, "STATUS=Service unhealthy\n");
        return 1;
    }

    return result;
}

