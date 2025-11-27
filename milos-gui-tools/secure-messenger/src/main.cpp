#include "secure_messenger.h"
#include "e2e_encryption.h"
#include "forward_secrecy.h"
#include "emergency_eject.h"
#include "admin_dashboard.h"
#include "milos/logging/logger.h"
#include "milos/ui/crash_handler.h"
#include "milos/ui/error_handler.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include <signal.h>

SecureMessenger* g_messenger = nullptr;

void signalHandler(int signal) {
    if (g_messenger) {
        LOG_INFO(QString("Received signal %1, shutting down...").arg(signal));
        g_messenger->stop();
        QApplication::quit();
    }
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("milos-secure-messenger");
    app.setOrganizationName("MilOS");
    
    // Initialize crash handler FIRST
    CrashHandler::instance()->initialize("milos-secure-messenger", "1.0.0");
    CrashHandler::instance()->installCrashHandlers();
    
    // Initialize logger
    Logger::instance()->initialize("milos-secure-messenger");
    
    // Initialize error handler
    ErrorHandler::instance()->initialize("milos-secure-messenger");

    // Setup signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Create and initialize secure messenger
    SecureMessenger messenger;
    g_messenger = &messenger;

    if (!messenger.initialize()) {
        LOG_CRITICAL("Failed to initialize Secure Messenger");
        ErrorHandler::instance()->handleError(
            "INITIALIZATION_FAILED",
            "Failed to initialize Secure Messenger",
            ErrorHandler::Critical,
            ErrorHandler::Initialization
        );
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
    E2EEncryption* e2eEncryption = messenger.getE2EEncryption();
    ForwardSecrecy* forwardSecrecy = messenger.getForwardSecrecy();
    EmergencyEject* emergencyEject = messenger.getEmergencyEject();
    AdminDashboard* adminDashboard = messenger.getAdminDashboard();

    if (!usbAuth) {
        LOG_ERROR("Failed to get USB Authorization");
        ErrorHandler::instance()->handleError(
            "COMPONENT_INITIALIZATION_FAILED",
            "Failed to get USB Authorization component",
            ErrorHandler::Error,
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
    engine.rootContext()->setContextProperty("e2eEncryption", e2eEncryption);
    engine.rootContext()->setContextProperty("forwardSecrecy", forwardSecrecy);
    engine.rootContext()->setContextProperty("emergencyEject", emergencyEject);
    engine.rootContext()->setContextProperty("adminDashboard", adminDashboard);

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

    if (!messenger.start()) {
        LOG_ERROR("Failed to start Secure Messenger");
        ErrorHandler::instance()->handleError(
            "START_FAILED",
            "Failed to start Secure Messenger service",
            ErrorHandler::Error,
            ErrorHandler::Service
        );
        return 1;
    }

    LOG_INFO("Secure Messenger running");

    // Run Qt event loop
    return app.exec();
}


    engine.rootContext()->setContextProperty("messagingCore", messagingCore);
    engine.rootContext()->setContextProperty("textMessaging", textMessaging);
    engine.rootContext()->setContextProperty("conversationManager", conversationManager);
    engine.rootContext()->setContextProperty("e2eEncryption", e2eEncryption);
    engine.rootContext()->setContextProperty("forwardSecrecy", forwardSecrecy);
    engine.rootContext()->setContextProperty("emergencyEject", emergencyEject);
    engine.rootContext()->setContextProperty("adminDashboard", adminDashboard);

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

    if (!messenger.start()) {
        LOG_ERROR("Failed to start Secure Messenger");
        ErrorHandler::instance()->handleError(
            "START_FAILED",
            "Failed to start Secure Messenger service",
            ErrorHandler::Error,
            ErrorHandler::Service
        );
        return 1;
    }

    LOG_INFO("Secure Messenger running");

    // Run Qt event loop
    return app.exec();
}

