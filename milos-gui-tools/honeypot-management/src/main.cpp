#include "honeypot_management_system.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-honeypot-management");
    app.setOrganizationName("MilOS");
    
    // Create and initialize honeypot management system
    HoneypotManagementSystem system;
    
    if (!system.initialize()) {
        std::cerr << "Failed to initialize Honeypot Management System" << std::endl;
        return 1;
    }
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose components to QML
    engine.rootContext()->setContextProperty("honeypotSystem", &system);
    engine.rootContext()->setContextProperty("honeypotManager", system.getHoneypotManager());
    engine.rootContext()->setContextProperty("threatAnalyzer", system.getThreatAnalyzer());
    
    // Load main QML file
    engine.load("qrc:/ui/main.qml");
    
    if (engine.rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return 1;
    }
    
    std::cout << "Honeypot Management System running..." << std::endl;
    
    return app.exec();
}

