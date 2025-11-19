#include <CLI/CLI.hpp>
#include <QCoreApplication>
#include <QDBusInterface>
#include <QDBusReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    app.setApplicationName("milos-network-segmentation");

    CLI::App cliApp{"MilOS Network Segmentation CLI Tool", "milos-network-segmentation"};
    cliApp.require_subcommand(0, 1);

    // Create command
    auto* createCmd = cliApp.add_subcommand("create", "Create network segment");
    std::string createName;
    std::string createNetwork;
    std::string createDescription;
    createCmd->add_option("name", createName, "Segment name")->required();
    createCmd->add_option("network", createNetwork, "Network address (CIDR)")->required();
    createCmd->add_option("--description", createDescription, "Segment description");

    // Update command
    auto* updateCmd = cliApp.add_subcommand("update", "Update network segment");
    std::string updateSegmentId;
    std::string updateName;
    std::string updateNetwork;
    std::string updateDescription;
    updateCmd->add_option("segment-id", updateSegmentId, "Segment ID")->required();
    updateCmd->add_option("name", updateName, "Segment name")->required();
    updateCmd->add_option("network", updateNetwork, "Network address (CIDR)")->required();
    updateCmd->add_option("--description", updateDescription, "Segment description");

    // Delete command
    auto* deleteCmd = cliApp.add_subcommand("delete", "Delete network segment");
    std::string deleteSegmentId;
    deleteCmd->add_option("segment-id", deleteSegmentId, "Segment ID to delete")->required();

    // List command
    auto* listCmd = cliApp.add_subcommand("list", "List network segments");

    // Get command
    auto* getCmd = cliApp.add_subcommand("get", "Get segment information");
    std::string getSegmentId;
    getCmd->add_option("segment-id", getSegmentId, "Segment ID")->required();

    // Firewall rule commands
    auto* firewallCmd = cliApp.add_subcommand("firewall", "Firewall rule management");
    auto* generateRulesCmd = firewallCmd->add_subcommand("generate", "Generate firewall rules from segments");
    auto* validateRulesCmd = firewallCmd->add_subcommand("validate", "Validate firewall rules");
    auto* previewRulesCmd = firewallCmd->add_subcommand("preview", "Preview firewall rules");
    auto* applyRulesCmd = firewallCmd->add_subcommand("apply", "Apply firewall rules");
    auto* rollbackRulesCmd = firewallCmd->add_subcommand("rollback", "Rollback firewall rules");

    // Isolation enforcement commands
    auto* isolationCmd = cliApp.add_subcommand("isolation", "Network isolation enforcement");
    auto* enforceIsolationCmd = isolationCmd->add_subcommand("enforce", "Enforce isolation for segment");
    std::string enforceSegmentId;
    enforceIsolationCmd->add_option("segment-id", enforceSegmentId, "Segment ID")->required();
    auto* removeIsolationCmd = isolationCmd->add_subcommand("remove", "Remove isolation for segment");
    std::string removeSegmentId;
    removeIsolationCmd->add_option("segment-id", removeSegmentId, "Segment ID")->required();
    auto* verifyIsolationCmd = isolationCmd->add_subcommand("verify", "Verify segment isolation");
    std::string verifySegmentId;
    verifyIsolationCmd->add_option("segment-id", verifySegmentId, "Segment ID")->required();

    try {
        cliApp.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return cliApp.exit(e);
    }

    // Create D-Bus interface
    QDBusInterface interface("org.milos.NetworkSegmentation",
                            "/org/milos/NetworkSegmentation",
                            "org.milos.NetworkSegmentation",
                            QDBusConnection::sessionBus());

    if (!interface.isValid()) {
        std::cerr << "Error: Cannot connect to Network Segmentation service." << std::endl;
        std::cerr << "Make sure the service is running: milos-network-segmentation" << std::endl;
        return 1;
    }

    // Execute commands
    if (*createCmd) {
        QDBusReply<QString> reply = interface.call("CreateSegment",
                                                    QString::fromStdString(createName),
                                                    QString::fromStdString(createNetwork),
                                                    QString::fromStdString(createDescription));
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        QString segmentId = reply.value();
        if (segmentId.isEmpty()) {
            std::cerr << "Error: Failed to create segment." << std::endl;
            return 1;
        }
        std::cout << "Segment created: " << segmentId.toStdString() << std::endl;
        return 0;
    } else if (*updateCmd) {
        QDBusReply<bool> reply = interface.call("UpdateSegment",
                                                QString::fromStdString(updateSegmentId),
                                                QString::fromStdString(updateName),
                                                QString::fromStdString(updateNetwork),
                                                QString::fromStdString(updateDescription));
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        if (reply.value()) {
            std::cout << "Segment updated successfully" << std::endl;
            return 0;
        } else {
            std::cerr << "Error: Failed to update segment." << std::endl;
            return 1;
        }
    } else if (*deleteCmd) {
        QDBusReply<bool> reply = interface.call("DeleteSegment", QString::fromStdString(deleteSegmentId));
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        if (reply.value()) {
            std::cout << "Segment deleted successfully" << std::endl;
            return 0;
        } else {
            std::cerr << "Error: Failed to delete segment." << std::endl;
            return 1;
        }
    } else if (*listCmd) {
        QDBusReply<QStringList> reply = interface.call("ListSegments");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        QStringList segments = reply.value();
        std::cout << "Segments (" << segments.size() << "):" << std::endl;
        for (const QString& segmentId : segments) {
            std::cout << "  - " << segmentId.toStdString() << std::endl;
        }
        return 0;
    } else if (*getCmd) {
        QDBusReply<QString> reply = interface.call("GetSegment", QString::fromStdString(getSegmentId));
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        QString json = reply.value();
        if (json.isEmpty()) {
            std::cerr << "Error: Segment not found." << std::endl;
            return 1;
        }
        std::cout << json.toStdString() << std::endl;
        return 0;
    } else if (*generateRulesCmd) {
        QDBusReply<bool> reply = interface.call("GenerateFirewallRules");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        if (reply.value()) {
            std::cout << "Firewall rules generated successfully" << std::endl;
            return 0;
        } else {
            std::cerr << "Error: Failed to generate firewall rules." << std::endl;
            return 1;
        }
    } else if (*validateRulesCmd) {
        QDBusReply<QString> reply = interface.call("ValidateFirewallRules");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        std::cout << reply.value().toStdString() << std::endl;
        return 0;
    } else if (*previewRulesCmd) {
        QDBusReply<QString> reply = interface.call("PreviewFirewallRules");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        std::cout << reply.value().toStdString() << std::endl;
        return 0;
    } else if (*applyRulesCmd) {
        QDBusReply<bool> reply = interface.call("ApplyFirewallRules");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        if (reply.value()) {
            std::cout << "Firewall rules applied successfully" << std::endl;
            return 0;
        } else {
            std::cerr << "Error: Failed to apply firewall rules." << std::endl;
            return 1;
        }
    } else if (*rollbackRulesCmd) {
        QDBusReply<bool> reply = interface.call("RollbackFirewallRules");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        if (reply.value()) {
            std::cout << "Firewall rules rolled back successfully" << std::endl;
            return 0;
        } else {
            std::cerr << "Error: Failed to rollback firewall rules." << std::endl;
            return 1;
        }
    } else if (*enforceIsolationCmd) {
        QDBusReply<bool> reply = interface.call("EnforceIsolation", QString::fromStdString(enforceSegmentId));
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        if (reply.value()) {
            std::cout << "Isolation enforced successfully" << std::endl;
            return 0;
        } else {
            std::cerr << "Error: Failed to enforce isolation." << std::endl;
            return 1;
        }
    } else if (*removeIsolationCmd) {
        QDBusReply<bool> reply = interface.call("RemoveIsolation", QString::fromStdString(removeSegmentId));
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        if (reply.value()) {
            std::cout << "Isolation removed successfully" << std::endl;
            return 0;
        } else {
            std::cerr << "Error: Failed to remove isolation." << std::endl;
            return 1;
        }
    } else if (*verifyIsolationCmd) {
        QDBusReply<bool> reply = interface.call("VerifyIsolation", QString::fromStdString(verifySegmentId));
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        if (reply.value()) {
            std::cout << "Segment is isolated" << std::endl;
            return 0;
        } else {
            std::cout << "Segment is not isolated" << std::endl;
            return 1;
        }
    } else {
        std::cout << cliApp.help() << std::endl;
    }

    return 0;
}

