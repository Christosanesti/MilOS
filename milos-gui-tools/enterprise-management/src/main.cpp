#include "enterprise_management_system.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-enterprise-management");
    app.setOrganizationName("MilOS");
    
    // Create and initialize enterprise management system
    EnterpriseManagementSystem system;
    
    if (!system.initialize()) {
        std::cerr << "Failed to initialize Enterprise Management System" << std::endl;
        return 1;
    }
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose components to QML
    engine.rootContext()->setContextProperty("enterpriseSystem", &system);
    engine.rootContext()->setContextProperty("configManager", system.getConfigManager());
    engine.rootContext()->setContextProperty("deploymentOrchestrator", system.getDeploymentOrchestrator());
    engine.rootContext()->setContextProperty("multiSiteManager", system.getMultiSiteManager());
    
    // Load main QML file
    engine.load("qrc:/ui/main.qml");
    
    if (engine.rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return 1;
    }
    
    std::cout << "Enterprise Management System running..." << std::endl;
    
    return app.exec();
}

