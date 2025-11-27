#include "milos/ui/crash_handler.h"
#include "milos/logging/logger.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QSysInfo>
#include <QCoreApplication>
#include <csignal>
#include <cstdlib>
#include <execinfo.h>
#include <unistd.h>
#include <dlfcn.h>
#include <link.h>

CrashHandler* CrashHandler::s_instance = nullptr;

CrashHandler::CrashHandler(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_crashDetected(false)
{
}

CrashHandler::~CrashHandler() {
}

CrashHandler* CrashHandler::instance() {
    if (!s_instance) {
        s_instance = new CrashHandler();
    }
    return s_instance;
}

bool CrashHandler::initialize(const QString& applicationName, const QString& applicationVersion) {
    if (m_initialized) {
        return true;
    }

    m_applicationName = applicationName;
    m_applicationVersion = applicationVersion;

    // Initialize logger - use default initialization
    if (!Logger::instance()->isInitialized()) {
        Logger::instance()->initialize(applicationName + "-CrashHandler");
    }

    // Create crash reports directory
    m_crashReportsDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/crash-reports";
    QDir().mkpath(m_crashReportsDir);

    // Collect system information
    collectSystemInfo();

    // Install crash handlers
    if (!installCrashHandlers()) {
        LOG_WARNING("Failed to install crash handlers");
        return false;
    }

    m_initialized = true;
    LOG_INFO("Crash handler initialized");
    return true;
}

bool CrashHandler::installCrashHandlers() {
    // Install signal handlers for common crash signals
    signal(SIGSEGV, CrashHandler::signalHandler);  // Segmentation fault
    signal(SIGABRT, CrashHandler::signalHandler);   // Abort
    signal(SIGFPE, CrashHandler::signalHandler);    // Floating point exception
    signal(SIGILL, CrashHandler::signalHandler);    // Illegal instruction
    signal(SIGBUS, CrashHandler::signalHandler);    // Bus error

    // Install terminate handler for uncaught exceptions
    std::set_terminate(CrashHandler::terminateHandler);

    return true;
}

void CrashHandler::signalHandler(int signal) {
    if (s_instance) {
        s_instance->handleCrash(signal);
    }
    
    // Restore default handler and re-raise signal
    signal(signal, SIG_DFL);
    raise(signal);
}

void CrashHandler::terminateHandler() {
    if (s_instance) {
        s_instance->handleException("Uncaught C++ exception");
    }
    
    std::abort();
}

void CrashHandler::handleCrash(int signal, void* context) {
    if (m_crashDetected) {
        // Already handling a crash, avoid recursion
        return;
    }

    m_crashDetected = true;
    emit crashDetectedChanged();

    CrashInfo crashInfo;
    crashInfo.crashId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    crashInfo.timestamp = QDateTime::currentDateTime();
    crashInfo.applicationName = m_applicationName;
    crashInfo.applicationVersion = m_applicationVersion;

    // Map signal number to signal name
    switch (signal) {
        case SIGSEGV: crashInfo.signal = "SIGSEGV"; break;
        case SIGABRT: crashInfo.signal = "SIGABRT"; break;
        case SIGFPE: crashInfo.signal = "SIGFPE"; break;
        case SIGILL: crashInfo.signal = "SIGILL"; break;
        case SIGBUS: crashInfo.signal = "SIGBUS"; break;
        default: crashInfo.signal = QString("SIG%1").arg(signal); break;
    }

    crashInfo.errorMessage = QString("Application crashed with signal: %1").arg(crashInfo.signal);
    crashInfo.stackTrace = generateStackTrace(context);
    crashInfo.loadedLibraries = getLoadedLibraries();
    crashInfo.systemInfo = m_systemInfo;
    crashInfo.applicationState = m_applicationState;
    crashInfo.canRecover = canRecoverFromCrash(crashInfo);

    // Save crash report
    QString reportPath = saveCrashReport(crashInfo);
    crashInfo.logFile = reportPath;
    m_lastCrashReport = reportPath;

    LOG_CRITICAL(QString("Application crashed: %1").arg(crashInfo.errorMessage));

    emit crashReportReady(crashInfo);
    emit crashReportChanged();
}

void CrashHandler::handleException(const QString& exception) {
    if (m_crashDetected) {
        return;
    }

    m_crashDetected = true;
    emit crashDetectedChanged();

    CrashInfo crashInfo;
    crashInfo.crashId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    crashInfo.timestamp = QDateTime::currentDateTime();
    crashInfo.applicationName = m_applicationName;
    crashInfo.applicationVersion = m_applicationVersion;
    crashInfo.signal = "EXCEPTION";
    crashInfo.errorMessage = exception;
    crashInfo.stackTrace = generateStackTrace();
    crashInfo.loadedLibraries = getLoadedLibraries();
    crashInfo.systemInfo = m_systemInfo;
    crashInfo.applicationState = m_applicationState;
    crashInfo.canRecover = canRecoverFromCrash(crashInfo);

    QString reportPath = saveCrashReport(crashInfo);
    crashInfo.logFile = reportPath;
    m_lastCrashReport = reportPath;

    LOG_CRITICAL(QString("Uncaught exception: %1").arg(exception));

    emit crashReportReady(crashInfo);
    emit crashReportChanged();
}

QString CrashHandler::generateCrashReport(const CrashInfo& crashInfo) const {
    QJsonObject report;

    // Basic information
    report["crash_id"] = crashInfo.crashId;
    report["timestamp"] = crashInfo.timestamp.toString(Qt::ISODate);
    report["application_name"] = crashInfo.applicationName;
    report["application_version"] = crashInfo.applicationVersion;
    report["signal"] = crashInfo.signal;
    report["error_message"] = crashInfo.errorMessage;

    // Stack trace
    report["stack_trace"] = crashInfo.stackTrace;

    // System information
    report["system_info"] = QJsonObject::fromVariantMap(crashInfo.systemInfo);

    // Application state
    report["application_state"] = QJsonObject::fromVariantMap(crashInfo.applicationState);

    // Loaded libraries
    QJsonArray libraries;
    for (const QString& lib : crashInfo.loadedLibraries) {
        libraries.append(lib);
    }
    report["loaded_libraries"] = libraries;

    // Recovery information
    report["can_recover"] = crashInfo.canRecover;

    QJsonDocument doc(report);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
}

QString CrashHandler::saveCrashReport(const CrashInfo& crashInfo) {
    QString reportJson = generateCrashReport(crashInfo);
    QString sanitized = sanitizeCrashReport(reportJson);

    QString filename = QString("crash-%1-%2.json")
                      .arg(crashInfo.crashId)
                      .arg(crashInfo.timestamp.toString("yyyyMMdd-HHmmss"));
    QString filepath = m_crashReportsDir + "/" + filename;

    QFile file(filepath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << sanitized;
        file.close();
        
        LOG_INFO(QString("Crash report saved: %1").arg(filepath));
        return filepath;
    } else {
        LOG_ERROR(QString("Failed to save crash report: %1").arg(file.errorString()));
        return QString();
    }
}

bool CrashHandler::submitCrashReport(const QString& crashReportPath, bool includeSystemInfo) {
    QFile file(crashReportPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR(QString("Failed to read crash report: %1").arg(crashReportPath));
        return false;
    }

    QTextStream stream(&file);
    QString reportJson = stream.readAll();
    file.close();

    // Extract crash ID from report
    QJsonDocument doc = QJsonDocument::fromJson(reportJson.toUtf8());
    if (doc.isNull() || !doc.isObject()) {
        LOG_ERROR("Invalid crash report format");
        return false;
    }

    QJsonObject report = doc.object();
    QString crashId = report.value("crash_id").toString();

    // In production, would submit to bug tracking system
    // For now, log and save locally
    LOG_INFO(QString("Crash report ready for submission: %1").arg(crashId));

    // TODO: Implement actual submission to bug tracking system
    // - GitHub Issues API
    // - Jira API
    // - Custom bug tracking system
    // - Email to support

    emit crashReportSubmitted(crashId, true);
    return true;
}

QString CrashHandler::getCrashReportsDirectory() const {
    return m_crashReportsDir;
}

QStringList CrashHandler::getCrashReports() const {
    QStringList reports;
    QDir dir(m_crashReportsDir);
    
    QStringList filters;
    filters << "crash-*.json";
    
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files, QDir::Time | QDir::Reversed);
    for (const QFileInfo& fileInfo : files) {
        reports.append(fileInfo.fileName());
    }
    
    return reports;
}

