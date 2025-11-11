#include "network_dashboard.h"
#include "network_monitor.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include <iostream>
#include <signal.h>

NetworkDashboard* g_dashboard = nullptr;

void signalHandler(int signal) {
    if (g_dashboard) {
        std::cout << "Received signal " << signal << ", shutting down..." << std::endl;
        g_dashboard->stop();
        QApplication::quit();
    }
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-network-dashboard");
    app.setOrganizationName("MilOS");

    // Setup signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Create and initialize dashboard
    NetworkDashboard dashboard;
    g_dashboard = &dashboard;

    if (!dashboard.initialize()) {
        std::cerr << "Failed to initialize Network Dashboard" << std::endl;
        return 1;
    }

    // Create NetworkMonitor and expose to QML
    NetworkMonitor* networkMonitor = dashboard.getNetworkMonitor();
    if (!networkMonitor) {
        std::cerr << "Failed to get Network Monitor" << std::endl;
        return 1;
    }

    // Create NetworkTopologyManager and expose to QML
    NetworkTopologyManager* networkTopology = dashboard.getNetworkTopology();
    if (!networkTopology) {
        std::cerr << "Failed to get Network Topology" << std::endl;
        return 1;
    }

    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose NetworkMonitor and NetworkTopology to QML
    engine.rootContext()->setContextProperty("networkMonitor", networkMonitor);
    engine.rootContext()->setContextProperty("networkTopology", networkTopology);
    
    // Load main QML file
    engine.load("qrc:/src/ui/main.qml");

    if (engine.rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return 1;
    }

    if (!dashboard.start()) {
        std::cerr << "Failed to start Network Dashboard" << std::endl;
        return 1;
    }

    // Start network monitor
    if (!networkMonitor->start()) {
        std::cerr << "Failed to start Network Monitor" << std::endl;
        return 1;
    }

    std::cout << "Network Defense Dashboard running..." << std::endl;

    // Run Qt event loop
    return app.exec();
}

