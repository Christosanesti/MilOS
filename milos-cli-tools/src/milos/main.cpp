#include <CLI/CLI.hpp>
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <iostream>
#include <QProcess>
#include <QStringList>

// Forward declarations
int handleStatus(CLI::App& app);
int handleServices(CLI::App& app);
int handleConfig(CLI::App& app);
int handleVersion(CLI::App& app);
int handleHealth(CLI::App& app);

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    app.setApplicationName("milos");
    app.setOrganizationName("MilOS");

    CLI::App cliApp{"MilOS Unified Command Line Interface", "milos"};
    cliApp.require_subcommand(0, 1);

    // Status command
    auto* statusCmd = cliApp.add_subcommand("status", "Show system status overview");
    statusCmd->callback([&]() { exit(handleStatus(*statusCmd)); });

    // Services command
    auto* servicesCmd = cliApp.add_subcommand("services", "Manage MilOS services");
    std::string serviceAction;
    std::string serviceName;
    servicesCmd->add_option("action", serviceAction, "Action: list, start, stop, restart, status")
        ->required();
    servicesCmd->add_option("service", serviceName, "Service name (optional for list)");
    servicesCmd->callback([&]() { exit(handleServices(*servicesCmd)); });

    // Config command
    auto* configCmd = cliApp.add_subcommand("config", "Manage MilOS configuration");
    std::string configAction;
    std::string configKey;
    std::string configValue;
    configCmd->add_option("action", configAction, "Action: get, set, list")
        ->required();
    configCmd->add_option("key", configKey, "Configuration key");
    configCmd->add_option("value", configValue, "Configuration value (for set)");
    configCmd->callback([&]() { exit(handleConfig(*configCmd)); });

    // Version command
    auto* versionCmd = cliApp.add_subcommand("version", "Show version information");
    versionCmd->callback([&]() { exit(handleVersion(*versionCmd)); });

    // Health command
    auto* healthCmd = cliApp.add_subcommand("health", "Perform system health check");
    healthCmd->callback([&]() { exit(handleHealth(*healthCmd)); });

    // Route to existing tools if command not found
    CLI11_PARSE(cliApp, argc, argv);
    
    // If no subcommand was provided, show help
    if (cliApp.get_subcommands().empty()) {
        std::cout << cliApp.help() << std::endl;
        return 0;
    }

    return 0;
}

int handleStatus(CLI::App& app) {
    std::cout << "MilOS System Status\n";
    std::cout << "==================\n\n";

    // Check service statuses
    QStringList services = {
        "milos-audit-service",
        "milos-data-guard",
        "milos-update-service",
        "milos-file-integrity-monitoring",
        "milos-tpm-integration"
    };

    std::cout << "Services:\n";
    for (const QString& service : services) {
        QProcess process;
        process.start("systemctl", QStringList() << "is-active" << service);
        process.waitForFinished();
        QString status = process.readAllStandardOutput().trimmed();
        std::cout << "  " << service.toStdString() << ": " << status.toStdString() << "\n";
    }

    std::cout << "\nSystem Health: ";
    QDBusInterface healthInterface("org.milos.SystemHealth",
                                   "/org/milos/SystemHealth",
                                   "org.milos.SystemHealth",
                                   QDBusConnection::systemBus());
    
    if (healthInterface.isValid()) {
        QDBusReply<QString> reply = healthInterface.call("GetHealthStatus");
        if (reply.isValid()) {
            std::cout << reply.value().toStdString() << "\n";
        } else {
            std::cout << "Unknown\n";
        }
    } else {
        std::cout << "Health service not available\n";
    }

    return 0;
}

