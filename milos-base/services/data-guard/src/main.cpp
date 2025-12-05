#include "dataguard_service.h"
#include <milos/logging/logger.h>
#include <QCoreApplication>
#include <QTimer>
#include <systemd/sd-daemon.h>
#include <csignal>
#include <unistd.h>

static DataGuardService* g_service = nullptr;

void signalHandler(int signal) {
    if (g_service) {
        LOG_INFO(QString("Received signal %1, shutting down...").arg(signal));
        g_service->stop();
    }
    if (QCoreApplication::instance()) {
        QCoreApplication::instance()->quit();
    }
}

int main(int argc, char* argv[]) {
    // Create QCoreApplication for Qt D-Bus
    QCoreApplication app(argc, argv);
    app.setApplicationName("milos-data-guard-daemon");
    app.setOrganizationName("MilOS");

    // Register signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Initialize logger
    if (!Logger::instance()->initialize("milos-data-guard-daemon",
                                        "org.milos.AuditService",
                                        "/org/milos/AuditService",
                                        Logger::Info,
                                        true)) {
        // Fallback: continue without file logging
        Logger::instance()->initialize("milos-data-guard-daemon",
                                      "org.milos.AuditService",
                                      "/org/milos/AuditService",
                                      Logger::Info,
                                      false);
    }

    // Create and initialize service
    DataGuardService service;
    g_service = &service;

    if (!service.initialize()) {
        LOG_CRITICAL("Failed to initialize Data Transmission Guard Service");
        sd_notify(0, "STATUS=Failed to initialize\n");
        return 1;
    }

    // Start service
    if (!service.start()) {
        LOG_CRITICAL("Failed to start Data Transmission Guard Service");
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
        LOG_CRITICAL("Service became unhealthy, exiting");
        sd_notify(0, "STATUS=Service unhealthy\n");
        return 1;
    }

    return result;
}

