#include <CLI/CLI.hpp>
#include <QCoreApplication>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusConnection>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    app.setApplicationName("milos-tpm");

    CLI::App cliApp{"MilOS TPM Integration CLI Tool", "milos-tpm"};
    cliApp.require_subcommand(0, 1);

    // Create D-Bus interface
    QDBusInterface interface("org.milos.TPM",
                             "/org/milos/TPM",
                             "org.milos.TPM",
                             QDBusConnection::systemBus());

    if (!interface.isValid()) {
        std::cerr << "Error: Cannot connect to TPM Integration service" << std::endl;
        std::cerr << "Make sure the service is running: systemctl start milos-tpm-integration" << std::endl;
        return 1;
    }

    // Status command
    auto* statusCmd = cliApp.add_subcommand("status", "Show TPM status");

    // Generate key command
    auto* keyCmd = cliApp.add_subcommand("generate-key", "Generate key in TPM");
    std::string keyType = "RSA2048";
    std::string keyUsage = "signing";
    bool persistent = false;
    keyCmd->add_option("--type", keyType, "Key type (RSA2048, ECC256, etc.)");
    keyCmd->add_option("--usage", keyUsage, "Key usage (signing, encryption, storage)");
    keyCmd->add_flag("--persistent", persistent, "Store key persistently in TPM");

    // Boot measurement command
    auto* bootCmd = cliApp.add_subcommand("boot-measurement", "Show boot measurement");

    // Attest command
    auto* attestCmd = cliApp.add_subcommand("attest", "Generate attestation quote");
    std::string nonce;
    attestCmd->add_option("--nonce", nonce, "Nonce for quote (optional)");

    // Verify command
    auto* verifyCmd = cliApp.add_subcommand("verify", "Verify attestation quote");
    std::string quoteFile;
    verifyCmd->add_option("quote-file", quoteFile, "Path to quote file")->required();

    // Health command
    auto* healthCmd = cliApp.add_subcommand("health", "Show TPM health status");

    try {
        cliApp.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return cliApp.exit(e);
    }

    // Handle status command
    if (statusCmd->parsed()) {
        QDBusReply<QString> reply = interface.call("GetTPMStatus");
        if (reply.isValid()) {
            QJsonDocument doc = QJsonDocument::fromJson(reply.value().toUtf8());
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                std::cout << "TPM Status:" << std::endl;
                std::cout << "  Available: " << (obj["available"].toBool() ? "yes" : "no") << std::endl;
                std::cout << "  Device: " << obj["device_path"].toString().toStdString() << std::endl;
                std::cout << "  Version: " << obj["tpm_version"].toString().toStdString() << std::endl;
                std::cout << "  Status: " << obj["status"].toString().toStdString() << std::endl;
                if (obj.contains("health")) {
                    std::cout << "  Health: " << (obj["health"].toBool() ? "healthy" : "unhealthy") << std::endl;
                }
            }
        } else {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
    }

    // Handle generate-key command
    if (keyCmd->parsed()) {
        QJsonObject params;
        params["key_type"] = QString::fromStdString(keyType);
        params["key_usage"] = QString::fromStdString(keyUsage);
        params["persistent"] = persistent;

        QDBusReply<QString> reply = interface.call("GenerateKey", QJsonDocument(params).toJson());
        if (reply.isValid()) {
            QJsonDocument doc = QJsonDocument::fromJson(reply.value().toUtf8());
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                if (obj["success"].toBool()) {
                    std::cout << "Key generated successfully:" << std::endl;
                    std::cout << "  Key ID: " << obj["key_id"].toString().toStdString() << std::endl;
                    std::cout << "  Type: " << obj["key_type"].toString().toStdString() << std::endl;
                } else {
                    std::cerr << "Error: " << obj["error"].toString().toStdString() << std::endl;
                    return 1;
                }
            }
        } else {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
    }

    // Handle boot-measurement command
    if (bootCmd->parsed()) {
        QDBusReply<QString> reply = interface.call("GetBootMeasurement");
        if (reply.isValid()) {
            QJsonDocument doc = QJsonDocument::fromJson(reply.value().toUtf8());
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                if (obj["success"].toBool()) {
                    std::cout << "Boot Measurement (PCR values):" << std::endl;
                    QJsonArray pcrArray = obj["pcr_values"].toArray();
                    for (const QJsonValue& value : pcrArray) {
                        QJsonObject pcr = value.toObject();
                        std::cout << "  PCR" << pcr["pcr_index"].toInt() 
                                  << ": " << pcr["value"].toString().toStdString() << std::endl;
                    }
                }
            }
        } else {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
    }

    // Handle attest command
    if (attestCmd->parsed()) {
        QString nonceStr = nonce.empty() ? "" : QString::fromStdString(nonce);
        QDBusReply<QString> reply = interface.call("GenerateAttestation", nonceStr);
        if (reply.isValid()) {
            QJsonDocument doc = QJsonDocument::fromJson(reply.value().toUtf8());
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                if (obj["success"].toBool()) {
                    std::cout << "Attestation quote generated:" << std::endl;
                    std::cout << "  Quote Data: " << obj["quote_data"].toString().toStdString() << std::endl;
                    std::cout << "  Signature: " << obj["signature"].toString().toStdString() << std::endl;
                    std::cout << "  Nonce: " << obj["nonce"].toString().toStdString() << std::endl;
                }
            }
        } else {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
    }

    // Handle verify command
    if (verifyCmd->parsed()) {
        std::ifstream file(quoteFile);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open quote file: " << quoteFile << std::endl;
            return 1;
        }

        std::string quoteJson((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
        file.close();

        QDBusReply<QString> reply = interface.call("VerifyAttestation", QString::fromStdString(quoteJson));
        if (reply.isValid()) {
            QJsonDocument doc = QJsonDocument::fromJson(reply.value().toUtf8());
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                std::cout << "Verification result:" << std::endl;
                std::cout << "  Valid: " << (obj["is_valid"].toBool() ? "yes" : "no") << std::endl;
                if (!obj["error_message"].toString().isEmpty()) {
                    std::cout << "  Error: " << obj["error_message"].toString().toStdString() << std::endl;
                }
            }
        } else {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
    }

    // Handle health command
    if (healthCmd->parsed()) {
        QDBusReply<QString> reply = interface.call("GetTPMStatus");
        if (reply.isValid()) {
            QJsonDocument doc = QJsonDocument::fromJson(reply.value().toUtf8());
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                std::cout << "TPM Health Status:" << std::endl;
                if (obj.contains("health")) {
                    std::cout << "  Status: " << (obj["health"].toBool() ? "healthy" : "unhealthy") << std::endl;
                    std::cout << "  Message: " << obj["health_message"].toString().toStdString() << std::endl;
                }
            }
        } else {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
    }

    if (!cliApp.get_subcommands().empty() && !cliApp.get_subcommands()[0]->parsed()) {
        std::cout << cliApp.help() << std::endl;
    }

    return 0;
}

