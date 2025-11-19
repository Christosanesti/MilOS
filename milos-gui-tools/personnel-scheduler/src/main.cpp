#include "personnel_scheduler.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include <iostream>
#include <signal.h>

PersonnelScheduler* g_scheduler = nullptr;

void signalHandler(int signal) {
    if (g_scheduler) {
        std::cout << "Received signal " << signal << ", shutting down..." << std::endl;
        g_scheduler->stop();
        QApplication::quit();
    }
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-personnel-scheduler");
    app.setOrganizationName("MilOS");

    // Setup signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Create and initialize personnel scheduler
    PersonnelScheduler scheduler;
    g_scheduler = &scheduler;

    if (!scheduler.initialize()) {
        std::cerr << "Failed to initialize Personnel Scheduler" << std::endl;
        return 1;
    }

    // Get components and expose to QML
    DeviceManager* deviceManager = scheduler.getDeviceManager();
    DeviceHealthMonitor* healthMonitor = scheduler.getDeviceHealthMonitor();
    
    if (!deviceManager) {
        std::cerr << "Failed to get Device Manager" << std::endl;
        return 1;
    }

    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose components to QML
    engine.rootContext()->setContextProperty("deviceManager", deviceManager);
    engine.rootContext()->setContextProperty("healthMonitor", healthMonitor);
    
    // Load main QML file
    engine.load("qrc:/ui/main.qml");

    if (engine.rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return 1;
    }

    if (!scheduler.start()) {
        std::cerr << "Failed to start Personnel Scheduler" << std::endl;
        return 1;
    }

    std::cout << "Personnel Scheduler running..." << std::endl;

    // Run Qt event loop
    return app.exec();
}

