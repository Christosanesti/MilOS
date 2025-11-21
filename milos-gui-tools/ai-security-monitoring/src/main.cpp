#include "ai_security_monitoring_system.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-ai-security-monitoring");
    app.setOrganizationName("MilOS");
    
    // Create and initialize AI security monitoring system
    AISecurityMonitoringSystem system;
    
    if (!system.initialize()) {
        std::cerr << "Failed to initialize AI Security Monitoring System" << std::endl;
        return 1;
    }
    
    if (!system.start()) {
        std::cerr << "Failed to start AI Security Monitoring System" << std::endl;
        return 1;
    }
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose components to QML
    engine.rootContext()->setContextProperty("aiMonitoringSystem", &system);
    engine.rootContext()->setContextProperty("threatDetector", system.getThreatDetector());
    engine.rootContext()->setContextProperty("systemMonitor", system.getSystemMonitor());
    
    // Load main QML file
    engine.load("qrc:/ui/main.qml");
    
    if (engine.rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return 1;
    }
    
    std::cout << "AI Security Monitoring System running..." << std::endl;
    
    return app.exec();
}

