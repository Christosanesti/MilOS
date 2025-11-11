#include "milos/pqc/dbus_interface.h"
#include "milos/pqc/algorithm_factory.h"
#include "milos/pqc/algorithm_registry.h"
#include "milos/pqc/hybrid_encryption.h"
#include "milos/pqc/fallback_manager.h"
#include "milos/pqc/config_parser.h"
#include <QCoreApplication>
#include <QTimer>
#include <iostream>

int main(int argc, char* argv[]) {
    // Create QCoreApplication for Qt D-Bus
    QCoreApplication app(argc, argv);
    app.setApplicationName("milos-pqc-service");
    app.setOrganizationName("MilOS");

    // Initialize components
    AlgorithmFactory& factory = AlgorithmFactory::getInstance();
    AlgorithmRegistry& registry = AlgorithmRegistry::getInstance();
    
    // Load configuration
    ConfigParser config;
    if (!config.load("/etc/milos/pqc/config.yaml")) {
        std::cerr << "Failed to load configuration, using defaults" << std::endl;
    }

    // Initialize hybrid encryption (will be populated when algorithms are registered)
    HybridEncryption hybridEncryption;
    
    // Initialize fallback manager
    FallbackManager fallbackManager;
    std::vector<std::string> fallbackPriority = {
        config.getString("fallback.fallback_priority[0]"),
        config.getString("fallback.fallback_priority[1]"),
        "AES-256-GCM"  // Classical fallback
    };
    fallbackManager.initialize(fallbackPriority, 
                               config.getBool("fallback.fallback_to_classical", true));

    // Create and initialize D-Bus interface
    DBusInterface dbusInterface;
    if (!dbusInterface.initialize(&factory, &registry, &hybridEncryption, &fallbackManager)) {
        std::cerr << "Failed to initialize D-Bus interface" << std::endl;
        return 1;
    }

    // Start D-Bus interface
    if (!dbusInterface.start()) {
        std::cerr << "Failed to start D-Bus interface" << std::endl;
        return 1;
    }

    std::cout << "PQC Abstraction Layer D-Bus service started" << std::endl;

    // Run Qt event loop (required for D-Bus)
    return app.exec();
}

