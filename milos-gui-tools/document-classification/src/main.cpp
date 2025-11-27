#include "document_classification_system.h"
#include "milos/logging/logger.h"
#include "milos/ui/crash_handler.h"
#include "milos/ui/error_handler.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-document-classification");
    app.setOrganizationName("MilOS");
    
    // Initialize crash handler FIRST
    CrashHandler::instance()->initialize("milos-document-classification", "1.0.0");
    CrashHandler::instance()->installCrashHandlers();
    
    // Initialize logger
    Logger::instance()->initialize("milos-document-classification");
    
    // Initialize error handler
    ErrorHandler::instance()->initialize("milos-document-classification");
    
    // Create and initialize document classification system
    DocumentClassificationSystem system;
    
    if (!system.initialize()) {
        LOG_ERROR("Failed to initialize Document Classification System");
        ErrorHandler::instance()->handleError(
            "INITIALIZATION_FAILED",
            "Failed to initialize Document Classification System",
            ErrorHandler::Critical,
            ErrorHandler::Initialization
        );
        return 1;
    }
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose error handler and crash handler to QML
    engine.rootContext()->setContextProperty("errorHandler", ErrorHandler::instance());
    engine.rootContext()->setContextProperty("crashHandler", CrashHandler::instance());
    
    // Expose components to QML
    engine.rootContext()->setContextProperty("documentClassifier", system.getClassifier());
    engine.rootContext()->setContextProperty("classificationHandler", system.getHandler());
    
    // Load main QML file
    engine.load("qrc:/ui/main.qml");
    
    if (engine.rootObjects().isEmpty()) {
        LOG_ERROR("Failed to load QML");
        ErrorHandler::instance()->handleError(
            "QML_LOAD_FAILED",
            "Failed to load QML interface",
            ErrorHandler::Error,
            ErrorHandler::UI
        );
        return 1;
    }
    
    LOG_INFO("Document Classification System running");
    
    return app.exec();
}


    // Expose error handler and crash handler to QML
    engine.rootContext()->setContextProperty("errorHandler", ErrorHandler::instance());
    engine.rootContext()->setContextProperty("crashHandler", CrashHandler::instance());
    
    // Expose components to QML
    engine.rootContext()->setContextProperty("documentClassifier", system.getClassifier());
    engine.rootContext()->setContextProperty("classificationHandler", system.getHandler());
    
    // Load main QML file
    engine.load("qrc:/ui/main.qml");
    
    if (engine.rootObjects().isEmpty()) {
        LOG_ERROR("Failed to load QML");
        ErrorHandler::instance()->handleError(
            "QML_LOAD_FAILED",
            "Failed to load QML interface",
            ErrorHandler::Error,
            ErrorHandler::UI
        );
        return 1;
    }
    
    LOG_INFO("Document Classification System running");
    
    return app.exec();
}