bool CrashHandler::deleteCrashReport(const QString& crashReportId) {
    QStringList reports = getCrashReports();
    for (const QString& report : reports) {
        if (report.contains(crashReportId)) {
            QString filepath = m_crashReportsDir + "/" + report;
            if (QFile::remove(filepath)) {
                LOG_INFO(QString("Deleted crash report: %1").arg(report));
                return true;
            }
        }
    }
    return false;
}

void CrashHandler::collectSystemInfo() {
    m_systemInfo["os_name"] = QSysInfo::prettyProductName();
    m_systemInfo["os_version"] = QSysInfo::productVersion();
    m_systemInfo["kernel_version"] = QSysInfo::kernelVersion();
    m_systemInfo["architecture"] = QSysInfo::currentCpuArchitecture();
    m_systemInfo["machine_hostname"] = QSysInfo::machineHostName();
    m_systemInfo["qt_version"] = QString::fromLatin1(QT_VERSION_STR);
    m_systemInfo["qt_runtime_version"] = qVersion();

    // Get memory information
    QFile meminfo("/proc/meminfo");
    if (meminfo.open(QIODevice::ReadOnly)) {
        QTextStream stream(&meminfo);
        QString content = stream.readAll();
        QRegularExpression regex(R"(MemTotal:\s+(\d+)\s+kB)");
        QRegularExpressionMatch match = regex.match(content);
        if (match.hasMatch()) {
            qint64 memKB = match.captured(1).toLongLong();
            m_systemInfo["total_memory_mb"] = memKB / 1024;
        }
        meminfo.close();
    }

    // Get CPU information
    QFile cpuinfo("/proc/cpuinfo");
    if (cpuinfo.open(QIODevice::ReadOnly)) {
        QTextStream stream(&cpuinfo);
        QString content = stream.readAll();
        QRegularExpression regex(R"(model name\s+:\s+(.+))");
        QRegularExpressionMatch match = regex.match(content);
        if (match.hasMatch()) {
            m_systemInfo["cpu_model"] = match.captured(1).trimmed();
        }
        int cpuCount = content.count("processor");
        m_systemInfo["cpu_count"] = cpuCount;
        cpuinfo.close();
    }
}

