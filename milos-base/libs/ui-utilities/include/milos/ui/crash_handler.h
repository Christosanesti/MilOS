#ifndef MILOS_UI_CRASH_HANDLER_H
#define MILOS_UI_CRASH_HANDLER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QStringList>
#include <QProcess>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QSysInfo>
#include <QCoreApplication>

/**
 * @brief Crash Handler
 * 
 * Handles application crashes, collects crash information,
 * and provides bug reporting functionality.
 */
class CrashHandler : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool crashDetected READ crashDetected NOTIFY crashDetectedChanged)
    Q_PROPERTY(QString lastCrashReport READ lastCrashReport NOTIFY crashReportChanged)

public:
    /**
     * @brief Crash information structure
     */
    struct CrashInfo {
        QString crashId;
        QDateTime timestamp;
        QString applicationName;
        QString applicationVersion;
        QString signal;  // SIGSEGV, SIGABRT, etc.
        QString errorMessage;
        QString stackTrace;
        QStringList loadedLibraries;
        QVariantMap systemInfo;
        QVariantMap applicationState;
        QString logFile;
        bool canRecover;
    };

    explicit CrashHandler(QObject* parent = nullptr);
    ~CrashHandler();

    /**
     * @brief Get singleton instance
     */
    static CrashHandler* instance();

    /**
     * @brief Initialize crash handler
     * @param applicationName Application name
     * @param applicationVersion Application version
     * @return true if successful
     */
    bool initialize(const QString& applicationName, const QString& applicationVersion = "1.0.0");

    /**
     * @brief Install crash handlers
     * @return true if successful
     */
    bool installCrashHandlers();

    /**
     * @brief Handle crash
     * @param signal Signal number (SIGSEGV, SIGABRT, etc.)
     * @param context Signal context (if available)
     */
    void handleCrash(int signal, void* context = nullptr);

    /**
     * @brief Handle uncaught exception
     * @param exception Exception information
     */
    void handleException(const QString& exception);

    /**
     * @brief Generate crash report
     * @param crashInfo Crash information
     * @return Crash report as JSON string
     */
    Q_INVOKABLE QString generateCrashReport(const CrashInfo& crashInfo) const;

    /**
     * @brief Save crash report to file
     * @param crashInfo Crash information
     * @return File path where report was saved
     */
    Q_INVOKABLE QString saveCrashReport(const CrashInfo& crashInfo);

    /**
     * @brief Submit crash report
     * @param crashReportPath Path to crash report file
     * @param includeSystemInfo Include system information
     * @return true if submission successful
     */
    Q_INVOKABLE bool submitCrashReport(const QString& crashReportPath, bool includeSystemInfo = true);

    /**
     * @brief Get crash reports directory
     */
    Q_INVOKABLE QString getCrashReportsDirectory() const;

    /**
     * @brief Get list of crash reports
     */
    Q_INVOKABLE QStringList getCrashReports() const;

    /**
     * @brief Delete crash report
     */
    Q_INVOKABLE bool deleteCrashReport(const QString& crashReportId);

    // Property getters
    bool crashDetected() const { return m_crashDetected; }
    QString lastCrashReport() const { return m_lastCrashReport; }

signals:
    void crashDetectedChanged();
    void crashReportChanged();
    void crashReportReady(const CrashInfo& crashInfo);
    void crashReportSubmitted(const QString& crashReportId, bool success);

private:
    void setupSignalHandlers();
    void collectSystemInfo();
    void collectApplicationState();
    QString generateStackTrace(void* context = nullptr);
    QStringList getLoadedLibraries();
    QString sanitizeCrashReport(const QString& report) const;
    bool canRecoverFromCrash(const CrashInfo& crashInfo) const;

    static CrashHandler* s_instance;
    static void signalHandler(int signal);
    static void terminateHandler();

    QString m_applicationName;
    QString m_applicationVersion;
    bool m_initialized;
    bool m_crashDetected;
    QString m_lastCrashReport;
    QVariantMap m_systemInfo;
    QVariantMap m_applicationState;
    QString m_crashReportsDir;
};

Q_DECLARE_METATYPE(CrashHandler::CrashInfo)

