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

    QString recordAttendanceEntry(const QString& deviceId, const QString& personnelId, const QString& location) {
        QDBusReply<QString> reply = m_interface->call("RecordAttendanceEntry", deviceId, personnelId, location);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString recordAttendanceExit(const QString& deviceId, const QString& personnelId, const QString& location) {
        QDBusReply<QString> reply = m_interface->call("RecordAttendanceExit", deviceId, personnelId, location);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString getAttendanceRecords(const QString& personnelId, const QString& startDate, const QString& endDate) {
        QDBusReply<QString> reply = m_interface->call("GetAttendanceRecords", personnelId, startDate, endDate);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString getAttendanceRecord(const QString& recordId) {
        QDBusReply<QString> reply = m_interface->call("GetAttendanceRecord", recordId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    bool isPersonnelPresent(const QString& personnelId) {
        QDBusReply<bool> reply = m_interface->call("IsPersonnelPresent", personnelId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

    QString createShift(const QString& personnelId, const QString& startDateTime, const QString& endDateTime, int shiftType, const QString& location) {
        QDBusReply<QString> reply = m_interface->call("CreateShift", personnelId, startDateTime, endDateTime, shiftType, location);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString getShifts(const QString& personnelId, const QString& startDate, const QString& endDate) {
        QDBusReply<QString> reply = m_interface->call("GetShifts", personnelId, startDate, endDate);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString detectConflicts(const QString& startDate, const QString& endDate) {
        QDBusReply<QString> reply = m_interface->call("DetectConflicts", startDate, endDate);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    QString createLeaveRequest(const QString& personnelId, const QString& startDate, const QString& endDate, int leaveType, const QString& reason) {
        QDBusReply<QString> reply = m_interface->call("CreateLeaveRequest", personnelId, startDate, endDate, leaveType, reason);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    bool approveLeaveRequest(const QString& requestId, const QString& approverId) {
        QDBusReply<bool> reply = m_interface->call("ApproveLeaveRequest", requestId, approverId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

    QString createSwapRequest(const QString& shiftId, const QString& requesterId, const QString& targetPersonnelId, const QString& reason) {
        QDBusReply<QString> reply = m_interface->call("CreateSwapRequest", shiftId, requesterId, targetPersonnelId, reason);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return QString();
        }
        return reply.value();
    }

    bool approveSwapRequest(const QString& swapId, const QString& approverId) {
        QDBusReply<bool> reply = m_interface->call("ApproveSwapRequest", swapId, approverId);
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return false;
        }
        return reply.value();
    }

    QString createCoverageRequest(const QString& shiftId, const QString& requesterId, const QString& reason) {
        QDBusReply<QString> reply = m_interface->call("CreateCoverageRequest", shiftId, requesterId, reason);
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

    // Record attendance entry command
    auto* entryCmd = cliApp.add_subcommand("entry", "Record attendance entry");
    std::string entryDeviceId;
    std::string entryPersonnelId;
    std::string entryLocation;
    entryCmd->add_option("--device", entryDeviceId, "Device ID")->required();
    entryCmd->add_option("--personnel", entryPersonnelId, "Personnel ID")->required();
    entryCmd->add_option("--location", entryLocation, "Location/area")->required();
    entryCmd->callback([&]() {
        QString recordId = client.recordAttendanceEntry(
            QString::fromStdString(entryDeviceId),
            QString::fromStdString(entryPersonnelId),
            QString::fromStdString(entryLocation)
        );
        if (!recordId.isEmpty()) {
            std::cout << "Attendance entry recorded: " << recordId.toStdString() << std::endl;
        } else {
            std::cerr << "Failed to record attendance entry" << std::endl;
        }
    });

    // Record attendance exit command
    auto* exitCmd = cliApp.add_subcommand("exit", "Record attendance exit");
    std::string exitDeviceId;
    std::string exitPersonnelId;
    std::string exitLocation;
    exitCmd->add_option("--device", exitDeviceId, "Device ID")->required();
    exitCmd->add_option("--personnel", exitPersonnelId, "Personnel ID")->required();
    exitCmd->add_option("--location", exitLocation, "Location/area")->required();
    exitCmd->callback([&]() {
        QString recordId = client.recordAttendanceExit(
            QString::fromStdString(exitDeviceId),
            QString::fromStdString(exitPersonnelId),
            QString::fromStdString(exitLocation)
        );
        if (!recordId.isEmpty()) {
            std::cout << "Attendance exit recorded: " << recordId.toStdString() << std::endl;
        } else {
            std::cerr << "Failed to record attendance exit" << std::endl;
        }
    });

    // List attendance records command
    auto* recordsCmd = cliApp.add_subcommand("records", "List attendance records");
    std::string recordsPersonnelId;
    std::string recordsStartDate;
    std::string recordsEndDate;
    recordsCmd->add_option("--personnel", recordsPersonnelId, "Personnel ID filter");
    recordsCmd->add_option("--start", recordsStartDate, "Start date (ISO format)");
    recordsCmd->add_option("--end", recordsEndDate, "End date (ISO format)");
    recordsCmd->callback([&]() {
        QString jsonRecords = client.getAttendanceRecords(
            QString::fromStdString(recordsPersonnelId),
            QString::fromStdString(recordsStartDate),
            QString::fromStdString(recordsEndDate)
        );
        if (!jsonRecords.isEmpty()) {
            std::cout << jsonRecords.toStdString() << std::endl;
        } else {
            std::cerr << "Failed to get attendance records" << std::endl;
        }
    });

    // Get attendance record command
    auto* recordCmd = cliApp.add_subcommand("record", "Get attendance record");
    std::string recordId;
    recordCmd->add_option("--id", recordId, "Record ID")->required();
    recordCmd->callback([&]() {
        QString jsonRecord = client.getAttendanceRecord(QString::fromStdString(recordId));
        if (!jsonRecord.isEmpty()) {
            std::cout << jsonRecord.toStdString() << std::endl;
        } else {
            std::cerr << "Record not found" << std::endl;
        }
    });

    // Check personnel presence command
    auto* presentCmd = cliApp.add_subcommand("present", "Check if personnel is present");
    std::string presentPersonnelId;
    presentCmd->add_option("--personnel", presentPersonnelId, "Personnel ID")->required();
    presentCmd->callback([&]() {
        bool present = client.isPersonnelPresent(QString::fromStdString(presentPersonnelId));
        std::cout << "Personnel " << presentPersonnelId << " is " << (present ? "present" : "not present") << std::endl;
    });

    // Create shift command
    auto* shiftCreateCmd = cliApp.add_subcommand("shift-create", "Create shift assignment");
    std::string shiftPersonnelId;
    std::string shiftStartDateTime;
    std::string shiftEndDateTime;
    int shiftType = 0;
    std::string shiftLocation;
    shiftCreateCmd->add_option("--personnel", shiftPersonnelId, "Personnel ID")->required();
    shiftCreateCmd->add_option("--start", shiftStartDateTime, "Start date/time (ISO format)")->required();
    shiftCreateCmd->add_option("--end", shiftEndDateTime, "End date/time (ISO format)")->required();
    shiftCreateCmd->add_option("--type", shiftType, "Shift type (0=Day, 1=Night, 2=Swing, 3=Custom)")->default_val(0);
    shiftCreateCmd->add_option("--location", shiftLocation, "Location/area")->required();
    shiftCreateCmd->callback([&]() {
        QString shiftId = client.createShift(
            QString::fromStdString(shiftPersonnelId),
            QString::fromStdString(shiftStartDateTime),
            QString::fromStdString(shiftEndDateTime),
            shiftType,
            QString::fromStdString(shiftLocation)
        );
        if (!shiftId.isEmpty()) {
            std::cout << "Shift created: " << shiftId.toStdString() << std::endl;
        } else {
            std::cerr << "Failed to create shift" << std::endl;
        }
    });

    // Get shifts command
    auto* shiftsCmd = cliApp.add_subcommand("shifts", "List shift assignments");
    std::string shiftsPersonnelId;
    std::string shiftsStartDate;
    std::string shiftsEndDate;
    shiftsCmd->add_option("--personnel", shiftsPersonnelId, "Personnel ID filter");
    shiftsCmd->add_option("--start", shiftsStartDate, "Start date (ISO format)");
    shiftsCmd->add_option("--end", shiftsEndDate, "End date (ISO format)");
    shiftsCmd->callback([&]() {
        QString jsonShifts = client.getShifts(
            QString::fromStdString(shiftsPersonnelId),
            QString::fromStdString(shiftsStartDate),
            QString::fromStdString(shiftsEndDate)
        );
        if (!jsonShifts.isEmpty()) {
            std::cout << jsonShifts.toStdString() << std::endl;
        } else {
            std::cerr << "Failed to get shifts" << std::endl;
        }
    });

    // Detect conflicts command
    auto* conflictsCmd = cliApp.add_subcommand("conflicts", "Detect scheduling conflicts");
    std::string conflictsStartDate;
    std::string conflictsEndDate;
    conflictsCmd->add_option("--start", conflictsStartDate, "Start date (ISO format)");
    conflictsCmd->add_option("--end", conflictsEndDate, "End date (ISO format)");
    conflictsCmd->callback([&]() {
        QString jsonConflicts = client.detectConflicts(
            QString::fromStdString(conflictsStartDate),
            QString::fromStdString(conflictsEndDate)
        );
        if (!jsonConflicts.isEmpty()) {
            std::cout << jsonConflicts.toStdString() << std::endl;
        } else {
            std::cout << "No conflicts detected" << std::endl;
        }
    });

    // Create leave request command
    auto* leaveCmd = cliApp.add_subcommand("leave", "Create leave request");
    std::string leavePersonnelId;
    std::string leaveStartDate;
    std::string leaveEndDate;
    int leaveType = 0;
    std::string leaveReason;
    leaveCmd->add_option("--personnel", leavePersonnelId, "Personnel ID")->required();
    leaveCmd->add_option("--start", leaveStartDate, "Start date (ISO format)")->required();
    leaveCmd->add_option("--end", leaveEndDate, "End date (ISO format)")->required();
    leaveCmd->add_option("--type", leaveType, "Leave type (0=Vacation, 1=Sick, 2=Personal, 3=Emergency, 4=Other)")->default_val(0);
    leaveCmd->add_option("--reason", leaveReason, "Reason for leave")->required();
    leaveCmd->callback([&]() {
        QString requestId = client.createLeaveRequest(
            QString::fromStdString(leavePersonnelId),
            QString::fromStdString(leaveStartDate),
            QString::fromStdString(leaveEndDate),
            leaveType,
            QString::fromStdString(leaveReason)
        );
        if (!requestId.isEmpty()) {
            std::cout << "Leave request created: " << requestId.toStdString() << std::endl;
        } else {
            std::cerr << "Failed to create leave request" << std::endl;
        }
    });

    // Approve leave request command
    auto* approveLeaveCmd = cliApp.add_subcommand("leave-approve", "Approve leave request");
    std::string approveLeaveRequestId;
    std::string approveLeaveApproverId;
    approveLeaveCmd->add_option("--request", approveLeaveRequestId, "Request ID")->required();
    approveLeaveCmd->add_option("--approver", approveLeaveApproverId, "Approver ID")->required();
    approveLeaveCmd->callback([&]() {
        bool success = client.approveLeaveRequest(
            QString::fromStdString(approveLeaveRequestId),
            QString::fromStdString(approveLeaveApproverId)
        );
        if (success) {
            std::cout << "Leave request approved" << std::endl;
        } else {
            std::cerr << "Failed to approve leave request" << std::endl;
        }
    });

    // Create swap request command
    auto* swapCmd = cliApp.add_subcommand("swap", "Create shift swap request");
    std::string swapShiftId;
    std::string swapRequesterId;
    std::string swapTargetId;
    std::string swapReason;
    swapCmd->add_option("--shift", swapShiftId, "Shift ID")->required();
    swapCmd->add_option("--requester", swapRequesterId, "Requester personnel ID")->required();
    swapCmd->add_option("--target", swapTargetId, "Target personnel ID")->required();
    swapCmd->add_option("--reason", swapReason, "Reason for swap")->required();
    swapCmd->callback([&]() {
        QString swapId = client.createSwapRequest(
            QString::fromStdString(swapShiftId),
            QString::fromStdString(swapRequesterId),
            QString::fromStdString(swapTargetId),
            QString::fromStdString(swapReason)
        );
        if (!swapId.isEmpty()) {
            std::cout << "Swap request created: " << swapId.toStdString() << std::endl;
        } else {
            std::cerr << "Failed to create swap request" << std::endl;
        }
    });

    // Approve swap request command
    auto* approveSwapCmd = cliApp.add_subcommand("swap-approve", "Approve shift swap request");
    std::string approveSwapId;
    std::string approveSwapApproverId;
    approveSwapCmd->add_option("--swap", approveSwapId, "Swap ID")->required();
    approveSwapCmd->add_option("--approver", approveSwapApproverId, "Approver ID")->required();
    approveSwapCmd->callback([&]() {
        bool success = client.approveSwapRequest(
            QString::fromStdString(approveSwapId),
            QString::fromStdString(approveSwapApproverId)
        );
        if (success) {
            std::cout << "Swap request approved" << std::endl;
        } else {
            std::cerr << "Failed to approve swap request" << std::endl;
        }
    });

    // Create coverage request command
    auto* coverageCmd = cliApp.add_subcommand("coverage", "Create coverage request");
    std::string coverageShiftId;
    std::string coverageRequesterId;
    std::string coverageReason;
    coverageCmd->add_option("--shift", coverageShiftId, "Shift ID")->required();
    coverageCmd->add_option("--requester", coverageRequesterId, "Requester personnel ID")->required();
    coverageCmd->add_option("--reason", coverageReason, "Reason for coverage request")->required();
    coverageCmd->callback([&]() {
        QString requestId = client.createCoverageRequest(
            QString::fromStdString(coverageShiftId),
            QString::fromStdString(coverageRequesterId),
            QString::fromStdString(coverageReason)
        );
        if (!requestId.isEmpty()) {
            std::cout << "Coverage request created: " << requestId.toStdString() << std::endl;
        } else {
            std::cerr << "Failed to create coverage request" << std::endl;
        }
    });

    CLI11_PARSE(cliApp, argc, argv);
    return 0;
}

