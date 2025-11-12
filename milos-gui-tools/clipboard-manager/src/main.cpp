#include "clipboard_manager.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include <iostream>
#include <signal.h>

ClipboardManager* g_manager = nullptr;

void signalHandler(int signal) {
    if (g_manager) {
        std::cout << "Received signal " << signal << ", shutting down..." << std::endl;
        g_manager->stop();
        QApplication::quit();
    }
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-clipboard-manager");
    app.setOrganizationName("MilOS");

    // Setup signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Create and initialize clipboard manager
    ClipboardManager manager;
    g_manager = &manager;

    if (!manager.initialize()) {
        std::cerr << "Failed to initialize Clipboard Manager" << std::endl;
        return 1;
    }

    // Get ClipboardService and expose to QML
    ClipboardService* clipboardService = manager.getClipboardService();
    if (!clipboardService) {
        std::cerr << "Failed to get Clipboard Service" << std::endl;
        return 1;
    }

    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose ClipboardService to QML
    engine.rootContext()->setContextProperty("clipboardService", clipboardService);
    
    // Load main QML file
    engine.load("qrc:/ui/main.qml");

    if (engine.rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return 1;
    }

    if (!manager.start()) {
        std::cerr << "Failed to start Clipboard Manager" << std::endl;
        return 1;
    }

    std::cout << "Secure Clipboard Manager running..." << std::endl;

    // Run Qt event loop
    return app.exec();
}

