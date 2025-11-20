#include "secure_messenger.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include <iostream>
#include <signal.h>

SecureMessenger* g_messenger = nullptr;

void signalHandler(int signal) {
    if (g_messenger) {
        std::cout << "Received signal " << signal << ", shutting down..." << std::endl;
        g_messenger->stop();
        QApplication::quit();
    }
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-secure-messenger");
    app.setOrganizationName("MilOS");

    // Setup signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Create and initialize secure messenger
    SecureMessenger messenger;
    g_messenger = &messenger;

    if (!messenger.initialize()) {
        std::cerr << "Failed to initialize Secure Messenger" << std::endl;
        return 1;
    }

    // Get components and expose to QML
    USBAuthorization* usbAuth = messenger.getUSBAuthorization();
    KeyGenerator* keyGen = messenger.getKeyGenerator();
    KeyManager* keyMgr = messenger.getKeyManager();
    UserEnrollment* enrollment = messenger.getUserEnrollment();
    RoleManager* roleMgr = messenger.getRoleManager();
    MeshNetwork* meshNetwork = messenger.getMeshNetwork();
    NetworkManager* networkManager = messenger.getNetworkManager();
    NetworkHealthMonitor* healthMonitor = messenger.getNetworkHealthMonitor();
    MessagingCore* messagingCore = messenger.getMessagingCore();
    TextMessaging* textMessaging = messenger.getTextMessaging();
    ConversationManager* conversationManager = messenger.getConversationManager();

    if (!usbAuth) {
        std::cerr << "Failed to get USB Authorization" << std::endl;
        return 1;
    }

    // Create QML engine
    QQmlApplicationEngine engine;

    // Expose components to QML
    engine.rootContext()->setContextProperty("usbAuth", usbAuth);
    engine.rootContext()->setContextProperty("keyGen", keyGen);
    engine.rootContext()->setContextProperty("keyMgr", keyMgr);
    engine.rootContext()->setContextProperty("enrollment", enrollment);
    engine.rootContext()->setContextProperty("roleMgr", roleMgr);
    engine.rootContext()->setContextProperty("meshNetwork", meshNetwork);
    engine.rootContext()->setContextProperty("networkManager", networkManager);
    engine.rootContext()->setContextProperty("healthMonitor", healthMonitor);
    engine.rootContext()->setContextProperty("messagingCore", messagingCore);
    engine.rootContext()->setContextProperty("textMessaging", textMessaging);
    engine.rootContext()->setContextProperty("conversationManager", conversationManager);

    // Load main QML file
    engine.load("qrc:/ui/main.qml");

    if (engine.rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return 1;
    }

    if (!messenger.start()) {
        std::cerr << "Failed to start Secure Messenger" << std::endl;
        return 1;
    }

    std::cout << "Secure Messenger running..." << std::endl;

    // Run Qt event loop
    return app.exec();
}