#endif // MILOS_UI_CRASH_HANDLER_H



#define MILOS_UI_CRASH_HANDLER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QStringList>
#include <QProcess>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QSysInfo>
#include <QCoreApplication>

/**
 * @brief Crash Handler
 * 
 * Handles application crashes, collects crash information,
 * and provides bug reporting functionality.
 */
class CrashHandler : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool crashDetected READ crashDetected NOTIFY crashDetectedChanged)
    Q_PROPERTY(QString lastCrashReport READ lastCrashReport NOTIFY crashReportChanged)

public:
    /**
     * @brief Crash information structure
     */
    struct CrashInfo {
        QString crashId;
        QDateTime timestamp;
        QString applicationName;
        QString applicationVersion;
        QString signal;  // SIGSEGV, SIGABRT, etc.
        QString errorMessage;
        QString stackTrace;
        QStringList loadedLibraries;
        QVariantMap systemInfo;
        QVariantMap applicationState;
        QString logFile;
        bool canRecover;
    };

    explicit CrashHandler(QObject* parent = nullptr);
    ~CrashHandler();

    /**
     * @brief Get singleton instance
     */
    static CrashHandler* instance();

    /**
     * @brief Initialize crash handler
     * @param applicationName Application name
     * @param applicationVersion Application version
     * @return true if successful
     */
    bool initialize(const QString& applicationName, const QString& applicationVersion = "1.0.0");

    /**
     * @brief Install crash handlers
     * @return true if successful
     */
    bool installCrashHandlers();

    /**
     * @brief Handle crash
     * @param signal Signal number (SIGSEGV, SIGABRT, etc.)
     * @param context Signal context (if available)
     */
    void handleCrash(int signal, void* context = nullptr);

    /**
     * @brief Handle uncaught exception
     * @param exception Exception information
     */
    void handleException(const QString& exception);

    /**
     * @brief Generate crash report
     * @param crashInfo Crash information
     * @return Crash report as JSON string
     */
    Q_INVOKABLE QString generateCrashReport(const CrashInfo& crashInfo) const;

    /**
     * @brief Save crash report to file
     * @param crashInfo Crash information
     * @return File path where report was saved
     */
    Q_INVOKABLE QString saveCrashReport(const CrashInfo& crashInfo);

    /**
     * @brief Submit crash report
     * @param crashReportPath Path to crash report file
     * @param includeSystemInfo Include system information
     * @return true if submission successful
     */
    Q_INVOKABLE bool submitCrashReport(const QString& crashReportPath, bool includeSystemInfo = true);

    /**
     * @brief Get crash reports directory
     */
    Q_INVOKABLE QString getCrashReportsDirectory() const;

    /**
     * @brief Get list of crash reports
     */
    Q_INVOKABLE QStringList getCrashReports() const;

    /**
     * @brief Delete crash report
     */
    Q_INVOKABLE bool deleteCrashReport(const QString& crashReportId);

    // Property getters
    bool crashDetected() const { return m_crashDetected; }
    QString lastCrashReport() const { return m_lastCrashReport; }

signals:
    void crashDetectedChanged();
    void crashReportChanged();
    void crashReportReady(const CrashInfo& crashInfo);
    void crashReportSubmitted(const QString& crashReportId, bool success);

private:
    void setupSignalHandlers();
    void collectSystemInfo();
    void collectApplicationState();
    QString generateStackTrace(void* context = nullptr);
    QStringList getLoadedLibraries();
    QString sanitizeCrashReport(const QString& report) const;
    bool canRecoverFromCrash(const CrashInfo& crashInfo) const;

    static CrashHandler* s_instance;
    static void signalHandler(int signal);
    static void terminateHandler();

    QString m_applicationName;
    QString m_applicationVersion;
    bool m_initialized;
    bool m_crashDetected;
    QString m_lastCrashReport;
    QVariantMap m_systemInfo;
    QVariantMap m_applicationState;
    QString m_crashReportsDir;
};

Q_DECLARE_METATYPE(CrashHandler::CrashInfo)

#endif // MILOS_UI_CRASH_HANDLER_H


