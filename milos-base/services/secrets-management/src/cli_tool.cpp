#include <CLI/CLI.hpp>
#include <QCoreApplication>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusConnection>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    app.setApplicationName("milos-secrets");

    CLI::App cliApp{"MilOS Secrets Management CLI Tool", "milos-secrets"};
    cliApp.require_subcommand(0, 1);

    // Create D-Bus interface
    QDBusInterface interface("org.milos.SecretsManagement",
                             "/org/milos/SecretsManagement",
                             "org.milos.SecretsManagement",
                             QDBusConnection::systemBus());

    if (!interface.isValid()) {
        std::cerr << "Error: Cannot connect to Secrets Management service" << std::endl;
        std::cerr << "Make sure the service is running: systemctl start milos-secrets-management" << std::endl;
        return 1;
    }

    // Store command
    auto* storeCmd = cliApp.add_subcommand("store", "Store a secret");
    std::string secretId;
    std::string secretFile;
    std::string secretType = "password";
    std::string description;
    std::string expiresAt;
    storeCmd->add_option("secret-id", secretId, "Secret ID")->required();
    storeCmd->add_option("--file", secretFile, "File containing secret data");
    storeCmd->add_option("--type", secretType, "Secret type (password, api_key, certificate)");
    storeCmd->add_option("--description", description, "Description");
    storeCmd->add_option("--expires", expiresAt, "Expiration date (YYYY-MM-DD)");

    // Get command
    auto* getCmd = cliApp.add_subcommand("get", "Get a secret");
    std::string getSecretId;
    std::string version;
    std::string outputFile;
    getCmd->add_option("secret-id", getSecretId, "Secret ID")->required();
    getCmd->add_option("--version", version, "Version ID (optional)");
    getCmd->add_option("--output", outputFile, "Output file (optional)");

    // Rotate command
    auto* rotateCmd = cliApp.add_subcommand("rotate", "Rotate a secret");
    std::string rotateSecretId;
    rotateCmd->add_option("secret-id", rotateSecretId, "Secret ID")->required();

    // List command
    auto* listCmd = cliApp.add_subcommand("list", "List all secrets");

    // Metadata command
    auto* metadataCmd = cliApp.add_subcommand("metadata", "Get secret metadata");
    std::string metadataSecretId;
    metadataCmd->add_option("secret-id", metadataSecretId, "Secret ID")->required();

    // Rollback command
    auto* rollbackCmd = cliApp.add_subcommand("rollback", "Rollback secret to version");
    std::string rollbackSecretId;
    std::string rollbackVersion;
    rollbackCmd->add_option("secret-id", rollbackSecretId, "Secret ID")->required();
    rollbackCmd->add_option("version", rollbackVersion, "Version ID")->required();

    // Delete command
    auto* deleteCmd = cliApp.add_subcommand("delete", "Delete a secret");
    std::string deleteSecretId;
    deleteCmd->add_option("secret-id", deleteSecretId, "Secret ID")->required();

    try {
        cliApp.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return cliApp.exit(e);
    }

    // Handle store command
    if (storeCmd->parsed()) {
        QByteArray secretData;
        
        if (!secretFile.empty()) {
            // Read from file
            std::ifstream file(secretFile, std::ios::binary);
            if (!file.is_open()) {
                std::cerr << "Error: Cannot open file: " << secretFile << std::endl;
                return 1;
            }
            file.seekg(0, std::ios::end);
            size_t size = file.tellg();
            file.seekg(0, std::ios::beg);
            secretData.resize(size);
            file.read(reinterpret_cast<char*>(secretData.data()), size);
        } else {
            // Read from stdin
            std::string line;
            while (std::getline(std::cin, line)) {
                secretData.append(line.c_str());
                secretData.append("\n");
            }
            secretData.chop(1); // Remove trailing newline
        }

        QString base64Data = QString::fromUtf8(secretData.toBase64());
        QDBusReply<bool> reply = interface.call("StoreSecret",
                                                 QString::fromStdString(secretId),
                                                 base64Data,
                                                 QString::fromStdString(secretType),
                                                 QString::fromStdString(description),
                                                 QString::fromStdString(expiresAt));
        
        if (reply.isValid() && reply.value()) {
            std::cout << "Secret stored successfully: " << secretId << std::endl;
        } else {
            std::cerr << "Error: Failed to store secret" << std::endl;
            if (!reply.isValid()) {
                std::cerr << "  " << reply.error().message().toStdString() << std::endl;
            }
            return 1;
        }
    }

    // Handle get command
    if (getCmd->parsed()) {
        QDBusReply<QString> reply = interface.call("GetSecret",
                                                   QString::fromStdString(getSecretId),
                                                   QString::fromStdString(version));
        
        if (reply.isValid() && !reply.value().isEmpty()) {
            QByteArray decodedData = QByteArray::fromBase64(reply.value().toUtf8());
            
            if (!outputFile.empty()) {
                // Write to file
                std::ofstream file(outputFile, std::ios::binary);
                if (!file.is_open()) {
                    std::cerr << "Error: Cannot open output file: " << outputFile << std::endl;
                    return 1;
                }
                file.write(reinterpret_cast<const char*>(decodedData.data()), decodedData.size());
                std::cout << "Secret written to: " << outputFile << std::endl;
            } else {
                // Write to stdout
                std::cout.write(reinterpret_cast<const char*>(decodedData.data()), decodedData.size());
            }
        } else {
            std::cerr << "Error: Secret not found or access denied" << std::endl;
            if (!reply.isValid()) {
                std::cerr << "  " << reply.error().message().toStdString() << std::endl;
            }
            return 1;
        }
    }

    // Handle rotate command
    if (rotateCmd->parsed()) {
        QDBusReply<bool> reply = interface.call("RotateSecret",
                                                QString::fromStdString(rotateSecretId));
        
        if (reply.isValid() && reply.value()) {
            std::cout << "Secret rotated successfully: " << rotateSecretId << std::endl;
        } else {
            std::cerr << "Error: Failed to rotate secret" << std::endl;
            if (!reply.isValid()) {
                std::cerr << "  " << reply.error().message().toStdString() << std::endl;
            }
            return 1;
        }
    }

    // Handle list command
    if (listCmd->parsed()) {
        QDBusReply<QStringList> reply = interface.call("ListSecrets");
        
        if (reply.isValid()) {
            QStringList secrets = reply.value();
            if (secrets.isEmpty()) {
                std::cout << "No secrets found" << std::endl;
            } else {
                std::cout << "Secrets:" << std::endl;
                for (const QString& secret : secrets) {
                    std::cout << "  " << secret.toStdString() << std::endl;
                }
            }
        } else {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
    }

    // Handle metadata command
    if (metadataCmd->parsed()) {
        QDBusReply<QVariantMap> reply = interface.call("GetSecretMetadata",
                                                      QString::fromStdString(metadataSecretId));
        
        if (reply.isValid()) {
            QVariantMap metadata = reply.value();
            if (metadata.isEmpty()) {
                std::cerr << "Error: Secret not found" << std::endl;
                return 1;
            }
            
            std::cout << "Secret Metadata:" << std::endl;
            std::cout << "  ID: " << metadata["secret_id"].toString().toStdString() << std::endl;
            std::cout << "  Type: " << metadata["secret_type"].toString().toStdString() << std::endl;
            std::cout << "  Description: " << metadata["description"].toString().toStdString() << std::endl;
            std::cout << "  Created: " << metadata["created_at"].toString().toStdString() << std::endl;
            std::cout << "  Updated: " << metadata["updated_at"].toString().toStdString() << std::endl;
            std::cout << "  Expires: " << metadata["expires_at"].toString().toStdString() << std::endl;
            std::cout << "  Owner: " << metadata["owner"].toString().toStdString() << std::endl;
            std::cout << "  Encrypted: " << (metadata["is_encrypted"].toBool() ? "yes" : "no") << std::endl;
        } else {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
    }

    // Handle rollback command
    if (rollbackCmd->parsed()) {
        QDBusReply<bool> reply = interface.call("RollbackSecret",
                                                QString::fromStdString(rollbackSecretId),
                                                QString::fromStdString(rollbackVersion));
        
        if (reply.isValid() && reply.value()) {
            std::cout << "Secret rolled back successfully: " << rollbackSecretId << " to version " << rollbackVersion << std::endl;
        } else {
            std::cerr << "Error: Failed to rollback secret" << std::endl;
            if (!reply.isValid()) {
                std::cerr << "  " << reply.error().message().toStdString() << std::endl;
            }
            return 1;
        }
    }

    // Handle delete command
    if (deleteCmd->parsed()) {
        QDBusReply<bool> reply = interface.call("DeleteSecret",
                                                QString::fromStdString(deleteSecretId));
        
        if (reply.isValid() && reply.value()) {
            std::cout << "Secret deleted successfully: " << deleteSecretId << std::endl;
        } else {
            std::cerr << "Error: Failed to delete secret" << std::endl;
            if (!reply.isValid()) {
                std::cerr << "  " << reply.error().message().toStdString() << std::endl;
            }
            return 1;
        }
    }

    return 0;
}

