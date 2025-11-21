#include "encrypted_email_system.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-encrypted-email");
    app.setOrganizationName("MilOS");
    
    // Create and initialize encrypted email system
    EncryptedEmailSystem system;
    
    if (!system.initialize()) {
        std::cerr << "Failed to initialize Encrypted Email System" << std::endl;
        return 1;
    }
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose components to QML
    engine.rootContext()->setContextProperty("emailClient", system.getEmailClient());
    
    // Load main QML file
    engine.load("qrc:/ui/main.qml");
    
    if (engine.rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return 1;
    }
    
    std::cout << "Encrypted Email Client running..." << std::endl;
    
    return app.exec();
}

