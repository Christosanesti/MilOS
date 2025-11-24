#include "secure_dev_system.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-secure-development");
    app.setOrganizationName("MilOS");
    
    // Create and initialize secure development system
    SecureDevSystem system;
    
    if (!system.initialize()) {
        std::cerr << "Failed to initialize Secure Development System" << std::endl;
        return 1;
    }
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose components to QML
    engine.rootContext()->setContextProperty("secureDevSystem", &system);
    engine.rootContext()->setContextProperty("codeAnalyzer", system.getCodeAnalyzer());
    engine.rootContext()->setContextProperty("supplyChainManager", system.getSupplyChainManager());
    
    // Load main QML file
    engine.load("qrc:/ui/main.qml");
    
    if (engine.rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return 1;
    }
    
    std::cout << "Secure Development Environment running..." << std::endl;
    
    return app.exec();
}






