#include "network_dashboard.h"
#include <QCoreApplication>
#include <QTimer>
#include <iostream>
#include <signal.h>

NetworkDashboard* g_dashboard = nullptr;

void signalHandler(int signal) {
    if (g_dashboard) {
        std::cout << "Received signal " << signal << ", shutting down..." << std::endl;
        g_dashboard->stop();
        QCoreApplication::quit();
    }
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
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

    if (!dashboard.start()) {
        std::cerr << "Failed to start Network Dashboard" << std::endl;
        return 1;
    }

    std::cout << "Network Defense Dashboard running..." << std::endl;

    // Run Qt event loop
    return app.exec();
}

