#include "military_radio_system.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-military-radio");
    app.setOrganizationName("MilOS");
    
    // Create and initialize military radio system
    MilitaryRadioSystem system;
    
    if (!system.initialize()) {
        std::cerr << "Failed to initialize Military Radio System" << std::endl;
        return 1;
    }
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose components to QML
    engine.rootContext()->setContextProperty("radioSystem", &system);
    engine.rootContext()->setContextProperty("radioManager", system.getRadioManager());
    
    // Load main QML file
    engine.load("qrc:/ui/main.qml");
    
    if (engine.rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return 1;
    }
    
    std::cout << "Military Radio System running..." << std::endl;
    
    return app.exec();
}

