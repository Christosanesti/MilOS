#include "document_classification_system.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-document-classification");
    app.setOrganizationName("MilOS");
    
    // Create and initialize document classification system
    DocumentClassificationSystem system;
    
    if (!system.initialize()) {
        std::cerr << "Failed to initialize Document Classification System" << std::endl;
        return 1;
    }
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose components to QML
    engine.rootContext()->setContextProperty("documentClassifier", system.getClassifier());
    engine.rootContext()->setContextProperty("classificationHandler", system.getHandler());
    
    // Load main QML file
    engine.load("qrc:/ui/main.qml");
    
    if (engine.rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return 1;
    }
    
    std::cout << "Document Classification System running..." << std::endl;
    
    return app.exec();
}