int handleServices(CLI::App& app) {
    std::string action = app.get_option("action")->as<std::string>();
    std::string service = app.count("service") > 0 ? app.get_option("service")->as<std::string>() : "";

    if (action == "list") {
        std::cout << "MilOS Services:\n";
        QStringList services = {
            "milos-audit-service",
            "milos-data-guard",
            "milos-update-service",
            "milos-file-integrity-monitoring",
            "milos-tpm-integration"
        };
        
        for (const QString& svc : services) {
            QProcess process;
            process.start("systemctl", QStringList() << "is-active" << svc);
            process.waitForFinished();
            QString status = process.readAllStandardOutput().trimmed();
            std::cout << "  " << svc.toStdString() << ": " << status.toStdString() << "\n";
        }
    } else if (!service.empty()) {
        QString serviceName = QString::fromStdString(service);
        QStringList args;
        
        if (action == "start") {
            args << "start" << serviceName;
        } else if (action == "stop") {
            args << "stop" << serviceName;
        } else if (action == "restart") {
            args << "restart" << serviceName;
        } else if (action == "status") {
            args << "status" << serviceName;
        } else {
            std::cerr << "Unknown action: " << action << std::endl;
            return 1;
        }
        
        QProcess process;
        process.start("systemctl", args);
        process.waitForFinished();
        std::cout << process.readAllStandardOutput().toStdString();
        return process.exitCode();
    } else {
        std::cerr << "Service name required for action: " << action << std::endl;
        return 1;
    }

    return 0;
}

int handleConfig(CLI::App& app) {
    std::string action = app.get_option("action")->as<std::string>();
    std::string key = app.count("key") > 0 ? app.get_option("key")->as<std::string>() : "";
    std::string value = app.count("value") > 0 ? app.get_option("value")->as<std::string>() : "";

    if (action == "list") {
        std::cout << "MilOS Configuration:\n";
        std::cout << "  Configuration files located in /etc/milos/\n";
        std::cout << "  User configuration in ~/.config/milos/\n";
    } else if (action == "get") {
        if (key.empty()) {
            std::cerr << "Configuration key required for get action" << std::endl;
            return 1;
        }
        std::cout << "Getting configuration: " << key << std::endl;
        // Implementation would read from config files
    } else if (action == "set") {
        if (key.empty() || value.empty()) {
            std::cerr << "Configuration key and value required for set action" << std::endl;
            return 1;
        }
        std::cout << "Setting configuration: " << key << " = " << value << std::endl;
        // Implementation would write to config files
    } else {
        std::cerr << "Unknown action: " << action << std::endl;
        return 1;
    }

    return 0;
}

int handleVersion(CLI::App& app) {
    std::cout << "MilOS Version Information\n";
    std::cout << "========================\n\n";
    std::cout << "MilOS CLI Tools: 1.0.0\n";
    std::cout << "Build Date: " << __DATE__ << " " << __TIME__ << "\n";
    std::cout << "Qt Version: " << QT_VERSION_STR << "\n";
    return 0;
}

int handleHealth(CLI::App& app) {
    std::cout << "MilOS System Health Check\n";
    std::cout << "=========================\n\n";

    // Check D-Bus services
    QStringList services = {
        "org.milos.AuditService",
        "org.milos.DataGuard",
        "org.milos.UpdateService",
        "org.milos.FileIntegrityMonitoring",
        "org.milos.TPM"
    };

    std::cout << "D-Bus Services:\n";
    for (const QString& service : services) {
        QDBusInterface interface(service,
                                 "/org/milos/" + service.split(".").last(),
                                 "org.freedesktop.DBus.Properties",
                                 QDBusConnection::systemBus());
        bool available = interface.isValid();
        std::cout << "  " << service.toStdString() << ": " << (available ? "Available" : "Unavailable") << "\n";
    }

    std::cout << "\nSystem Services:\n";
    QStringList systemServices = {
        "milos-audit-service",
        "milos-data-guard",
        "milos-update-service"
    };

    for (const QString& service : systemServices) {
        QProcess process;
        process.start("systemctl", QStringList() << "is-active" << service);
        process.waitForFinished();
        QString status = process.readAllStandardOutput().trimmed();
        std::cout << "  " << service.toStdString() << ": " << status.toStdString() << "\n";
    }

    return 0;
}

