#include "compliance_checklist_system.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-compliance-checklist");
    app.setOrganizationName("MilOS");
    
    // Create and initialize compliance checklist system
    ComplianceChecklistSystem system;
    
    if (!system.initialize()) {
        std::cerr << "Failed to initialize Compliance Checklist System" << std::endl;
        return 1;
    }
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose components to QML
    engine.rootContext()->setContextProperty("checklistManager", system.getChecklistManager());
    engine.rootContext()->setContextProperty("complianceReporter", system.getReporter());
    
    // Load main QML file
    engine.load("qrc:/ui/main.qml");
    
    if (engine.rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return 1;
    }
    
    std::cout << "Compliance Checklist System running..." << std::endl;
    
    return app.exec();
}

