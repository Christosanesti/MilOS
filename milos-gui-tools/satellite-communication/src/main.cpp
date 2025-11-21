#include "satellite_communication_system.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-satellite-communication");
    app.setOrganizationName("MilOS");
    
    // Create and initialize satellite communication system
    SatelliteCommunicationSystem system;
    
    if (!system.initialize()) {
        std::cerr << "Failed to initialize Satellite Communication System" << std::endl;
        return 1;
    }
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose components to QML
    engine.rootContext()->setContextProperty("satcomSystem", &system);
    engine.rootContext()->setContextProperty("bandwidthOptimizer", system.getBandwidthOptimizer());
    
    // Load main QML file
    engine.load("qrc:/ui/main.qml");
    
    if (engine.rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return 1;
    }
    
    std::cout << "Satellite Communication System running..." << std::endl;
    
    return app.exec();
}

