#include "backup_system.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include <iostream>
#include <signal.h>

BackupSystem* g_system = nullptr;

void signalHandler(int signal) {
    if (g_system) {
        std::cout << "Received signal " << signal << ", shutting down..." << std::endl;
        g_system->stop();
        QApplication::quit();
    }
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-backup-system");
    app.setOrganizationName("MilOS");

    // Setup signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Create and initialize backup system
    BackupSystem system;
    g_system = &system;

    if (!system.initialize()) {
        std::cerr << "Failed to initialize Backup System" << std::endl;
        return 1;
    }

    // Get BackupService and expose to QML
    BackupService* backupService = system.getBackupService();
    if (!backupService) {
        std::cerr << "Failed to get Backup Service" << std::endl;
        return 1;
    }

    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose BackupService to QML
    engine.rootContext()->setContextProperty("backupService", backupService);
    
    // Load main QML file
    engine.load("qrc:/ui/main.qml");

    if (engine.rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return 1;
    }

    if (!system.start()) {
        std::cerr << "Failed to start Backup System" << std::endl;
        return 1;
    }

    std::cout << "Zero-Knowledge Backup System running..." << std::endl;

    // Run Qt event loop
    return app.exec();
}

