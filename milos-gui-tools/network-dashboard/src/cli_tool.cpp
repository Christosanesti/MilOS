#include <CLI/CLI.hpp>
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>
#include <string>
#include <fstream>
#include <iterator>

class NetworkDashboardClient {
public:
    NetworkDashboardClient() {
        m_connection = QDBusConnection::systemBus();
        m_interface = new QDBusInterface(
            "org.milos.NetworkDashboard",
            "/org/milos/NetworkDashboard",
            "org.milos.NetworkDashboard",
            m_connection,
            this
        );
    }

    bool isConnected() const {
        return m_connection.isConnected() && m_interface->isValid();
    }

    bool startCapture(const QString& interface = QString(), const QString& filter = QString()) {
        QDBusReply<bool> reply = m_interface->call("StartCapture", interface, filter);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

    bool stopCapture() {
        QDBusReply<bool> reply = m_interface->call("StopCapture");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

    QString getPacketStats() {
        QDBusReply<QString> reply = m_interface->call("GetPacketStats");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString getThreats() {
        QDBusReply<QString> reply = m_interface->call("GetThreats");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    bool configureIDS(const QString& rulesJson) {
        QDBusReply<bool> reply = m_interface->call("ConfigureIDS", rulesJson);
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

void printStatistics(const QString& jsonStats) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStats.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        std::cerr << "Error parsing statistics: " << error.errorString().toStdString() << std::endl;
        return;
    }

    QJsonObject obj = doc.object();
    std::cout << "Packet Statistics:" << std::endl;
    std::cout << "  Total Packets: " << obj["total_packets"].toInt() << std::endl;
    std::cout << "  Total Bytes: " << obj["total_bytes"].toInt() << std::endl;

    if (obj.contains("protocols")) {
        QJsonObject protocols = obj["protocols"].toObject();
        std::cout << "  Protocol Distribution:" << std::endl;
        for (auto it = protocols.begin(); it != protocols.end(); ++it) {
            QJsonObject protoStats = it.value().toObject();
            std::cout << "    " << it.key().toStdString() << ": "
                      << protoStats["packet_count"].toInt() << " packets, "
                      << protoStats["byte_count"].toInt() << " bytes ("
                      << QString::number(protoStats["percentage_packets"].toDouble(), 'f', 2).toStdString()
                      << "% packets, "
                      << QString::number(protoStats["percentage_bytes"].toDouble(), 'f', 2).toStdString()
                      << "% bytes)" << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    app.setApplicationName("milos-network-monitor");
    app.setOrganizationName("MilOS");

    CLI::App cliApp{"MilOS Network Monitor CLI Tool", "milos-network-monitor"};
    cliApp.require_subcommand(0, 1);

    // Capture command
    auto* captureCmd = cliApp.add_subcommand("capture", "Start/stop packet capture");
    std::string interface;
    std::string filter;
    bool start = false;
    bool stop = false;
    bool stats = false;
    
    captureCmd->add_option("-i,--interface", interface, "Network interface");
    captureCmd->add_option("-f,--filter", filter, "BPF filter");
    captureCmd->add_flag("--start", start, "Start capture");
    captureCmd->add_flag("--stop", stop, "Stop capture");
    captureCmd->add_flag("--stats", stats, "Show statistics");

    // Analyze command
    auto* analyzeCmd = cliApp.add_subcommand("analyze", "Analyze network traffic");

    // Threats command
    auto* threatsCmd = cliApp.add_subcommand("threats", "Display threat information");

    // IDS Config command
    auto* idsConfigCmd = cliApp.add_subcommand("ids-config", "Manage IDS configuration");
    std::string configFile;
    bool show = false;
    bool reload = false;
    
    idsConfigCmd->add_option("-f,--file", configFile, "IDS rules configuration file");
    idsConfigCmd->add_flag("--show", show, "Show current IDS configuration");
    idsConfigCmd->add_flag("--reload", reload, "Reload IDS configuration");

    try {
        cliApp.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return cliApp.exit(e);
    }

    // Create D-Bus client
    NetworkDashboardClient client;

    if (!client.isConnected()) {
        std::cerr << "Error: Cannot connect to Network Dashboard service." << std::endl;
        std::cerr << "Make sure the service is running: milos-network-dashboard" << std::endl;
        return 1;
    }

    // Execute commands
    if (*analyzeCmd) {
        QString statsJson = client.getPacketStats();
        if (statsJson.isEmpty()) {
            return 1;
        }
        printStatistics(statsJson);
        return 0;
    } else if (*threatsCmd) {
        QString threatsJson = client.getThreats();
        if (threatsJson.isEmpty()) {
            std::cout << "No threats detected." << std::endl;
            return 0;
        }
        
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(threatsJson.toUtf8(), &error);
        if (error.error != QJsonParseError::NoError) {
            std::cerr << "Error parsing threats: " << error.errorString().toStdString() << std::endl;
            return 1;
        }
        
        QJsonArray threats = doc.array();
        std::cout << "Threats detected: " << threats.size() << std::endl;
        for (int i = 0; i < threats.size(); i++) {
            QJsonObject threat = threats[i].toObject();
            std::cout << "  Threat " << (i + 1) << ": " << threat["title"].toString().toStdString() << std::endl;
        }
        return 0;
    } else if (*idsConfigCmd) {
        if (show) {
            // Show current configuration
            QString threatsJson = client.getThreats();
            if (!threatsJson.isEmpty()) {
                std::cout << "Current IDS Configuration:" << std::endl;
                std::cout << threatsJson.toStdString() << std::endl;
            }
            return 0;
        } else if (reload) {
            // Reload configuration
            if (configFile.empty()) {
                std::cerr << "Error: Configuration file required for reload" << std::endl;
                return 1;
            }
            // Read file and configure
            std::ifstream file(configFile);
            if (!file.is_open()) {
                std::cerr << "Error: Cannot open configuration file: " << configFile << std::endl;
                return 1;
            }
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            bool success = client.configureIDS(QString::fromStdString(content));
            if (success) {
                std::cout << "IDS configuration reloaded successfully" << std::endl;
                return 0;
            } else {
                std::cerr << "Error: Failed to reload IDS configuration" << std::endl;
                return 1;
            }
        } else {
            std::cout << "Use --show or --reload with ids-config command" << std::endl;
        }
    } else if (*captureCmd) {
        if (start) {
            bool success = client.startCapture(
                interface.empty() ? QString() : QString::fromStdString(interface),
                filter.empty() ? QString() : QString::fromStdString(filter)
            );
            if (success) {
                std::cout << "Packet capture started" << std::endl;
                return 0;
            } else {
                std::cerr << "Error: Failed to start packet capture." << std::endl;
                return 1;
            }
        } else if (stop) {
            bool success = client.stopCapture();
            if (success) {
                std::cout << "Packet capture stopped" << std::endl;
                return 0;
            } else {
                std::cerr << "Error: Failed to stop packet capture." << std::endl;
                return 1;
            }
        } else if (stats) {
            QString statsJson = client.getPacketStats();
            if (statsJson.isEmpty()) {
                return 1;
            }
            printStatistics(statsJson);
        } else {
            std::cout << "Use --start, --stop, or --stats with capture command" << std::endl;
        }
    } else {
        std::cout << cliApp.help() << std::endl;
    }

    return 0;
}

