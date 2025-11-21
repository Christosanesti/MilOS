#include "secure_communication_hub_system.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-secure-communication-hub");
    app.setOrganizationName("MilOS");
    
    // Create and initialize secure communication hub system
    SecureCommunicationHubSystem system;
    
    if (!system.initialize()) {
        std::cerr << "Failed to initialize Secure Communication Hub System" << std::endl;
        return 1;
    }
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose components to QML
    engine.rootContext()->setContextProperty("communicationHub", system.getHub());
    
    // Load main QML file
    engine.load("qrc:/ui/main.qml");
    
    if (engine.rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return 1;
    }
    
    std::cout << "Secure Communication Hub running..." << std::endl;
    
    return app.exec();
}

