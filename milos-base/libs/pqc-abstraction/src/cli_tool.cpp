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

class PQCAbstractionClient {
public:
    PQCAbstractionClient() {
        m_connection = QDBusConnection::systemBus();
        m_interface = new QDBusInterface(
            "org.milos.PQCAbstraction",
            "/org/milos/PQCAbstraction",
            "org.milos.PQCAbstraction",
            m_connection,
            this
        );
    }

    bool isConnected() const {
        return m_connection.isConnected() && m_interface->isValid();
    }

    QString getAvailableAlgorithms() {
        QDBusReply<QString> reply = m_interface->call("GetAvailableAlgorithms");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    bool selectAlgorithm(const QString& algorithmId) {
        QDBusReply<bool> reply = m_interface->call("SelectAlgorithm", algorithmId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

    QString getAlgorithmInfo(const QString& algorithmId) {
        QDBusReply<QString> reply = m_interface->call("GetAlgorithmInfo", algorithmId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString encrypt(const QString& data, const QString& algorithmId = QString()) {
        QDBusReply<QString> reply = m_interface->call("Encrypt", data, algorithmId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString decrypt(const QString& encryptedData, const QString& algorithmId = QString()) {
        QDBusReply<QString> reply = m_interface->call("Decrypt", encryptedData, algorithmId);
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

void printAlgorithms(const QString& jsonAlgorithms) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonAlgorithms.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        std::cerr << "Error parsing algorithms: " << error.errorString().toStdString() << std::endl;
        return;
    }

    QJsonObject obj = doc.object();
    if (obj.contains("algorithms")) {
        QJsonArray algorithms = obj["algorithms"].toArray();
        int count = obj["count"].toInt();
        std::cout << "Available Algorithms (" << count << "):" << std::endl;
        for (const auto& algValue : algorithms) {
            QJsonObject alg = algValue.toObject();
            std::cout << "  - " << alg["algorithm_id"].toString().toStdString()
                      << " (" << alg["name"].toString().toStdString() << ")"
                      << " - " << alg["version"].toString().toStdString() << std::endl;
            if (alg["is_nist_approved"].toBool()) {
                std::cout << "    [NIST Approved]" << std::endl;
            }
        }
    }
}

void printAlgorithmInfo(const QString& jsonInfo) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonInfo.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        std::cerr << "Error parsing algorithm info: " << error.errorString().toStdString() << std::endl;
        return;
    }

    QJsonObject obj = doc.object();
    if (obj.contains("error")) {
        std::cerr << "Error: " << obj["error"].toString().toStdString() << std::endl;
        return;
    }

    std::cout << "Algorithm Information:" << std::endl;
    std::cout << "  ID: " << obj["algorithm_id"].toString().toStdString() << std::endl;
    std::cout << "  Name: " << obj["name"].toString().toStdString() << std::endl;
    std::cout << "  Version: " << obj["version"].toString().toStdString() << std::endl;
    std::cout << "  Description: " << obj["description"].toString().toStdString() << std::endl;
    std::cout << "  Key Size: " << obj["key_size"].toInt() << " bytes" << std::endl;
    std::cout << "  NIST Approved: " << (obj["is_nist_approved"].toBool() ? "Yes" : "No") << std::endl;
    std::cout << "  Hybrid Supported: " << (obj["is_hybrid_supported"].toBool() ? "Yes" : "No") << std::endl;
    std::cout << "  Performance Score: " << obj["performance_score"].toDouble() << std::endl;
}

int main(int argc, char* argv[]) {
    // Create QCoreApplication for Qt D-Bus
    QCoreApplication app(argc, argv);
    app.setApplicationName("milos-pqc");
    app.setOrganizationName("MilOS");

    CLI::App cliApp{"MilOS PQC Abstraction Layer CLI Tool", "milos-pqc"};
    cliApp.require_subcommand(0, 1);

    // List command
    auto* listCmd = cliApp.add_subcommand("list", "List available algorithms");

    // Select command
    auto* selectCmd = cliApp.add_subcommand("select", "Select algorithm");
    std::string algorithmId;
    selectCmd->add_option("algorithm-id", algorithmId, "Algorithm ID")->required();

    // Info command
    auto* infoCmd = cliApp.add_subcommand("info", "Display algorithm information");
    std::string infoAlgorithmId;
    infoCmd->add_option("algorithm-id", infoAlgorithmId, "Algorithm ID")->required();

    // Encrypt command
    auto* encryptCmd = cliApp.add_subcommand("encrypt", "Encrypt data");
    std::string inputFile;
    std::string outputFile;
    std::string encryptAlgorithmId;
    encryptCmd->add_option("input", inputFile, "Input file")->required();
    encryptCmd->add_option("-o,--output", outputFile, "Output file");
    encryptCmd->add_option("-a,--algorithm", encryptAlgorithmId, "Algorithm ID");

    // Decrypt command
    auto* decryptCmd = cliApp.add_subcommand("decrypt", "Decrypt data");
    std::string decryptInputFile;
    std::string decryptOutputFile;
    std::string decryptAlgorithmId;
    decryptCmd->add_option("input", decryptInputFile, "Input file")->required();
    decryptCmd->add_option("-o,--output", decryptOutputFile, "Output file");
    decryptCmd->add_option("-a,--algorithm", decryptAlgorithmId, "Algorithm ID");

    try {
        cliApp.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return cliApp.exit(e);
    }

    // Create D-Bus client
    PQCAbstractionClient client;

    if (!client.isConnected()) {
        std::cerr << "Error: Cannot connect to PQC Abstraction service." << std::endl;
        std::cerr << "Make sure the service is running: milos-pqc-service" << std::endl;
        return 1;
    }

    // Execute commands
    if (*listCmd) {
        QString algorithms = client.getAvailableAlgorithms();
        if (algorithms.isEmpty()) {
            return 1;
        }
        printAlgorithms(algorithms);
    } else if (*selectCmd) {
        bool success = client.selectAlgorithm(QString::fromStdString(algorithmId));
        if (success) {
            std::cout << "Algorithm selected: " << algorithmId << std::endl;
            return 0;
        } else {
            std::cerr << "Error: Failed to select algorithm." << std::endl;
            return 1;
        }
    } else if (*infoCmd) {
        QString info = client.getAlgorithmInfo(QString::fromStdString(infoAlgorithmId));
        if (info.isEmpty()) {
            return 1;
        }
        printAlgorithmInfo(info);
    } else if (*encryptCmd) {
        // Read input file
        QFile file(QString::fromStdString(inputFile));
        if (!file.open(QIODevice::ReadOnly)) {
            std::cerr << "Error: Cannot open input file: " << inputFile << std::endl;
            return 1;
        }
        QByteArray data = file.readAll();
        QString base64Data = QString::fromUtf8(data.toBase64());

        // Encrypt
        QString encrypted = client.encrypt(
            base64Data,
            encryptAlgorithmId.empty() ? QString() : QString::fromStdString(encryptAlgorithmId)
        );
        if (encrypted.isEmpty()) {
            return 1;
        }

        // Write output
        if (outputFile.empty()) {
            std::cout << encrypted.toStdString() << std::endl;
        } else {
            QFile outFile(QString::fromStdString(outputFile));
            if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                std::cerr << "Error: Cannot open output file: " << outputFile << std::endl;
                return 1;
            }
            outFile.write(encrypted.toUtf8());
            std::cout << "Encrypted data written to: " << outputFile << std::endl;
        }
    } else if (*decryptCmd) {
        // Read input file
        QFile file(QString::fromStdString(decryptInputFile));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            std::cerr << "Error: Cannot open input file: " << decryptInputFile << std::endl;
            return 1;
        }
        QString encryptedData = QString::fromUtf8(file.readAll());

        // Decrypt
        QString decrypted = client.decrypt(
            encryptedData,
            decryptAlgorithmId.empty() ? QString() : QString::fromStdString(decryptAlgorithmId)
        );
        if (decrypted.isEmpty()) {
            return 1;
        }

        // Parse result
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(decrypted.toUtf8(), &error);
        if (error.error != QJsonParseError::NoError || !doc.isObject()) {
            std::cerr << "Error: Invalid decryption result" << std::endl;
            return 1;
        }

        QJsonObject result = doc.object();
        if (!result["success"].toBool()) {
            std::cerr << "Error: " << result["error_message"].toString().toStdString() << std::endl;
            return 1;
        }

        // Decode plaintext
        QByteArray plaintext = QByteArray::fromBase64(
            result["plaintext"].toString().toUtf8()
        );

        // Write output
        if (decryptOutputFile.empty()) {
            std::cout << plaintext.toStdString() << std::endl;
        } else {
            QFile outFile(QString::fromStdString(decryptOutputFile));
            if (!outFile.open(QIODevice::WriteOnly)) {
                std::cerr << "Error: Cannot open output file: " << decryptOutputFile << std::endl;
                return 1;
            }
            outFile.write(plaintext);
            std::cout << "Decrypted data written to: " << decryptOutputFile << std::endl;
        }
    } else {
        std::cout << cliApp.help() << std::endl;
    }

    return 0;
}

