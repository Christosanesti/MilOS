#include <CLI/CLI.hpp>
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusError>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <iostream>
#include <string>

class PersonnelSchedulerClient {
public:
    PersonnelSchedulerClient() {
        m_connection = QDBusConnection::sessionBus();
        m_interface = new QDBusInterface(
            "org.milos.PersonnelScheduler",
            "/org/milos/PersonnelScheduler",
            "org.milos.PersonnelScheduler",
            m_connection,
            this
        );
    }

    bool isConnected() const {
        return m_connection.isConnected() && m_interface->isValid();
    }

    QStringList detectDevices(const QString& deviceType) {
        QDBusReply<QStringList> reply = m_interface->call("DetectDevices", deviceType);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QStringList();
        }
        return reply.value();
    }

    bool registerDevice(const QString& deviceId, const QString& deviceType) {
        QDBusReply<bool> reply = m_interface->call("RegisterDevice", deviceId, deviceType);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

    bool unregisterDevice(const QString& deviceId) {
        QDBusReply<bool> reply = m_interface->call("UnregisterDevice", deviceId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

    QStringList getRegisteredDevices() {
        QDBusReply<QStringList> reply = m_interface->call("GetRegisteredDevices");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QStringList();
        }
        return reply.value();
    }

    QVariantMap getDeviceInfo(const QString& deviceId) {
        QDBusReply<QVariantMap> reply = m_interface->call("GetDeviceInfo", deviceId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QVariantMap();
        }
        return reply.value();
    }

    QVariantMap getDeviceHealth(const QString& deviceId) {
        QDBusReply<QVariantMap> reply = m_interface->call("GetDeviceHealth", deviceId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QVariantMap();
        }
        return reply.value();
    }

    QString captureBiometric(const QString& deviceId, int timeoutMs) {
        QDBusReply<QString> reply = m_interface->call("CaptureBiometric", deviceId, timeoutMs);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    int verifyBiometric(const QString& deviceId, const QString& templateData, const QString& capturedData) {
        QDBusReply<int> reply = m_interface->call("VerifyBiometric", deviceId, templateData, capturedData);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 0;
        }
        return reply.value();
    }

    QString enrollBiometric(const QString& deviceId, const QString& capturedData) {
        QDBusReply<QString> reply = m_interface->call("EnrollBiometric", deviceId, capturedData);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QVariantMap getDeviceConfiguration(const QString& deviceId) {
        QDBusReply<QVariantMap> reply = m_interface->call("GetDeviceConfiguration", deviceId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QVariantMap();
        }
        return reply.value();
    }

    bool setDeviceConfiguration(const QString& deviceId, const QVariantMap& config) {
        QDBusReply<bool> reply = m_interface->call("SetDeviceConfiguration", deviceId, config);
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

void printDevices(const QStringList& devices) {
    std::cout << "Devices (" << devices.size() << " entries):" << std::endl;
    for (int i = 0; i < devices.size(); i++) {
        std::cout << "  " << (i + 1) << ". " << devices[i].toStdString() << std::endl;
    }
}

void printDeviceInfo(const QVariantMap& info) {
    std::cout << "Device Information:" << std::endl;
    std::cout << "  ID: " << info["device_id"].toString().toStdString() << std::endl;
    std::cout << "  Type: " << info["device_type"].toString().toStdString() << std::endl;
    std::cout << "  Vendor: " << info["vendor"].toString().toStdString() << std::endl;
    std::cout << "  Model: " << info["model"].toString().toStdString() << std::endl;
    std::cout << "  Connected: " << (info["connected"].toBool() ? "Yes" : "No") << std::endl;
}

void printDeviceHealth(const QVariantMap& health) {
    std::cout << "Device Health:" << std::endl;
    std::cout << "  Status: " << health["status"].toString().toStdString() << std::endl;
    std::cout << "  Health Score: " << health["health_score"].toInt() << std::endl;
    if (!health["error_message"].toString().isEmpty()) {
        std::cout << "  Error: " << health["error_message"].toString().toStdString() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    app.setApplicationName("milos-personnel");
    app.setOrganizationName("MilOS");

    CLI::App cliApp{"MilOS Personnel Scheduler CLI Tool", "milos-personnel"};
    cliApp.require_subcommand(0, 1);

    PersonnelSchedulerClient client;
    if (!client.isConnected()) {
        std::cerr << "Error: Cannot connect to Personnel Scheduler service" << std::endl;
        std::cerr << "Make sure milos-personnel-scheduler is running" << std::endl;
        return 1;
    }

    // Device detection command
    auto* detectCmd = cliApp.add_subcommand("detect", "Detect available devices");
    std::string deviceType;
    detectCmd->add_option("--type", deviceType, "Device type (face or fingerprint)")->required();
    detectCmd->callback([&]() {
        QStringList devices = client.detectDevices(QString::fromStdString(deviceType));
        printDevices(devices);
    });

    // Device registration command
    auto* registerCmd = cliApp.add_subcommand("register", "Register device");
    std::string registerDeviceId;
    std::string registerDeviceType;
    registerCmd->add_option("--id", registerDeviceId, "Device ID")->required();
    registerCmd->add_option("--type", registerDeviceType, "Device type (face or fingerprint)")->required();
    registerCmd->callback([&]() {
        bool success = client.registerDevice(
            QString::fromStdString(registerDeviceId),
            QString::fromStdString(registerDeviceType)
        );
        if (success) {
            std::cout << "Device registered successfully" << std::endl;
        } else {
            std::cerr << "Failed to register device" << std::endl;
        }
    });

    // List registered devices command
    auto* listCmd = cliApp.add_subcommand("list", "List registered devices");
    listCmd->callback([&]() {
        QStringList devices = client.getRegisteredDevices();
        printDevices(devices);
    });

    // Device info command
    auto* infoCmd = cliApp.add_subcommand("info", "Get device information");
    std::string infoDeviceId;
    infoCmd->add_option("--id", infoDeviceId, "Device ID")->required();
    infoCmd->callback([&]() {
        QVariantMap info = client.getDeviceInfo(QString::fromStdString(infoDeviceId));
        if (!info.isEmpty()) {
            printDeviceInfo(info);
        } else {
            std::cerr << "Device not found" << std::endl;
        }
    });

    // Device health command
    auto* healthCmd = cliApp.add_subcommand("health", "Get device health status");
    std::string healthDeviceId;
    healthCmd->add_option("--id", healthDeviceId, "Device ID")->required();
    healthCmd->callback([&]() {
        QVariantMap health = client.getDeviceHealth(QString::fromStdString(healthDeviceId));
        if (!health.isEmpty()) {
            printDeviceHealth(health);
        } else {
            std::cerr << "Device not found" << std::endl;
        }
    });

    // Capture biometric command
    auto* captureCmd = cliApp.add_subcommand("capture", "Capture biometric data");
    std::string captureDeviceId;
    int captureTimeout = 5000;
    captureCmd->add_option("--id", captureDeviceId, "Device ID")->required();
    captureCmd->add_option("--timeout", captureTimeout, "Timeout in milliseconds")->default_val(5000);
    captureCmd->callback([&]() {
        QString data = client.captureBiometric(QString::fromStdString(captureDeviceId), captureTimeout);
        if (!data.isEmpty()) {
            std::cout << "Biometric data captured (base64): " << data.toStdString() << std::endl;
        } else {
            std::cerr << "Failed to capture biometric data" << std::endl;
        }
    });

    // Verify biometric command
    auto* verifyCmd = cliApp.add_subcommand("verify", "Verify biometric data");
    std::string verifyDeviceId;
    std::string verifyTemplate;
    std::string verifyCaptured;
    verifyCmd->add_option("--id", verifyDeviceId, "Device ID")->required();
    verifyCmd->add_option("--template", verifyTemplate, "Template data (base64)")->required();
    verifyCmd->add_option("--captured", verifyCaptured, "Captured data (base64)")->required();
    verifyCmd->callback([&]() {
        int score = client.verifyBiometric(
            QString::fromStdString(verifyDeviceId),
            QString::fromStdString(verifyTemplate),
            QString::fromStdString(verifyCaptured)
        );
        std::cout << "Match score: " << score << std::endl;
    });

    // Enroll biometric command
    auto* enrollCmd = cliApp.add_subcommand("enroll", "Enroll biometric template");
    std::string enrollDeviceId;
    std::string enrollCaptured;
    enrollCmd->add_option("--id", enrollDeviceId, "Device ID")->required();
    enrollCmd->add_option("--captured", enrollCaptured, "Captured data (base64)")->required();
    enrollCmd->callback([&]() {
        QString templateData = client.enrollBiometric(
            QString::fromStdString(enrollDeviceId),
            QString::fromStdString(enrollCaptured)
        );
        if (!templateData.isEmpty()) {
            std::cout << "Template created (base64): " << templateData.toStdString() << std::endl;
        } else {
            std::cerr << "Failed to enroll biometric" << std::endl;
        }
    });

    CLI11_PARSE(cliApp, argc, argv);
    return 0;
}

