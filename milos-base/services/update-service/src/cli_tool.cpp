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

class UpdateServiceClient {
public:
    UpdateServiceClient() {
        m_connection = QDBusConnection::systemBus();
        m_interface = new QDBusInterface(
            "org.milos.UpdateService",
            "/org/milos/UpdateService",
            "org.milos.UpdateService",
            m_connection,
            this
        );
    }

    bool isConnected() const {
        return m_connection.isConnected() && m_interface->isValid();
    }

    QString checkUpdates() {
        QDBusReply<QString> reply = m_interface->call("CheckUpdates");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString applyUpdates(const QString& packageList) {
        QDBusReply<QString> reply = m_interface->call("ApplyUpdates", packageList);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    bool rollbackUpdate(const QString& updateId) {
        QDBusReply<bool> reply = m_interface->call("RollbackUpdate", updateId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

    QString getUpdateStatus(const QString& updateId = QString()) {
        QDBusReply<QString> reply = m_interface->call("GetUpdateStatus", updateId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString getUpdateHistory() {
        QDBusReply<QString> reply = m_interface->call("GetUpdateHistory");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString getHealthStatus() {
        QDBusReply<QString> reply = m_interface->call("GetHealthStatus");
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

void printUpdates(const QString& jsonUpdates) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonUpdates.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        std::cerr << "Error parsing updates: " << error.errorString().toStdString() << std::endl;
        return;
    }

    QJsonObject obj = doc.object();
    if (obj.contains("updates")) {
        QJsonArray updates = obj["updates"].toArray();
        int count = obj["count"].toInt();
        std::cout << "Found " << count << " available updates:" << std::endl;
        for (const auto& updateValue : updates) {
            QJsonObject update = updateValue.toObject();
            std::cout << "  - " << update["name"].toString().toStdString()
                      << " (" << update["installed_version"].toString().toStdString()
                      << " -> " << update["version"].toString().toStdString() << ")" << std::endl;
        }
    }
}

void printUpdateStatus(const QString& jsonStatus) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStatus.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        std::cerr << "Error parsing status: " << error.errorString().toStdString() << std::endl;
        return;
    }

    QJsonObject obj = doc.object();
    if (obj.contains("status")) {
        std::cout << "Update Status: " << obj["status"].toString().toStdString() << std::endl;
        if (obj.contains("update_id")) {
            std::cout << "Update ID: " << obj["update_id"].toString().toStdString() << std::endl;
        }
        if (obj.contains("package_list")) {
            QJsonArray packages = obj["package_list"].toArray();
            std::cout << "Packages: ";
            for (const auto& pkg : packages) {
                std::cout << pkg.toString().toStdString() << " ";
            }
            std::cout << std::endl;
        }
        if (obj.contains("error_message")) {
            std::cout << "Error: " << obj["error_message"].toString().toStdString() << std::endl;
        }
    }
}

void printUpdateHistory(const QString& jsonHistory) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonHistory.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        std::cerr << "Error parsing history: " << error.errorString().toStdString() << std::endl;
        return;
    }

    QJsonObject obj = doc.object();
    if (obj.contains("updates")) {
        QJsonArray updates = obj["updates"].toArray();
        int count = obj["count"].toInt();
        std::cout << "Update History (" << count << " entries):" << std::endl;
        for (const auto& updateValue : updates) {
            QJsonObject update = updateValue.toObject();
            std::cout << "  - " << update["update_id"].toString().toStdString()
                      << " (" << update["status"].toString().toStdString() << ")" << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    // Create QCoreApplication for Qt D-Bus
    QCoreApplication app(argc, argv);
    app.setApplicationName("milos-update");
    app.setOrganizationName("MilOS");

    CLI::App cliApp{"MilOS Update Service CLI Tool", "milos-update"};
    cliApp.require_subcommand(0, 1);

    // Check command
    auto* checkCmd = cliApp.add_subcommand("check", "Check for available updates");

    // Update command
    auto* updateCmd = cliApp.add_subcommand("update", "Apply security updates");
    std::string packageFile;
    updateCmd->add_option("package-file", packageFile, "Package list file (JSON array)")->required();

    // Rollback command
    auto* rollbackCmd = cliApp.add_subcommand("rollback", "Rollback update");
    std::string updateId;
    rollbackCmd->add_option("update-id", updateId, "Update ID to rollback")->required();

    // Status command
    auto* statusCmd = cliApp.add_subcommand("status", "Display update status");
    std::string statusUpdateId;
    statusCmd->add_option("update-id", statusUpdateId, "Update ID (optional)");

    // History command
    auto* historyCmd = cliApp.add_subcommand("history", "Display update history");

    // Health command
    auto* healthCmd = cliApp.add_subcommand("health", "Display service health status");

    try {
        cliApp.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return cliApp.exit(e);
    }

    // Create D-Bus client
    UpdateServiceClient client;

    if (!client.isConnected()) {
        std::cerr << "Error: Cannot connect to Update Service." << std::endl;
        std::cerr << "Make sure the service is running: systemctl status milos-update-service" << std::endl;
        return 1;
    }

    // Execute commands
    if (*checkCmd) {
        QString updates = client.checkUpdates();
        if (updates.isEmpty()) {
            return 1;
        }
        printUpdates(updates);
    } else if (*updateCmd) {
        QFile file(QString::fromStdString(packageFile));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            std::cerr << "Error: Cannot open package file: " << packageFile << std::endl;
            return 1;
        }

        QByteArray packageData = file.readAll();
        QString updateId = client.applyUpdates(QString::fromUtf8(packageData));
        if (updateId.isEmpty()) {
            return 1;
        }
        std::cout << "Update started. Update ID: " << updateId.toStdString() << std::endl;
    } else if (*rollbackCmd) {
        bool success = client.rollbackUpdate(QString::fromStdString(updateId));
        if (success) {
            std::cout << "Update rolled back successfully." << std::endl;
            return 0;
        } else {
            std::cerr << "Error: Failed to rollback update." << std::endl;
            return 1;
        }
    } else if (*statusCmd) {
        QString status = client.getUpdateStatus(
            statusUpdateId.empty() ? QString() : QString::fromStdString(statusUpdateId)
        );
        if (status.isEmpty()) {
            return 1;
        }
        printUpdateStatus(status);
    } else if (*historyCmd) {
        QString history = client.getUpdateHistory();
        if (history.isEmpty()) {
            return 1;
        }
        printUpdateHistory(history);
    } else if (*healthCmd) {
        QString health = client.getHealthStatus();
        if (health.isEmpty()) {
            return 1;
        }
        // Print health status as formatted JSON
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(health.toUtf8(), &error);
        if (error.error == QJsonParseError::NoError) {
            std::cout << doc.toJson(QJsonDocument::Indented).toStdString() << std::endl;
        } else {
            std::cout << health.toStdString() << std::endl;
        }
    } else {
        std::cout << cliApp.help() << std::endl;
    }

    return 0;
}

