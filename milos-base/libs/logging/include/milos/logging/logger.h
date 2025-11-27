#ifndef MILOS_LOGGING_LOGGER_H
#define MILOS_LOGGING_LOGGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QVariantMap>
#include <QDBusInterface>
#include <QDBusConnection>

/**
 * @brief Centralized Logging System for MilOS
 * 
 * Provides unified logging with multiple log levels, D-Bus integration
 * with audit service, and graceful degradation.
 */
class Logger : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Log levels
     */
    enum LogLevel {
        Debug = 0,      // Debug information (only in debug builds)
        Info = 1,       // Informational messages
        Warning = 2,    // Warning messages
        Error = 3,      // Error messages
        Critical = 4,   // Critical errors
        Audit = 5       // Audit events (always logged)
    };
    Q_ENUM(LogLevel)

    /**
     * @brief Get singleton instance
     */
    static Logger* instance();

    /**
     * @brief Initialize logger
     * @param componentName Component/service name
     * @param auditServiceBus D-Bus service name for audit service
     * @param auditServicePath D-Bus object path for audit service
     * @param minLogLevel Minimum log level to output
     * @param enableFileLogging Enable file logging
     * @return true if initialization successful
     */
    bool initialize(const QString& componentName,
                    const QString& auditServiceBus = "org.milos.AuditService",
                    const QString& auditServicePath = "/org/milos/AuditService",
                    LogLevel minLogLevel = Info,
                    bool enableFileLogging = false);

    /**
     * @brief Set minimum log level
     */
    void setMinLogLevel(LogLevel level);

    /**
     * @brief Log message
     */
    void log(LogLevel level, const QString& message, const QVariantMap& context = QVariantMap());

    /**
     * @brief Convenience methods for each log level
     */
    void debug(const QString& message, const QVariantMap& context = QVariantMap());
    void info(const QString& message, const QVariantMap& context = QVariantMap());
    void warning(const QString& message, const QVariantMap& context = QVariantMap());
    void error(const QString& message, const QVariantMap& context = QVariantMap());
    void critical(const QString& message, const QVariantMap& context = QVariantMap());
    void audit(const QString& eventType, const QVariantMap& eventData = QVariantMap());

    /**
     * @brief Check if audit service is available
     */
    bool isAuditServiceAvailable() const;

    /**
     * @brief Get component name
     */
    QString componentName() const { return m_componentName; }

    /**
     * @brief Check if logger is initialized
     */
    bool isInitialized() const;

private:
    Logger(QObject* parent = nullptr);
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void writeToFile(LogLevel level, const QString& message, const QVariantMap& context);
    bool sendToAuditService(LogLevel level, const QString& message, const QVariantMap& context);
    QString logLevelToString(LogLevel level) const;
    QString formatMessage(LogLevel level, const QString& message) const;

    static Logger* s_instance;

    QString m_componentName;
    QString m_auditServiceBus;
    QString m_auditServicePath;
    LogLevel m_minLogLevel;
    bool m_initialized;
    bool m_enableFileLogging;
    bool m_gracefulDegradation;
    QDBusInterface* m_auditInterface;
};

// Convenience macros
#ifdef QT_DEBUG
#define LOG_DEBUG(msg) Logger::instance()->debug(msg)
#define LOG_DEBUG_CTX(msg, ctx) Logger::instance()->debug(msg, ctx)
#else
#define LOG_DEBUG(msg) ((void)0)
#define LOG_DEBUG_CTX(msg, ctx) ((void)0)
#endif

#define LOG_INFO(msg) Logger::instance()->info(msg)
#define LOG_INFO_CTX(msg, ctx) Logger::instance()->info(msg, ctx)
#define LOG_WARNING(msg) Logger::instance()->warning(msg)
#define LOG_WARNING_CTX(msg, ctx) Logger::instance()->warning(msg, ctx)
#define LOG_ERROR(msg) Logger::instance()->error(msg)
#define LOG_ERROR_CTX(msg, ctx) Logger::instance()->error(msg, ctx)
#define LOG_CRITICAL(msg) Logger::instance()->critical(msg)
#define LOG_CRITICAL_CTX(msg, ctx) Logger::instance()->critical(msg, ctx)
#define LOG_AUDIT(event) Logger::instance()->audit(event)
#define LOG_AUDIT_CTX(event, ctx) Logger::instance()->audit(event, ctx)

