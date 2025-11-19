#include <CLI/CLI.hpp>
#include <QCoreApplication>
#include <QDBusInterface>
#include <QDBusReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    app.setApplicationName("milos-security-status");

    CLI::App cliApp{"MilOS Security Status CLI Tool", "milos-security-status"};
    cliApp.require_subcommand(0, 1);

    // Overall status command
    auto* overallCmd = cliApp.add_subcommand("overall", "Get overall security status");

    // Security posture command
    auto* postureCmd = cliApp.add_subcommand("posture", "Get security posture");

    // Hardening status command
    auto* hardeningCmd = cliApp.add_subcommand("hardening", "Get hardening status");

    // Encryption status command
    auto* encryptionCmd = cliApp.add_subcommand("encryption", "Get encryption status");

    // Network security status command
    auto* networkCmd = cliApp.add_subcommand("network", "Get network security status");

    // Update status command
    auto* updateCmd = cliApp.add_subcommand("update", "Get update status");

    // Refresh command
    auto* refreshCmd = cliApp.add_subcommand("refresh", "Refresh all status");

    try {
        cliApp.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return cliApp.exit(e);
    }

    // Create D-Bus interface
    QDBusInterface interface("org.milos.SecurityStatusDashboard",
                            "/org/milos/SecurityStatusDashboard",
                            "org.milos.SecurityStatusDashboard",
                            QDBusConnection::sessionBus());

    if (!interface.isValid()) {
        std::cerr << "Error: Cannot connect to Security Status Dashboard service." << std::endl;
        std::cerr << "Make sure the service is running: milos-security-status-dashboard" << std::endl;
        return 1;
    }

    // Execute commands
    if (*overallCmd) {
        QDBusReply<QString> reply = interface.call("GetOverallStatus");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        std::cout << reply.value().toStdString() << std::endl;
        return 0;
    } else if (*postureCmd) {
        QDBusReply<QString> reply = interface.call("GetSecurityPosture");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        std::cout << reply.value().toStdString() << std::endl;
        return 0;
    } else if (*hardeningCmd) {
        QDBusReply<QString> reply = interface.call("GetHardeningStatus");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        std::cout << reply.value().toStdString() << std::endl;
        return 0;
    } else if (*encryptionCmd) {
        QDBusReply<QString> reply = interface.call("GetEncryptionStatus");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        std::cout << reply.value().toStdString() << std::endl;
        return 0;
    } else if (*networkCmd) {
        QDBusReply<QString> reply = interface.call("GetNetworkSecurityStatus");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        std::cout << reply.value().toStdString() << std::endl;
        return 0;
    } else if (*updateCmd) {
        QDBusReply<QString> reply = interface.call("GetUpdateStatus");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        std::cout << reply.value().toStdString() << std::endl;
        return 0;
    } else if (*refreshCmd) {
        QDBusReply<bool> reply = interface.call("RefreshStatus");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        if (reply.value()) {
            std::cout << "Status refreshed successfully" << std::endl;
            return 0;
        } else {
            std::cerr << "Error: Failed to refresh status." << std::endl;
            return 1;
        }
    } else {
        std::cout << cliApp.help() << std::endl;
    }

    return 0;
}

