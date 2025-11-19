#include "dbus_interface.h"
#include "device_manager.h"
#include "device_health.h"
#include "attendance_tracker.h"
#include "shift_scheduler.h"
#include "conflict_detector.h"
#include "leave_manager.h"
#include "shift_swap.h"
#include "coverage_manager.h"
#include "biometric_abstraction.h"
#include <QDBusConnection>
#include <QDBusMetaType>
#include <QDebug>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

PersonnelSchedulerDBusInterface::PersonnelSchedulerDBusInterface(QObject* parent)
    : QObject(parent)
    , m_deviceManager(nullptr)
    , m_healthMonitor(nullptr)
    , m_attendanceTracker(nullptr)
    , m_shiftScheduler(nullptr)
    , m_conflictDetector(nullptr)
    , m_leaveManager(nullptr)
    , m_swapManager(nullptr)
    , m_coverageManager(nullptr)
    , m_initialized(false)
{
}

PersonnelSchedulerDBusInterface::~PersonnelSchedulerDBusInterface() {
}

void PersonnelSchedulerDBusInterface::setDeviceManager(DeviceManager* deviceManager) {
    m_deviceManager = deviceManager;
}

void PersonnelSchedulerDBusInterface::setDeviceHealthMonitor(DeviceHealthMonitor* healthMonitor) {
    m_healthMonitor = healthMonitor;
}

void PersonnelSchedulerDBusInterface::setAttendanceTracker(AttendanceTracker* attendanceTracker) {
    m_attendanceTracker = attendanceTracker;
}

void PersonnelSchedulerDBusInterface::setShiftScheduler(ShiftScheduler* shiftScheduler) {
    m_shiftScheduler = shiftScheduler;
}

void PersonnelSchedulerDBusInterface::setConflictDetector(ConflictDetector* conflictDetector) {
    m_conflictDetector = conflictDetector;
}

void PersonnelSchedulerDBusInterface::setLeaveManager(LeaveManager* leaveManager) {
    m_leaveManager = leaveManager;
}

void PersonnelSchedulerDBusInterface::setShiftSwapManager(ShiftSwapManager* swapManager) {
    m_swapManager = swapManager;
}

void PersonnelSchedulerDBusInterface::setCoverageManager(CoverageManager* coverageManager) {
    m_coverageManager = coverageManager;
}

bool PersonnelSchedulerDBusInterface::initialize() {
    if (m_initialized) {
        return true;
    }
    
    if (!m_deviceManager) {
        qWarning() << "Device manager not set";
        return false;
    }
    
    QDBusConnection connection = QDBusConnection::sessionBus();
    if (!connection.isConnected()) {
        qWarning() << "Cannot connect to D-Bus session bus";
        return false;
    }
    
    if (!connection.registerObject("/org/milos/PersonnelScheduler", this)) {
        qWarning() << "Failed to register D-Bus object";
        return false;
    }
    
    if (!connection.registerService("org.milos.PersonnelScheduler")) {
        qWarning() << "Failed to register D-Bus service";
        return false;
    }
    
    m_initialized = true;
    return true;
}

QStringList PersonnelSchedulerDBusInterface::DetectDevices(const QString& deviceType) {
    if (!m_deviceManager) {
        return QStringList();
    }
    
    BiometricDeviceType type;
    if (deviceType == "face") {
        type = BiometricDeviceType::FaceRecognition;
    } else if (deviceType == "fingerprint") {
        type = BiometricDeviceType::Fingerprint;
    } else {
        return QStringList();
    }
    
    QList<QString> devices = m_deviceManager->detectDevices(type);
    return QStringList(devices);
}

bool PersonnelSchedulerDBusInterface::RegisterDevice(const QString& deviceId, const QString& deviceType) {
    if (!m_deviceManager) {
        return false;
    }
    
    BiometricDeviceType type;
    if (deviceType == "face") {
        type = BiometricDeviceType::FaceRecognition;
    } else if (deviceType == "fingerprint") {
        type = BiometricDeviceType::Fingerprint;
    } else {
        return false;
    }
    
    std::unique_ptr<IBiometricDevice> device;
    if (type == BiometricDeviceType::FaceRecognition) {
        device = BiometricDeviceFactory::createFaceRecognitionDevice(deviceId);
    } else {
        device = BiometricDeviceFactory::createFingerprintDevice(deviceId);
    }
    
    if (!device) {
        return false;
    }
    
    if (!device->initialize()) {
        return false;
    }
    
    return m_deviceManager->registerDevice(std::move(device));
}