#endif // MILOS_LOGGING_LOGGER_H



#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QVariantMap>
#include <QDBusInterface>
#include <QDBusConnection>

/**
 * @brief Centralized Logging System for MilOS
 * 
 * Provides unified logging with multiple log levels, D-Bus integration
 * with audit service, and graceful degradation.
 */
class Logger : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Log levels
     */
    enum LogLevel {
        Debug = 0,      // Debug information (only in debug builds)
        Info = 1,       // Informational messages
        Warning = 2,    // Warning messages
        Error = 3,      // Error messages
        Critical = 4,   // Critical errors
        Audit = 5       // Audit events (always logged)
    };
    Q_ENUM(LogLevel)

    /**
     * @brief Get singleton instance
     */
    static Logger* instance();

    /**
     * @brief Initialize logger
     * @param componentName Component/service name
     * @param auditServiceBus D-Bus service name for audit service
     * @param auditServicePath D-Bus object path for audit service
     * @param minLogLevel Minimum log level to output
     * @param enableFileLogging Enable file logging
     * @return true if initialization successful
     */
    bool initialize(const QString& componentName,
                    const QString& auditServiceBus = "org.milos.AuditService",
                    const QString& auditServicePath = "/org/milos/AuditService",
                    LogLevel minLogLevel = Info,
                    bool enableFileLogging = false);

    /**
     * @brief Set minimum log level
     */
    void setMinLogLevel(LogLevel level);

    /**
     * @brief Log message
     */
    void log(LogLevel level, const QString& message, const QVariantMap& context = QVariantMap());

    /**
     * @brief Convenience methods for each log level
     */
    void debug(const QString& message, const QVariantMap& context = QVariantMap());
    void info(const QString& message, const QVariantMap& context = QVariantMap());
    void warning(const QString& message, const QVariantMap& context = QVariantMap());
    void error(const QString& message, const QVariantMap& context = QVariantMap());
    void critical(const QString& message, const QVariantMap& context = QVariantMap());
    void audit(const QString& eventType, const QVariantMap& eventData = QVariantMap());

    /**
     * @brief Check if audit service is available
     */
    bool isAuditServiceAvailable() const;

    /**
     * @brief Get component name
     */
    QString componentName() const { return m_componentName; }

    /**
     * @brief Check if logger is initialized
     */
    bool isInitialized() const;

private:
    Logger(QObject* parent = nullptr);
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void writeToFile(LogLevel level, const QString& message, const QVariantMap& context);
    bool sendToAuditService(LogLevel level, const QString& message, const QVariantMap& context);
    QString logLevelToString(LogLevel level) const;
    QString formatMessage(LogLevel level, const QString& message) const;

    static Logger* s_instance;

    QString m_componentName;
    QString m_auditServiceBus;
    QString m_auditServicePath;
    LogLevel m_minLogLevel;
    bool m_initialized;
    bool m_enableFileLogging;
    bool m_gracefulDegradation;
    QDBusInterface* m_auditInterface;
};

// Convenience macros
#ifdef QT_DEBUG
#define LOG_DEBUG(msg) Logger::instance()->debug(msg)
#define LOG_DEBUG_CTX(msg, ctx) Logger::instance()->debug(msg, ctx)
#else
#define LOG_DEBUG(msg) ((void)0)
#define LOG_DEBUG_CTX(msg, ctx) ((void)0)
#endif

#define LOG_INFO(msg) Logger::instance()->info(msg)
#define LOG_INFO_CTX(msg, ctx) Logger::instance()->info(msg, ctx)
#define LOG_WARNING(msg) Logger::instance()->warning(msg)
#define LOG_WARNING_CTX(msg, ctx) Logger::instance()->warning(msg, ctx)
#define LOG_ERROR(msg) Logger::instance()->error(msg)
#define LOG_ERROR_CTX(msg, ctx) Logger::instance()->error(msg, ctx)
#define LOG_CRITICAL(msg) Logger::instance()->critical(msg)
#define LOG_CRITICAL_CTX(msg, ctx) Logger::instance()->critical(msg, ctx)
#define LOG_AUDIT(event) Logger::instance()->audit(event)
#define LOG_AUDIT_CTX(event, ctx) Logger::instance()->audit(event, ctx)

#endif // MILOS_LOGGING_LOGGER_H

