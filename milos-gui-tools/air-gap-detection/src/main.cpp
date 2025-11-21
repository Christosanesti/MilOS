#include "air_gap_system.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-air-gap-detection");
    app.setOrganizationName("MilOS");
    
    // Create and initialize air-gap system
    AirGapSystem system;
    
    if (!system.initialize()) {
        std::cerr << "Failed to initialize Air-Gap Detection System" << std::endl;
        return 1;
    }
    
    if (!system.start()) {
        std::cerr << "Failed to start Air-Gap Detection System" << std::endl;
        return 1;
    }
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose components to QML
    engine.rootContext()->setContextProperty("airGapDetector", system.getDetector());
    engine.rootContext()->setContextProperty("airGapEnforcement", system.getEnforcement());
    
    // Load main QML file
    engine.load("qrc:/ui/main.qml");
    
    if (engine.rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return 1;
    }
    
    std::cout << "Air-Gap Detection System running..." << std::endl;
    
    return app.exec();
}

