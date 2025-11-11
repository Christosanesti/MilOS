#include <CLI/CLI.hpp>
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <iostream>
#include <string>

class DataGuardClient {
public:
    DataGuardClient() {
        m_connection = QDBusConnection::systemBus();
        m_interface = new QDBusInterface(
            "org.milos.DataGuard",
            "/org/milos/DataGuard",
            "org.milos.DataGuard",
            m_connection,
            this
        );
    }

    bool isConnected() const {
        return m_connection.isConnected() && m_interface->isValid();
    }

    QString getTransmissionStatus() {
        QDBusReply<QString> reply = m_interface->call("GetTransmissionStatus");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QStringList getBlockedTransmissions() {
        QDBusReply<QStringList> reply = m_interface->call("GetBlockedTransmissions");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QStringList();
        }
        return reply.value();
    }

    bool configurePolicy(const QString& policy) {
        QDBusReply<bool> reply = m_interface->call("ConfigurePolicy", policy);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

    QString getPolicyStatus(const QString& policyId) {
        QDBusReply<QString> reply = m_interface->call("GetPolicyStatus", policyId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

private:
    QDBusConnection m_connection;
    QDBusInterface* m_interface;
};

void printStatus(const QString& jsonStatus) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStatus.toUtf8(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        std::cerr << "Error parsing status: " << error.errorString().toStdString() << std::endl;
        return;
    }

    QJsonObject obj = doc.object();
    std::cout << "Service Status: " << obj["status"].toString().toStdString() << std::endl;
    
    if (obj.contains("blocked")) {
        std::cout << "Blocked Transmissions: " << obj["blocked"].toInt() << std::endl;
    }
    if (obj.contains("allowed")) {
        std::cout << "Allowed Transmissions: " << obj["allowed"].toInt() << std::endl;
    }
    if (obj.contains("healthy")) {
        std::cout << "Health Status: " << (obj["healthy"].toBool() ? "Healthy" : "Unhealthy") << std::endl;
    }
    if (obj.contains("message")) {
        std::cout << "Message: " << obj["message"].toString().toStdString() << std::endl;
    }
}

void printBlockedTransmissions(const QStringList& blocked) {
    if (blocked.isEmpty()) {
        std::cout << "No blocked transmissions found." << std::endl;
        return;
    }

    std::cout << "Blocked Transmissions:" << std::endl;
    for (int i = 0; i < blocked.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << blocked[i].toStdString() << std::endl;
    }
}

void printPolicyStatus(const QString& jsonStatus) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStatus.toUtf8(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        std::cerr << "Error parsing policy status: " << error.errorString().toStdString() << std::endl;
        return;
    }

    QJsonObject obj = doc.object();
    QString status = obj["status"].toString();
    
    if (status == "not_found") {
        std::cout << "Policy not found." << std::endl;
        return;
    }
    
    if (status == "error") {
        std::cout << "Error: " << obj["message"].toString().toStdString() << std::endl;
        return;
    }

    std::cout << "Policy ID: " << obj["policy_id"].toString().toStdString() << std::endl;
    std::cout << "Policy Name: " << obj["policy_name"].toString().toStdString() << std::endl;
    std::cout << "Policy Type: " << obj["policy_type"].toString().toStdString() << std::endl;
    std::cout << "Enabled: " << (obj["enabled"].toBool() ? "Yes" : "No") << std::endl;
    std::cout << "Priority: " << obj["priority"].toInt() << std::endl;
    std::cout << "Rule Count: " << obj["rule_count"].toInt() << std::endl;
}

int main(int argc, char* argv[]) {
    // Create QCoreApplication for Qt D-Bus
    QCoreApplication app(argc, argv);
    app.setApplicationName("milos-data-guard");
    app.setOrganizationName("MilOS");

    CLI::App cliApp{"MilOS Data Transmission Guard CLI Tool", "milos-data-guard"};
    cliApp.require_subcommand(0, 1);

    // Status command
    auto* statusCmd = cliApp.add_subcommand("status", "Display service status and transmission monitoring status");
    
    // Blocked command
    auto* blockedCmd = cliApp.add_subcommand("blocked", "List blocked transmissions");
    
    // Policy command
    auto* policyCmd = cliApp.add_subcommand("policy", "Manage transmission policies");
    
    std::string policyId;
    std::string policyFile;
    
    auto* policyListCmd = policyCmd->add_subcommand("list", "List all policies");
    auto* policyShowCmd = policyCmd->add_subcommand("show", "Show policy details");
    policyShowCmd->add_option("policy-id", policyId, "Policy ID to show")->required();
    auto* policyConfigCmd = policyCmd->add_subcommand("configure", "Configure policy");
    policyConfigCmd->add_option("policy-file", policyFile, "Policy configuration file (JSON)")->required();
    
    // Config command
    auto* configCmd = cliApp.add_subcommand("config", "Manage service configuration");
    auto* configShowCmd = configCmd->add_subcommand("show", "Show current configuration");
    auto* configReloadCmd = configCmd->add_subcommand("reload", "Reload configuration from file");

    try {
        cliApp.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return cliApp.exit(e);
    }

    // Create D-Bus client
    DataGuardClient client;
    
    if (!client.isConnected()) {
        std::cerr << "Error: Cannot connect to Data Transmission Guard service." << std::endl;
        std::cerr << "Make sure the service is running: systemctl status milos-data-guard" << std::endl;
        return 1;
    }

    // Execute commands
    if (*statusCmd) {
        QString status = client.getTransmissionStatus();
        if (status.isEmpty()) {
            return 1;
        }
        printStatus(status);
    } else if (*blockedCmd) {
        QStringList blocked = client.getBlockedTransmissions();
        printBlockedTransmissions(blocked);
    } else if (*policyCmd) {
        if (*policyListCmd) {
            std::cout << "Policy list functionality requires policy manager extension." << std::endl;
            std::cout << "Use 'policy show <policy-id>' to view individual policies." << std::endl;
        } else if (*policyShowCmd) {
            QString status = client.getPolicyStatus(QString::fromStdString(policyId));
            if (status.isEmpty()) {
                return 1;
            }
            printPolicyStatus(status);
        } else if (*policyConfigCmd) {
            QFile file(QString::fromStdString(policyFile));
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                std::cerr << "Error: Cannot open policy file: " << policyFile << std::endl;
                return 1;
            }
            
            QByteArray policyData = file.readAll();
            QString policyJson = QString::fromUtf8(policyData);
            
            if (client.configurePolicy(policyJson)) {
                std::cout << "Policy configured successfully." << std::endl;
            } else {
                std::cerr << "Error: Failed to configure policy." << std::endl;
                return 1;
            }
        } else {
            std::cout << policyCmd->help() << std::endl;
        }
    } else if (*configCmd) {
        if (*configShowCmd) {
            std::cout << "Configuration file location: /etc/milos/data-guard/config.yaml" << std::endl;
            std::cout << "Use 'cat /etc/milos/data-guard/config.yaml' to view configuration." << std::endl;
        } else if (*configReloadCmd) {
            std::cout << "Configuration reload functionality requires service restart:" << std::endl;
            std::cout << "  systemctl reload milos-data-guard" << std::endl;
            std::cout << "Or restart the service:" << std::endl;
            std::cout << "  systemctl restart milos-data-guard" << std::endl;
        } else {
            std::cout << configCmd->help() << std::endl;
        }
    } else {
        std::cout << cliApp.help() << std::endl;
    }

    return 0;
}

