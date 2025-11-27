#include "network_dashboard.h"
#include "network_monitor.h"
#include "milos/logging/logger.h"
#include "milos/ui/crash_handler.h"
#include "milos/ui/error_handler.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include <signal.h>

NetworkDashboard* g_dashboard = nullptr;

void signalHandler(int signal) {
    if (g_dashboard) {
        LOG_INFO(QString("Received signal %1, shutting down...").arg(signal));
        g_dashboard->stop();
        QApplication::quit();
    }
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-network-dashboard");
    app.setOrganizationName("MilOS");
    
    // Initialize crash handler FIRST
    CrashHandler::instance()->initialize("milos-network-dashboard", "1.0.0");
    CrashHandler::instance()->installCrashHandlers();
    
    // Initialize logger
    Logger::instance()->initialize("milos-network-dashboard");
    
    // Initialize error handler
    ErrorHandler::instance()->initialize("milos-network-dashboard");

    // Setup signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Create and initialize dashboard
    NetworkDashboard dashboard;
    g_dashboard = &dashboard;

    if (!dashboard.initialize()) {
        LOG_ERROR("Failed to initialize Network Dashboard");
        ErrorHandler::instance()->handleError(
            "INITIALIZATION_FAILED",
            "Failed to initialize Network Dashboard",
            ErrorHandler::Critical,
            ErrorHandler::Initialization
        );
        return 1;
    }

    // Create NetworkMonitor and expose to QML
    NetworkMonitor* networkMonitor = dashboard.getNetworkMonitor();
    if (!networkMonitor) {
        LOG_ERROR("Failed to get Network Monitor");
        ErrorHandler::instance()->handleError(
            "COMPONENT_INITIALIZATION_FAILED",
            "Failed to get Network Monitor component",
            ErrorHandler::Error,
            ErrorHandler::Initialization
        );
        return 1;
    }

    // Create NetworkTopologyManager and expose to QML
    NetworkTopologyManager* networkTopology = dashboard.getNetworkTopology();
    if (!networkTopology) {
        LOG_ERROR("Failed to get Network Topology");
        ErrorHandler::instance()->handleError(
            "COMPONENT_INITIALIZATION_FAILED",
            "Failed to get Network Topology component",
            ErrorHandler::Error,
            ErrorHandler::Initialization
        );
        return 1;
    }

    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose error handler and crash handler to QML
    engine.rootContext()->setContextProperty("errorHandler", ErrorHandler::instance());
    engine.rootContext()->setContextProperty("crashHandler", CrashHandler::instance());
    
    // Expose NetworkMonitor and NetworkTopology to QML
    engine.rootContext()->setContextProperty("networkMonitor", networkMonitor);
    engine.rootContext()->setContextProperty("networkTopology", networkTopology);
    
    // Load main QML file
    engine.load("qrc:/src/ui/main.qml");

    if (engine.rootObjects().isEmpty()) {
        LOG_ERROR("Failed to load QML");
        ErrorHandler::instance()->handleError(
            "QML_LOAD_FAILED",
            "Failed to load QML interface",
            ErrorHandler::Error,
            ErrorHandler::UI
        );
        return 1;
    }

    if (!dashboard.start()) {
        LOG_ERROR("Failed to start Network Dashboard");
        ErrorHandler::instance()->handleError(
            "START_FAILED",
            "Failed to start Network Dashboard service",
            ErrorHandler::Error,
            ErrorHandler::Service
        );
        return 1;
    }

    // Start network monitor
    if (!networkMonitor->start()) {
        LOG_ERROR("Failed to start Network Monitor");
        ErrorHandler::instance()->handleError(
            "START_FAILED",
            "Failed to start Network Monitor",
            ErrorHandler::Error,
            ErrorHandler::Service
        );
        return 1;
    }

    LOG_INFO("Network Defense Dashboard running");

    // Run Qt event loop
    return app.exec();
}


    // Expose error handler and crash handler to QML
    engine.rootContext()->setContextProperty("errorHandler", ErrorHandler::instance());
    engine.rootContext()->setContextProperty("crashHandler", CrashHandler::instance());
    
    // Expose NetworkMonitor and NetworkTopology to QML
    engine.rootContext()->setContextProperty("networkMonitor", networkMonitor);
    engine.rootContext()->setContextProperty("networkTopology", networkTopology);
    
    // Load main QML file
    engine.load("qrc:/src/ui/main.qml");

    if (engine.rootObjects().isEmpty()) {
        LOG_ERROR("Failed to load QML");
        ErrorHandler::instance()->handleError(
            "QML_LOAD_FAILED",
            "Failed to load QML interface",
            ErrorHandler::Error,
            ErrorHandler::UI
        );
        return 1;
    }

    if (!dashboard.start()) {
        LOG_ERROR("Failed to start Network Dashboard");
        ErrorHandler::instance()->handleError(
            "START_FAILED",
            "Failed to start Network Dashboard service",
            ErrorHandler::Error,
            ErrorHandler::Service
        );
        return 1;
    }

    // Start network monitor
    if (!networkMonitor->start()) {
        LOG_ERROR("Failed to start Network Monitor");
        ErrorHandler::instance()->handleError(
            "START_FAILED",
            "Failed to start Network Monitor",
            ErrorHandler::Error,
            ErrorHandler::Service
        );
        return 1;
    }

    LOG_INFO("Network Defense Dashboard running");

    // Run Qt event loop
    return app.exec();
}

