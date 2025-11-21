#include "advanced_network_defense_system.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-advanced-network-defense");
    app.setOrganizationName("MilOS");
    
    // Create and initialize advanced network defense system
    AdvancedNetworkDefenseSystem system;
    
    if (!system.initialize()) {
        std::cerr << "Failed to initialize Advanced Network Defense System" << std::endl;
        return 1;
    }
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose components to QML
    engine.rootContext()->setContextProperty("networkDefenseSystem", &system);
    engine.rootContext()->setContextProperty("mlDetector", system.getMLThreatDetector());
    engine.rootContext()->setContextProperty("forensics", system.getNetworkForensics());
    engine.rootContext()->setContextProperty("packetAnalyzer", system.getPacketAnalyzer());
    engine.rootContext()->setContextProperty("idsEngine", system.getIDSRulesEngine());
    engine.rootContext()->setContextProperty("incidentWorkflow", system.getIncidentResponseWorkflow());
    
    // Load main QML file
    engine.load("qrc:/ui/main.qml");
    
    if (engine.rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return 1;
    }
    
    std::cout << "Advanced Network Defense System running..." << std::endl;
    
    return app.exec();
}