void CrashHandler::collectApplicationState() {
    m_applicationState["application_name"] = m_applicationName;
    m_applicationState["application_version"] = m_applicationVersion;
    m_applicationState["qt_version"] = QString::fromLatin1(QT_VERSION_STR);
    m_applicationState["process_id"] = static_cast<qint64>(QCoreApplication::applicationPid());
}

QString CrashHandler::generateStackTrace(void* context) {
    QString stackTrace;
    
    // Use backtrace for stack trace
    void* array[50];
    size_t size = backtrace(array, 50);
    char** symbols = backtrace_symbols(array, size);
    
    if (symbols) {
        for (size_t i = 0; i < size; ++i) {
            stackTrace += QString("#%1 %2\n").arg(i).arg(symbols[i]);
        }
        free(symbols);
    }
    
    return stackTrace;
}

QStringList CrashHandler::getLoadedLibraries() {
    QStringList libraries;
    
    // Use dl_iterate_phdr to get loaded libraries
    struct LibraryInfo {
        QStringList* libs;
    } info;
    info.libs = &libraries;
    
    dl_iterate_phdr([](struct dl_phdr_info* info, size_t, void* data) -> int {
        LibraryInfo* libInfo = static_cast<LibraryInfo*>(data);
        if (info->dlpi_name && strlen(info->dlpi_name) > 0) {
            libInfo->libs->append(QString::fromLocal8Bit(info->dlpi_name));
        }
        return 0;
    }, &info);
    
    return libraries;
}

QString CrashHandler::sanitizeCrashReport(const QString& report) const {
    // Remove sensitive information from crash report
    QString sanitized = report;
    
    // Remove potential passwords/tokens (basic pattern matching)
    QRegularExpression passwordRegex(R"((password|token|key|secret)\s*[:=]\s*["']?[^"'\s]+["']?)", QRegularExpression::CaseInsensitiveOption);
    sanitized.replace(passwordRegex, "\\1: [REDACTED]");
    
    // Remove file paths that might contain user information
    QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    sanitized.replace(homePath, "[HOME]");
    
    return sanitized;
}

bool CrashHandler::canRecoverFromCrash(const CrashInfo& crashInfo) const {
    // Determine if crash is recoverable
    // Some crashes (like memory corruption) are not recoverable
    // Others (like null pointer dereference in non-critical code) might be
    
    // For now, assume crashes are not recoverable
    // In production, would analyze crash type and context
    return false;
}


