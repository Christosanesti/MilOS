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

class AuditServiceClient {
public:
    AuditServiceClient() {
        m_connection = QDBusConnection::systemBus();
        m_interface = new QDBusInterface(
            "org.milos.AuditService",
            "/org/milos/AuditService",
            "org.milos.AuditService",
            m_connection,
            this
        );
    }

    bool isConnected() const {
        return m_connection.isConnected() && m_interface->isValid();
    }

    QString logEvent(const QString& eventData) {
        QDBusReply<QString> reply = m_interface->call("LogEvent", eventData);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString queryEvents(const QString& queryParams) {
        QDBusReply<QString> reply = m_interface->call("QueryEvents", queryParams);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString getEventStatistics(const QString& queryParams) {
        QDBusReply<QString> reply = m_interface->call("GetEventStatistics", queryParams);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString exportAuditLog(const QString& timeRange, const QString& format) {
        QDBusReply<QString> reply = m_interface->call("ExportAuditLog", timeRange, format);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    bool verifyLogIntegrity() {
        QDBusReply<bool> reply = m_interface->call("VerifyLogIntegrity");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
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

void printQueryResults(const QString& jsonResults) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonResults.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        std::cerr << "Error parsing results: " << error.errorString().toStdString() << std::endl;
        return;
    }

    QJsonObject obj = doc.object();
    if (obj.contains("entries")) {
        QJsonArray entries = obj["entries"].toArray();
        std::cout << "Found " << entries.size() << " entries:" << std::endl;
        for (const auto& entryValue : entries) {
            QJsonObject entry = entryValue.toObject();
            std::cout << "  - " << entry["event_type"].toString().toStdString() 
                      << " (" << entry["timestamp"].toString().toStdString() << ")" << std::endl;
        }
    }
}

void printStatistics(const QString& jsonStats) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStats.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        std::cerr << "Error parsing statistics: " << error.errorString().toStdString() << std::endl;
        return;
    }

    QJsonObject obj = doc.object();
    std::cout << "Event Statistics:" << std::endl;
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        std::cout << "  " << it.key().toStdString() << ": " << it.value().toInt() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    // Create QCoreApplication for Qt D-Bus
    QCoreApplication app(argc, argv);
    app.setApplicationName("milos-audit");
    app.setOrganizationName("MilOS");

    CLI::App cliApp{"MilOS Audit Service CLI Tool", "milos-audit"};
    cliApp.require_subcommand(0, 1);

    // Log command
    auto* logCmd = cliApp.add_subcommand("log", "Log a security event");
    std::string eventFile;
    logCmd->add_option("event-file", eventFile, "Event data file (JSON)")->required();

    // Query command
    auto* queryCmd = cliApp.add_subcommand("query", "Query audit logs");
    std::string queryFile;
    queryCmd->add_option("query-file", queryFile, "Query parameters file (JSON)")->required();

    // Stats command
    auto* statsCmd = cliApp.add_subcommand("stats", "Get event statistics");
    std::string statsFile;
    statsCmd->add_option("query-file", statsFile, "Query parameters file (JSON)")->required();

    // Export command
    auto* exportCmd = cliApp.add_subcommand("export", "Export audit logs");
    std::string timeRangeFile;
    std::string format = "json";
    exportCmd->add_option("time-range-file", timeRangeFile, "Time range file (JSON)")->required();
    exportCmd->add_option("--format", format, "Export format (json, csv, xml)")->default_val("json");

    // Verify command
    auto* verifyCmd = cliApp.add_subcommand("verify", "Verify log integrity");

    // Health command
    auto* healthCmd = cliApp.add_subcommand("health", "Display service health status");

    try {
        cliApp.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return cliApp.exit(e);
    }

    // Create D-Bus client
    AuditServiceClient client;

    if (!client.isConnected()) {
        std::cerr << "Error: Cannot connect to Audit Service." << std::endl;
        std::cerr << "Make sure the service is running: systemctl status milos-audit-service" << std::endl;
        return 1;
    }

    // Execute commands
    if (*logCmd) {
        QFile file(QString::fromStdString(eventFile));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            std::cerr << "Error: Cannot open event file: " << eventFile << std::endl;
            return 1;
        }

        QByteArray eventData = file.readAll();
        QString eventId = client.logEvent(QString::fromUtf8(eventData));
        if (eventId.isEmpty()) {
            return 1;
        }
        std::cout << "Event logged successfully. Event ID: " << eventId.toStdString() << std::endl;
    } else if (*queryCmd) {
        QFile file(QString::fromStdString(queryFile));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            std::cerr << "Error: Cannot open query file: " << queryFile << std::endl;
            return 1;
        }

        QByteArray queryData = file.readAll();
        QString results = client.queryEvents(QString::fromUtf8(queryData));
        if (results.isEmpty()) {
            return 1;
        }
        printQueryResults(results);
    } else if (*statsCmd) {
        QFile file(QString::fromStdString(statsFile));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            std::cerr << "Error: Cannot open query file: " << statsFile << std::endl;
            return 1;
        }

        QByteArray queryData = file.readAll();
        QString stats = client.getEventStatistics(QString::fromUtf8(queryData));
        if (stats.isEmpty()) {
            return 1;
        }
        printStatistics(stats);
    } else if (*exportCmd) {
        QFile file(QString::fromStdString(timeRangeFile));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            std::cerr << "Error: Cannot open time range file: " << timeRangeFile << std::endl;
            return 1;
        }

        QByteArray timeRangeData = file.readAll();
        QString exported = client.exportAuditLog(QString::fromUtf8(timeRangeData), QString::fromStdString(format));
        if (exported.isEmpty()) {
            return 1;
        }
        std::cout << exported.toStdString() << std::endl;
    } else if (*verifyCmd) {
        bool verified = client.verifyLogIntegrity();
        if (verified) {
            std::cout << "Log integrity verified successfully." << std::endl;
            return 0;
        } else {
            std::cerr << "Log integrity verification failed! Tampering detected." << std::endl;
            return 1;
        }
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

