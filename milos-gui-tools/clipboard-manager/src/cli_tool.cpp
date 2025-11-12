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

class ClipboardManagerClient {
public:
    ClipboardManagerClient() {
        m_connection = QDBusConnection::sessionBus();
        m_interface = new QDBusInterface(
            "org.milos.ClipboardManager",
            "/org/milos/ClipboardManager",
            "org.milos.ClipboardManager",
            m_connection,
            this
        );
    }

    bool isConnected() const {
        return m_connection.isConnected() && m_interface->isValid();
    }

    bool copyToClipboard(const QString& data) {
        QDBusReply<bool> reply = m_interface->call("CopyToClipboard", data);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

    QString getClipboard() {
        QDBusReply<QString> reply = m_interface->call("GetClipboard");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    bool clearClipboard() {
        QDBusReply<bool> reply = m_interface->call("ClearClipboard");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

    QString getClipboardHistory() {
        QDBusReply<QString> reply = m_interface->call("GetClipboardHistory");
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

void printHistory(const QString& jsonHistory) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonHistory.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        std::cerr << "Error parsing history: " << error.errorString().toStdString() << std::endl;
        return;
    }

    QJsonArray history = doc.array();
    std::cout << "Clipboard History (" << history.size() << " entries):" << std::endl;
    for (int i = 0; i < history.size(); i++) {
        QJsonObject entry = history[i].toObject();
        std::cout << "  " << (i + 1) << ". [" << entry["data_type"].toString().toStdString() << "] "
                  << "Timestamp: " << entry["timestamp"].toInt() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    app.setApplicationName("milos-clipboard");
    app.setOrganizationName("MilOS");

    CLI::App cliApp{"MilOS Secure Clipboard CLI Tool", "milos-clipboard"};
    cliApp.require_subcommand(0, 1);

    // Copy command
    auto* copyCmd = cliApp.add_subcommand("copy", "Copy data to clipboard");
    std::string data;
    copyCmd->add_option("data", data, "Data to copy to clipboard")->required();

    // Get command
    auto* getCmd = cliApp.add_subcommand("get", "Get clipboard data");

    // Clear command
    auto* clearCmd = cliApp.add_subcommand("clear", "Clear clipboard");

    // History command
    auto* historyCmd = cliApp.add_subcommand("history", "Show clipboard history");

    try {
        cliApp.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return cliApp.exit(e);
    }

    // Create D-Bus client
    ClipboardManagerClient client;

    if (!client.isConnected()) {
        std::cerr << "Error: Cannot connect to Clipboard Manager service." << std::endl;
        std::cerr << "Make sure the service is running: milos-clipboard-manager" << std::endl;
        return 1;
    }

    // Execute commands
    if (*copyCmd) {
        bool success = client.copyToClipboard(QString::fromStdString(data));
        if (success) {
            std::cout << "Data copied to clipboard" << std::endl;
            return 0;
        } else {
            std::cerr << "Error: Failed to copy data to clipboard." << std::endl;
            return 1;
        }
    } else if (*getCmd) {
        QString clipboard = client.getClipboard();
        if (clipboard.isEmpty()) {
            std::cout << "Clipboard is empty" << std::endl;
        } else {
            std::cout << clipboard.toStdString() << std::endl;
        }
        return 0;
    } else if (*clearCmd) {
        bool success = client.clearClipboard();
        if (success) {
            std::cout << "Clipboard cleared" << std::endl;
            return 0;
        } else {
            std::cerr << "Error: Failed to clear clipboard." << std::endl;
            return 1;
        }
    } else if (*historyCmd) {
        QString historyJson = client.getClipboardHistory();
        if (historyJson.isEmpty()) {
            std::cout << "No clipboard history" << std::endl;
            return 0;
        }
        printHistory(historyJson);
        return 0;
    } else {
        std::cout << cliApp.help() << std::endl;
    }

    return 0;
}

