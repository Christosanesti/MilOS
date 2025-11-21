#include "threat_intelligence_system.h"
#include "threat_feed.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-threat-intelligence");
    app.setOrganizationName("MilOS");
    
    // Create and initialize threat intelligence system
    ThreatIntelligenceSystem system;
    
    if (!system.initialize()) {
        std::cerr << "Failed to initialize Threat Intelligence System" << std::endl;
        return 1;
    }
    
    // Add sample feed
    ThreatFeed* feed = new ThreatFeed(&system);
    feed->initialize("feed-001", "Military Threat Feed", ThreatFeedType::Military, QUrl("https://example.com/threats"));
    feed->setEnabled(true);
    system.addFeed(feed);
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose components to QML
    engine.rootContext()->setContextProperty("threatIntelligenceSystem", &system);
    engine.rootContext()->setContextProperty("threatCorrelator", system.getCorrelator());
    engine.rootContext()->setContextProperty("threatBlocker", system.getBlocker());
    
    // Load main QML file
    engine.load("qrc:/ui/main.qml");
    
    if (engine.rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return 1;
    }
    
    std::cout << "Threat Intelligence System running..." << std::endl;
    
    return app.exec();
}