#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QSysInfo>
#include <QCoreApplication>
#include <csignal>
#include <cstdlib>
#include <execinfo.h>
#include <unistd.h>
#include <dlfcn.h>
#include <link.h>

CrashHandler* CrashHandler::s_instance = nullptr;

CrashHandler::CrashHandler(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_crashDetected(false)
{
}

CrashHandler::~CrashHandler() {
}

CrashHandler* CrashHandler::instance() {
    if (!s_instance) {
        s_instance = new CrashHandler();
    }
    return s_instance;
}

bool CrashHandler::initialize(const QString& applicationName, const QString& applicationVersion) {
    if (m_initialized) {
        return true;
    }

    m_applicationName = applicationName;
    m_applicationVersion = applicationVersion;

    // Initialize logger - use default initialization
    if (!Logger::instance()->isInitialized()) {
        Logger::instance()->initialize(applicationName + "-CrashHandler");
    }

    // Create crash reports directory
    m_crashReportsDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/crash-reports";
    QDir().mkpath(m_crashReportsDir);

    // Collect system information
    collectSystemInfo();

    // Install crash handlers
    if (!installCrashHandlers()) {
        LOG_WARNING("Failed to install crash handlers");
        return false;
    }

    m_initialized = true;
    LOG_INFO("Crash handler initialized");
    return true;
}

bool CrashHandler::installCrashHandlers() {
    // Install signal handlers for common crash signals
    signal(SIGSEGV, CrashHandler::signalHandler);  // Segmentation fault
    signal(SIGABRT, CrashHandler::signalHandler);   // Abort
    signal(SIGFPE, CrashHandler::signalHandler);    // Floating point exception
    signal(SIGILL, CrashHandler::signalHandler);    // Illegal instruction
    signal(SIGBUS, CrashHandler::signalHandler);    // Bus error

    // Install terminate handler for uncaught exceptions
    std::set_terminate(CrashHandler::terminateHandler);

    return true;
}

void CrashHandler::signalHandler(int signal) {
    if (s_instance) {
        s_instance->handleCrash(signal);
    }
    
    // Restore default handler and re-raise signal
    signal(signal, SIG_DFL);
    raise(signal);
}

void CrashHandler::terminateHandler() {
    if (s_instance) {
        s_instance->handleException("Uncaught C++ exception");
    }
    
    std::abort();
}

void CrashHandler::handleCrash(int signal, void* context) {
    if (m_crashDetected) {
        // Already handling a crash, avoid recursion
        return;
    }

    m_crashDetected = true;
    emit crashDetectedChanged();

    CrashInfo crashInfo;
    crashInfo.crashId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    crashInfo.timestamp = QDateTime::currentDateTime();
    crashInfo.applicationName = m_applicationName;
    crashInfo.applicationVersion = m_applicationVersion;

    // Map signal number to signal name
    switch (signal) {
        case SIGSEGV: crashInfo.signal = "SIGSEGV"; break;
        case SIGABRT: crashInfo.signal = "SIGABRT"; break;
        case SIGFPE: crashInfo.signal = "SIGFPE"; break;
        case SIGILL: crashInfo.signal = "SIGILL"; break;
        case SIGBUS: crashInfo.signal = "SIGBUS"; break;
        default: crashInfo.signal = QString("SIG%1").arg(signal); break;
    }

    crashInfo.errorMessage = QString("Application crashed with signal: %1").arg(crashInfo.signal);
    crashInfo.stackTrace = generateStackTrace(context);
    crashInfo.loadedLibraries = getLoadedLibraries();
    crashInfo.systemInfo = m_systemInfo;
    crashInfo.applicationState = m_applicationState;
    crashInfo.canRecover = canRecoverFromCrash(crashInfo);

    // Save crash report
    QString reportPath = saveCrashReport(crashInfo);
    crashInfo.logFile = reportPath;
    m_lastCrashReport = reportPath;

    LOG_CRITICAL(QString("Application crashed: %1").arg(crashInfo.errorMessage));

    emit crashReportReady(crashInfo);
    emit crashReportChanged();
}

void CrashHandler::handleException(const QString& exception) {
    if (m_crashDetected) {
        return;
    }

    m_crashDetected = true;
    emit crashDetectedChanged();

    CrashInfo crashInfo;
    crashInfo.crashId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    crashInfo.timestamp = QDateTime::currentDateTime();
    crashInfo.applicationName = m_applicationName;
    crashInfo.applicationVersion = m_applicationVersion;
    crashInfo.signal = "EXCEPTION";
    crashInfo.errorMessage = exception;
    crashInfo.stackTrace = generateStackTrace();
    crashInfo.loadedLibraries = getLoadedLibraries();
    crashInfo.systemInfo = m_systemInfo;
    crashInfo.applicationState = m_applicationState;
    crashInfo.canRecover = canRecoverFromCrash(crashInfo);

    QString reportPath = saveCrashReport(crashInfo);
    crashInfo.logFile = reportPath;
    m_lastCrashReport = reportPath;

    LOG_CRITICAL(QString("Uncaught exception: %1").arg(exception));

    emit crashReportReady(crashInfo);
    emit crashReportChanged();
}

QString CrashHandler::generateCrashReport(const CrashInfo& crashInfo) const {
    QJsonObject report;

    // Basic information
    report["crash_id"] = crashInfo.crashId;
    report["timestamp"] = crashInfo.timestamp.toString(Qt::ISODate);
    report["application_name"] = crashInfo.applicationName;
    report["application_version"] = crashInfo.applicationVersion;
    report["signal"] = crashInfo.signal;
    report["error_message"] = crashInfo.errorMessage;

    // Stack trace
    report["stack_trace"] = crashInfo.stackTrace;

    // System information
    report["system_info"] = QJsonObject::fromVariantMap(crashInfo.systemInfo);

    // Application state
    report["application_state"] = QJsonObject::fromVariantMap(crashInfo.applicationState);

    // Loaded libraries
    QJsonArray libraries;
    for (const QString& lib : crashInfo.loadedLibraries) {
        libraries.append(lib);
    }
    report["loaded_libraries"] = libraries;

    // Recovery information
    report["can_recover"] = crashInfo.canRecover;

    QJsonDocument doc(report);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
}

QString CrashHandler::saveCrashReport(const CrashInfo& crashInfo) {
    QString reportJson = generateCrashReport(crashInfo);
    QString sanitized = sanitizeCrashReport(reportJson);

    QString filename = QString("crash-%1-%2.json")
                      .arg(crashInfo.crashId)
                      .arg(crashInfo.timestamp.toString("yyyyMMdd-HHmmss"));
    QString filepath = m_crashReportsDir + "/" + filename;

    QFile file(filepath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << sanitized;
        file.close();
        
        LOG_INFO(QString("Crash report saved: %1").arg(filepath));
        return filepath;
    } else {
        LOG_ERROR(QString("Failed to save crash report: %1").arg(file.errorString()));
        return QString();
    }
}

bool CrashHandler::submitCrashReport(const QString& crashReportPath, bool includeSystemInfo) {
    QFile file(crashReportPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR(QString("Failed to read crash report: %1").arg(crashReportPath));
        return false;
    }

    QTextStream stream(&file);
    QString reportJson = stream.readAll();
    file.close();

    // Extract crash ID from report
    QJsonDocument doc = QJsonDocument::fromJson(reportJson.toUtf8());
    if (doc.isNull() || !doc.isObject()) {
        LOG_ERROR("Invalid crash report format");
        return false;
    }

    QJsonObject report = doc.object();
    QString crashId = report.value("crash_id").toString();

    // In production, would submit to bug tracking system
    // For now, log and save locally
    LOG_INFO(QString("Crash report ready for submission: %1").arg(crashId));

    // TODO: Implement actual submission to bug tracking system
    // - GitHub Issues API
    // - Jira API
    // - Custom bug tracking system
    // - Email to support

    emit crashReportSubmitted(crashId, true);
    return true;
}

QString CrashHandler::getCrashReportsDirectory() const {
    return m_crashReportsDir;
}

QStringList CrashHandler::getCrashReports() const {
    QStringList reports;
    QDir dir(m_crashReportsDir);
    
    QStringList filters;
    filters << "crash-*.json";
    
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files, QDir::Time | QDir::Reversed);
    for (const QFileInfo& fileInfo : files) {
        reports.append(fileInfo.fileName());
    }
    
    return reports;
}