bool PersonnelSchedulerDBusInterface::UnregisterDevice(const QString& deviceId) {
    if (!m_deviceManager) {
        return false;
    }
    
    m_deviceManager->unregisterDevice(deviceId);
    return true;
}

QStringList PersonnelSchedulerDBusInterface::GetRegisteredDevices() {
    if (!m_deviceManager) {
        return QStringList();
    }
    
    QList<QString> devices = m_deviceManager->getRegisteredDevices();
    return QStringList(devices);
}

QVariantMap PersonnelSchedulerDBusInterface::GetDeviceInfo(const QString& deviceId) {
    QVariantMap info;
    
    if (!m_deviceManager) {
        return info;
    }
    
    IBiometricDevice* device = m_deviceManager->getDevice(deviceId);
    if (!device) {
        return info;
    }
    
    info["device_id"] = device->deviceId();
    info["device_type"] = (device->deviceType() == BiometricDeviceType::FaceRecognition) ? "face" : "fingerprint";
    info["vendor"] = device->vendor();
    info["model"] = device->model();
    info["connected"] = device->isConnected();
    info["configuration"] = device->getConfiguration();
    
    return info;
}

QVariantMap PersonnelSchedulerDBusInterface::GetDeviceHealth(const QString& deviceId) {
    QVariantMap healthMap;
    
    if (!m_healthMonitor) {
        return healthMap;
    }
    
    DeviceHealth health = m_healthMonitor->getHealth(deviceId);
    
    QString statusStr;
    switch (health.status) {
        case DeviceStatus::Connected:
            statusStr = "connected";
            break;
        case DeviceStatus::Disconnected:
            statusStr = "disconnected";
            break;
        case DeviceStatus::Error:
            statusStr = "error";
            break;
        case DeviceStatus::Initializing:
            statusStr = "initializing";
            break;
    }
    
    healthMap["status"] = statusStr;
    healthMap["health_score"] = health.healthScore;
    healthMap["error_message"] = health.errorMessage;
    healthMap["metrics"] = health.metrics;
    
    return healthMap;
}

QString PersonnelSchedulerDBusInterface::CaptureBiometric(const QString& deviceId, int timeoutMs) {
    if (!m_deviceManager) {
        return QString();
    }
    
    IBiometricDevice* device = m_deviceManager->getDevice(deviceId);
    if (!device) {
        return QString();
    }
    
    QByteArray data = device->capture(timeoutMs);
    return data.toBase64();
}

int PersonnelSchedulerDBusInterface::VerifyBiometric(const QString& deviceId, const QString& templateData, const QString& capturedData) {
    if (!m_deviceManager) {
        return 0;
    }
    
    IBiometricDevice* device = m_deviceManager->getDevice(deviceId);
    if (!device) {
        return 0;
    }
    
    QByteArray templateBytes = QByteArray::fromBase64(templateData.toUtf8());
    QByteArray capturedBytes = QByteArray::fromBase64(capturedData.toUtf8());
    
    return device->verify(templateBytes, capturedBytes);
}

QString PersonnelSchedulerDBusInterface::EnrollBiometric(const QString& deviceId, const QString& capturedData) {
    if (!m_deviceManager) {
        return QString();
    }
    
    IBiometricDevice* device = m_deviceManager->getDevice(deviceId);
    if (!device) {
        return QString();
    }
    
    QByteArray capturedBytes = QByteArray::fromBase64(capturedData.toUtf8());
    QByteArray templateData = device->enroll(capturedBytes);
    
    return templateData.toBase64();
}

QVariantMap PersonnelSchedulerDBusInterface::GetDeviceConfiguration(const QString& deviceId) {
    if (!m_deviceManager) {
        return QVariantMap();
    }
    
    IBiometricDevice* device = m_deviceManager->getDevice(deviceId);
    if (!device) {
        return QVariantMap();
    }
    
    return device->getConfiguration();
}

