#include <CLI/CLI.hpp>
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusError>
#include <QJsonDocument>
#include <QJsonArray>
#include <iostream>
#include <string>

class BackupSystemClient {
public:
    BackupSystemClient() {
        m_connection = QDBusConnection::sessionBus();
        m_interface = new QDBusInterface(
            "org.milos.BackupSystem",
            "/org/milos/BackupSystem",
            "org.milos.BackupSystem",
            m_connection,
            this
        );
    }

    bool isConnected() const {
        return m_connection.isConnected() && m_interface->isValid();
    }

    QString createBackup(const QString& sourcePath, const QString& destinationId, const QString& backupName) {
        QDBusReply<QString> reply = m_interface->call("CreateBackup", sourcePath, destinationId, backupName);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    bool restoreBackup(const QString& backupId, const QString& restorePath) {
        QDBusReply<bool> reply = m_interface->call("RestoreBackup", backupId, restorePath);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

    QString listBackups() {
        QDBusReply<QString> reply = m_interface->call("ListBackups");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    bool deleteBackup(const QString& backupId) {
        QDBusReply<bool> reply = m_interface->call("DeleteBackup", backupId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

private:
    QDBusConnection m_connection;
    QDBusInterface* m_interface;
};

void printBackups(const QString& jsonBackups) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonBackups.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        std::cerr << "Error parsing backups: " << error.errorString().toStdString() << std::endl;
        return;
    }

    QJsonArray backups = doc.array();
    std::cout << "Backups (" << backups.size() << " entries):" << std::endl;
    for (int i = 0; i < backups.size(); i++) {
        QJsonObject backup = backups[i].toObject();
        std::cout << "  " << (i + 1) << ". " << backup["name"].toString().toStdString()
                  << " [" << backup["id"].toString().toStdString() << "]" << std::endl;
        std::cout << "     Source: " << backup["source_path"].toString().toStdString() << std::endl;
        std::cout << "     Timestamp: " << backup["timestamp"].toInt() << std::endl;
        std::cout << "     Size: " << backup["size"].toInt() << " bytes" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    app.setApplicationName("milos-backup");
    app.setOrganizationName("MilOS");

    CLI::App cliApp{"MilOS Zero-Knowledge Backup System CLI Tool", "milos-backup"};
    cliApp.require_subcommand(0, 1);

    // Create command
    auto* createCmd = cliApp.add_subcommand("create", "Create backup");
    std::string sourcePath;
    std::string destinationId;
    std::string backupName;
    createCmd->add_option("source", sourcePath, "Source path to backup")->required();
    createCmd->add_option("destination", destinationId, "Destination ID")->required();
    createCmd->add_option("name", backupName, "Backup name")->required();

    // Restore command
    auto* restoreCmd = cliApp.add_subcommand("restore", "Restore backup");
    std::string backupId;
    std::string restorePath;
    restoreCmd->add_option("backup-id", backupId, "Backup ID to restore")->required();
    restoreCmd->add_option("path", restorePath, "Restore destination path")->required();

    // List command
    auto* listCmd = cliApp.add_subcommand("list", "List backups");

    // Delete command
    auto* deleteCmd = cliApp.add_subcommand("delete", "Delete backup");
    std::string deleteBackupId;
    deleteCmd->add_option("backup-id", deleteBackupId, "Backup ID to delete")->required();

    try {
        cliApp.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return cliApp.exit(e);
    }

    // Create D-Bus client
    BackupSystemClient client;

    if (!client.isConnected()) {
        std::cerr << "Error: Cannot connect to Backup System service." << std::endl;
        std::cerr << "Make sure the service is running: milos-backup-system" << std::endl;
        return 1;
    }

    // Execute commands
    if (*createCmd) {
        QString backupId = client.createBackup(
            QString::fromStdString(sourcePath),
            QString::fromStdString(destinationId),
            QString::fromStdString(backupName)
        );
        if (backupId.isEmpty()) {
            std::cerr << "Error: Failed to create backup." << std::endl;
            return 1;
        } else {
            std::cout << "Backup created: " << backupId.toStdString() << std::endl;
            return 0;
        }
    } else if (*restoreCmd) {
        bool success = client.restoreBackup(
            QString::fromStdString(backupId),
            QString::fromStdString(restorePath)
        );
        if (success) {
            std::cout << "Backup restored successfully" << std::endl;
            return 0;
        } else {
            std::cerr << "Error: Failed to restore backup." << std::endl;
            return 1;
        }
    } else if (*listCmd) {
        QString backupsJson = client.listBackups();
        if (backupsJson.isEmpty()) {
            std::cout << "No backups found" << std::endl;
            return 0;
        }
        printBackups(backupsJson);
        return 0;
    } else if (*deleteCmd) {
        bool success = client.deleteBackup(QString::fromStdString(deleteBackupId));
        if (success) {
            std::cout << "Backup deleted successfully" << std::endl;
            return 0;
        } else {
            std::cerr << "Error: Failed to delete backup." << std::endl;
            return 1;
        }
    } else {
        std::cout << cliApp.help() << std::endl;
    }

    return 0;
}

