#include "personnel_location_system.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-personnel-location");
    app.setOrganizationName("MilOS");
    
    // Create and initialize personnel location system
    PersonnelLocationSystem system;
    
    if (!system.initialize()) {
        std::cerr << "Failed to initialize Personnel Location System" << std::endl;
        return 1;
    }
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose components to QML
    engine.rootContext()->setContextProperty("locationTracker", system.getTracker());
    
    // Load main QML file
    engine.load("qrc:/ui/main.qml");
    
    if (engine.rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return 1;
    }
    
    std::cout << "Personnel Location System running..." << std::endl;
    
    return app.exec();
}