bool PersonnelSchedulerDBusInterface::SetDeviceConfiguration(const QString& deviceId, const QVariantMap& config) {
    if (!m_deviceManager) {
        return false;
    }
    
    IBiometricDevice* device = m_deviceManager->getDevice(deviceId);
    if (!device) {
        return false;
    }
    
    return device->setConfiguration(config);
}

QString PersonnelSchedulerDBusInterface::RecordAttendanceEntry(const QString& deviceId, const QString& personnelId, const QString& location) {
    if (!m_attendanceTracker) {
        return QString();
    }
    
    return m_attendanceTracker->recordEntry(deviceId, personnelId, location);
}

QString PersonnelSchedulerDBusInterface::RecordAttendanceExit(const QString& deviceId, const QString& personnelId, const QString& location) {
    if (!m_attendanceTracker) {
        return QString();
    }
    
    return m_attendanceTracker->recordExit(deviceId, personnelId, location);
}

QString PersonnelSchedulerDBusInterface::GetAttendanceRecords(const QString& personnelId, const QString& startDate, const QString& endDate) {
    if (!m_attendanceTracker) {
        return QString();
    }
    
    QDateTime startDateTime = startDate.isEmpty() ? QDateTime() : QDateTime::fromString(startDate, Qt::ISODate);
    QDateTime endDateTime = endDate.isEmpty() ? QDateTime() : QDateTime::fromString(endDate, Qt::ISODate);
    
    QList<AttendanceRecord> records = m_attendanceTracker->getAttendanceRecords(personnelId, startDateTime, endDateTime);
    
    QJsonArray jsonArray;
    for (const AttendanceRecord& record : records) {
        QJsonObject obj;
        obj["record_id"] = record.recordId;
        obj["personnel_id"] = record.personnelId;
        obj["device_id"] = record.deviceId;
        obj["device_type"] = static_cast<int>(record.deviceType);
        obj["event_type"] = static_cast<int>(record.eventType);
        obj["timestamp"] = record.timestamp.toString(Qt::ISODate);
        obj["location"] = record.location;
        obj["match_score"] = record.matchScore;
        obj["validated"] = record.validated;
        jsonArray.append(obj);
    }
    
    QJsonDocument doc(jsonArray);
    return QString::fromUtf8(doc.toJson());
}

QString PersonnelSchedulerDBusInterface::GetAttendanceRecord(const QString& recordId) {
    if (!m_attendanceTracker) {
        return QString();
    }
    
    AttendanceRecord record = m_attendanceTracker->getAttendanceRecord(recordId);
    if (record.recordId.isEmpty()) {
        return QString();
    }
    
    QJsonObject obj;
    obj["record_id"] = record.recordId;
    obj["personnel_id"] = record.personnelId;
    obj["device_id"] = record.deviceId;
    obj["device_type"] = static_cast<int>(record.deviceType);
    obj["event_type"] = static_cast<int>(record.eventType);
    obj["timestamp"] = record.timestamp.toString(Qt::ISODate);
    obj["location"] = record.location;
    obj["match_score"] = record.matchScore;
    obj["validated"] = record.validated;
    
    QJsonDocument doc(obj);
    return QString::fromUtf8(doc.toJson());
}

bool PersonnelSchedulerDBusInterface::IsPersonnelPresent(const QString& personnelId) {
    if (!m_attendanceTracker) {
        return false;
    }
    
    return m_attendanceTracker->isPersonnelPresent(personnelId);
}

QString PersonnelSchedulerDBusInterface::CreateShift(const QString& personnelId, const QString& startDateTime, const QString& endDateTime, int shiftType, const QString& location) {
    if (!m_shiftScheduler) {
        return QString();
    }
    
    QDateTime start = QDateTime::fromString(startDateTime, Qt::ISODate);
    QDateTime end = QDateTime::fromString(endDateTime, Qt::ISODate);
    
    if (!start.isValid() || !end.isValid()) {
        return QString();
    }
    
    return m_shiftScheduler->createShift(personnelId, start, end, static_cast<ShiftType>(shiftType), location);
}