bool CrashHandler::deleteCrashReport(const QString& crashReportId) {
    QStringList reports = getCrashReports();
    for (const QString& report : reports) {
        if (report.contains(crashReportId)) {
            QString filepath = m_crashReportsDir + "/" + report;
            if (QFile::remove(filepath)) {
                LOG_INFO(QString("Deleted crash report: %1").arg(report));
                return true;
            }
        }
    }
    return false;
}

void CrashHandler::collectSystemInfo() {
    m_systemInfo["os_name"] = QSysInfo::prettyProductName();
    m_systemInfo["os_version"] = QSysInfo::productVersion();
    m_systemInfo["kernel_version"] = QSysInfo::kernelVersion();
    m_systemInfo["architecture"] = QSysInfo::currentCpuArchitecture();
    m_systemInfo["machine_hostname"] = QSysInfo::machineHostName();
    m_systemInfo["qt_version"] = QString::fromLatin1(QT_VERSION_STR);
    m_systemInfo["qt_runtime_version"] = qVersion();

    // Get memory information
    QFile meminfo("/proc/meminfo");
    if (meminfo.open(QIODevice::ReadOnly)) {
        QTextStream stream(&meminfo);
        QString content = stream.readAll();
        QRegularExpression regex(R"(MemTotal:\s+(\d+)\s+kB)");
        QRegularExpressionMatch match = regex.match(content);
        if (match.hasMatch()) {
            qint64 memKB = match.captured(1).toLongLong();
            m_systemInfo["total_memory_mb"] = memKB / 1024;
        }
        meminfo.close();
    }

    // Get CPU information
    QFile cpuinfo("/proc/cpuinfo");
    if (cpuinfo.open(QIODevice::ReadOnly)) {
        QTextStream stream(&cpuinfo);
        QString content = stream.readAll();
        QRegularExpression regex(R"(model name\s+:\s+(.+))");
        QRegularExpressionMatch match = regex.match(content);
        if (match.hasMatch()) {
            m_systemInfo["cpu_model"] = match.captured(1).trimmed();
        }
        int cpuCount = content.count("processor");
        m_systemInfo["cpu_count"] = cpuCount;
        cpuinfo.close();
    }
}

