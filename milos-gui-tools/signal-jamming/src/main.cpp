#include "signal_jamming_system.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-signal-jamming");
    app.setOrganizationName("MilOS");
    
    // Create and initialize signal jamming system
    SignalJammingSystem system;
    
    if (!system.initialize()) {
        std::cerr << "Failed to initialize Signal Jamming System" << std::endl;
        return 1;
    }
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose components to QML
    engine.rootContext()->setContextProperty("jammingManager", system.getManager());
    
    // Load main QML file
    engine.load("qrc:/ui/main.qml");
    
    if (engine.rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return 1;
    }
    
    std::cout << "Signal Jamming System running..." << std::endl;
    
    return app.exec();
}