QString PersonnelSchedulerDBusInterface::GetShifts(const QString& personnelId, const QString& startDate, const QString& endDate) {
    if (!m_shiftScheduler) {
        return QString();
    }
    
    QDateTime startDateTime = startDate.isEmpty() ? QDateTime() : QDateTime::fromString(startDate, Qt::ISODate);
    QDateTime endDateTime = endDate.isEmpty() ? QDateTime() : QDateTime::fromString(endDate, Qt::ISODate);
    
    QList<ShiftAssignment> shifts = m_shiftScheduler->getShifts(personnelId, startDateTime, endDateTime);
    
    QJsonArray jsonArray;
    for (const ShiftAssignment& shift : shifts) {
        QJsonObject obj;
        obj["shift_id"] = shift.shiftId;
        obj["personnel_id"] = shift.personnelId;
        obj["start_date_time"] = shift.startDateTime.toString(Qt::ISODate);
        obj["end_date_time"] = shift.endDateTime.toString(Qt::ISODate);
        obj["shift_type"] = static_cast<int>(shift.shiftType);
        obj["location"] = shift.location;
        obj["status"] = static_cast<int>(shift.status);
        obj["notes"] = shift.notes;
        jsonArray.append(obj);
    }
    
    QJsonDocument doc(jsonArray);
    return QString::fromUtf8(doc.toJson());
}

QString PersonnelSchedulerDBusInterface::DetectConflicts(const QString& startDate, const QString& endDate) {
    if (!m_shiftScheduler || !m_conflictDetector) {
        return QString();
    }
    
    QDateTime startDateTime = startDate.isEmpty() ? QDateTime() : QDateTime::fromString(startDate, Qt::ISODate);
    QDateTime endDateTime = endDate.isEmpty() ? QDateTime() : QDateTime::fromString(endDate, Qt::ISODate);
    
    QList<ShiftAssignment> shifts = m_shiftScheduler->getShifts(QString(), startDateTime, endDateTime);
    QList<Conflict> conflicts = m_conflictDetector->detectConflicts(shifts);
    
    QJsonArray jsonArray;
    for (const Conflict& conflict : conflicts) {
        QJsonObject obj;
        obj["type"] = static_cast<int>(conflict.type);
        obj["shift_id1"] = conflict.shiftId1;
        obj["shift_id2"] = conflict.shiftId2;
        obj["personnel_id"] = conflict.personnelId;
        obj["description"] = conflict.description;
        obj["conflict_time"] = conflict.conflictTime.toString(Qt::ISODate);
        jsonArray.append(obj);
    }
    
    QJsonDocument doc(jsonArray);
    return QString::fromUtf8(doc.toJson());
}

QString PersonnelSchedulerDBusInterface::CreateLeaveRequest(const QString& personnelId, const QString& startDate, const QString& endDate, int leaveType, const QString& reason) {
    if (!m_leaveManager) {
        return QString();
    }
    
    QDateTime startDateTime = QDateTime::fromString(startDate, Qt::ISODate);
    QDateTime endDateTime = QDateTime::fromString(endDate, Qt::ISODate);
    
    if (!startDateTime.isValid() || !endDateTime.isValid()) {
        return QString();
    }
    
    return m_leaveManager->createLeaveRequest(personnelId, startDateTime, endDateTime, static_cast<LeaveType>(leaveType), reason);
}

bool PersonnelSchedulerDBusInterface::ApproveLeaveRequest(const QString& requestId, const QString& approverId) {
    if (!m_leaveManager) {
        return false;
    }
    
    return m_leaveManager->approveLeaveRequest(requestId, approverId);
}

QString PersonnelSchedulerDBusInterface::CreateSwapRequest(const QString& shiftId, const QString& requesterId, const QString& targetPersonnelId, const QString& reason) {
    if (!m_swapManager) {
        return QString();
    }
    
    return m_swapManager->createSwapRequest(shiftId, requesterId, targetPersonnelId, reason);
}

bool PersonnelSchedulerDBusInterface::ApproveSwapRequest(const QString& swapId, const QString& approverId) {
    if (!m_swapManager) {
        return false;
    }
    
    return m_swapManager->approveSwapRequest(swapId, approverId);
}

QString PersonnelSchedulerDBusInterface::CreateCoverageRequest(const QString& shiftId, const QString& requesterId, const QString& reason) {
    if (!m_coverageManager) {
        return QString();
    }
    
    return m_coverageManager->createCoverageRequest(shiftId, requesterId, reason);
}