void CrashHandler::collectApplicationState() {
    m_applicationState["application_name"] = m_applicationName;
    m_applicationState["application_version"] = m_applicationVersion;
    m_applicationState["qt_version"] = QString::fromLatin1(QT_VERSION_STR);
    m_applicationState["process_id"] = static_cast<qint64>(QCoreApplication::applicationPid());
}

QString CrashHandler::generateStackTrace(void* context) {
    QString stackTrace;
    
    // Use backtrace for stack trace
    void* array[50];
    size_t size = backtrace(array, 50);
    char** symbols = backtrace_symbols(array, size);
    
    if (symbols) {
        for (size_t i = 0; i < size; ++i) {
            stackTrace += QString("#%1 %2\n").arg(i).arg(symbols[i]);
        }
        free(symbols);
    }
    
    return stackTrace;
}

QStringList CrashHandler::getLoadedLibraries() {
    QStringList libraries;
    
    // Use dl_iterate_phdr to get loaded libraries
    struct LibraryInfo {
        QStringList* libs;
    } info;
    info.libs = &libraries;
    
    dl_iterate_phdr([](struct dl_phdr_info* info, size_t, void* data) -> int {
        LibraryInfo* libInfo = static_cast<LibraryInfo*>(data);
        if (info->dlpi_name && strlen(info->dlpi_name) > 0) {
            libInfo->libs->append(QString::fromLocal8Bit(info->dlpi_name));
        }
        return 0;
    }, &info);
    
    return libraries;
}

QString CrashHandler::sanitizeCrashReport(const QString& report) const {
    // Remove sensitive information from crash report
    QString sanitized = report;
    
    // Remove potential passwords/tokens (basic pattern matching)
    QRegularExpression passwordRegex(R"((password|token|key|secret)\s*[:=]\s*["']?[^"'\s]+["']?)", QRegularExpression::CaseInsensitiveOption);
    sanitized.replace(passwordRegex, "\\1: [REDACTED]");
    
    // Remove file paths that might contain user information
    QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    sanitized.replace(homePath, "[HOME]");
    
    return sanitized;
}

bool CrashHandler::canRecoverFromCrash(const CrashInfo& crashInfo) const {
    // Determine if crash is recoverable
    // Some crashes (like memory corruption) are not recoverable
    // Others (like null pointer dereference in non-critical code) might be
    
    // For now, assume crashes are not recoverable
    // In production, would analyze crash type and context
    return false;
}

