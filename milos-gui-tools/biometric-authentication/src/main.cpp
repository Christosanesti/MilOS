#include "biometric_authentication_system.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-biometric-authentication");
    app.setOrganizationName("MilOS");
    
    // Create and initialize biometric authentication system
    BiometricAuthenticationSystem system;
    
    if (!system.initialize()) {
        std::cerr << "Failed to initialize Biometric Authentication System" << std::endl;
        return 1;
    }
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose components to QML
    engine.rootContext()->setContextProperty("biometricSystem", &system);
    engine.rootContext()->setContextProperty("biometricAuthenticator", system.getAuthenticator());
    engine.rootContext()->setContextProperty("mfaManager", system.getMFAManager());
    
    // Load main QML file
    engine.load("qrc:/ui/main.qml");
    
    if (engine.rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return 1;
    }
    
    std::cout << "Biometric Authentication System running..." << std::endl;
    
    return app.exec();
}

