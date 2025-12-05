#include "milos/logging/logger.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDBusReply>
#include <QDBusError>
#include <iostream>
#include <ctime>

Logger* Logger::s_instance = nullptr;

Logger::Logger(QObject* parent)
    : QObject(parent)
    , m_minLogLevel(Info)
    , m_initialized(false)
    , m_enableFileLogging(false)
    , m_gracefulDegradation(true)
    , m_auditInterface(nullptr)
{
}

Logger::~Logger() {
    if (m_auditInterface) {
        delete m_auditInterface;
        m_auditInterface = nullptr;
    }
}

Logger* Logger::instance() {
    if (!s_instance) {
        s_instance = new Logger();
    }
    return s_instance;
}

bool Logger::isInitialized() const {
    return m_initialized;
}

bool Logger::initialize(const QString& componentName,
                       const QString& auditServiceBus,
                       const QString& auditServicePath,
                       LogLevel minLogLevel,
                       bool enableFileLogging) {
    if (m_initialized) {
        return true;
    }

    m_componentName = componentName;
    m_auditServiceBus = auditServiceBus;
    m_auditServicePath = auditServicePath;
    m_minLogLevel = minLogLevel;
    m_enableFileLogging = enableFileLogging;

    // Initialize D-Bus interface to audit service
    QDBusConnection connection = QDBusConnection::systemBus();
    if (connection.isConnected()) {
        m_auditInterface = new QDBusInterface(
            m_auditServiceBus,
            m_auditServicePath,
            "org.milos.AuditService",
            connection
        );

        if (!m_auditInterface->isValid()) {
            if (m_gracefulDegradation) {
                std::cerr << "Warning: Audit service not available, using fallback logging" << std::endl;
            } else {
                delete m_auditInterface;
                m_auditInterface = nullptr;
                return false;
            }
        }
    } else {
        if (m_gracefulDegradation) {
            std::cerr << "Warning: D-Bus not connected, using fallback logging" << std::endl;
        } else {
            return false;
        }
    }

    // Create log directory if file logging enabled
    if (m_enableFileLogging) {
        QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/logs";
        QDir().mkpath(logDir);
    }

    m_initialized = true;
    return true;
}

void Logger::setMinLogLevel(LogLevel level) {
    m_minLogLevel = level;
}

void Logger::log(LogLevel level, const QString& message, const QVariantMap& context) {
    if (!m_initialized) {
        // Fallback to stderr if not initialized
        std::cerr << "[UNINITIALIZED] " << message.toStdString() << std::endl;
        return;
    }

    // Check if we should log this level
    if (level < m_minLogLevel && level != Audit) {
        return;
    }

    QString formattedMessage = formatMessage(level, message);

    // Write to file if enabled
    if (m_enableFileLogging) {
        writeToFile(level, message, context);
    }

    // Send to audit service for Audit and Critical levels
    if (level == Audit || level == Critical) {
        sendToAuditService(level, message, context);
    }

    // Output to console based on level
    if (level >= Error) {
        std::cerr << formattedMessage.toStdString() << std::endl;
    } else if (level >= Warning) {
        std::cout << formattedMessage.toStdString() << std::endl;
    } else if (level == Info) {
        std::cout << formattedMessage.toStdString() << std::endl;
    }
#ifdef QT_DEBUG
    else if (level == Debug) {
        std::cout << formattedMessage.toStdString() << std::endl;
    }
#endif
}

void Logger::debug(const QString& message, const QVariantMap& context) {
    log(Debug, message, context);
}

void Logger::info(const QString& message, const QVariantMap& context) {
    log(Info, message, context);
}

void Logger::warning(const QString& message, const QVariantMap& context) {
    log(Warning, message, context);
}

void Logger::error(const QString& message, const QVariantMap& context) {
    log(Error, message, context);
}

void Logger::critical(const QString& message, const QVariantMap& context) {
    log(Critical, message, context);
}

void Logger::audit(const QString& eventType, const QVariantMap& eventData) {
    QVariantMap context = eventData;
    context["event_type"] = eventType;
    log(Audit, QString("Audit event: %1").arg(eventType), context);
}

bool Logger::isAuditServiceAvailable() const {
    return m_auditInterface && m_auditInterface->isValid();
}

void Logger::writeToFile(LogLevel level, const QString& message, const QVariantMap& context) {
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/logs";
    QString logFile = QString("%1/%2.log").arg(logDir, m_componentName);

    QFile file(logFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&file);
        QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
        QString levelStr = logLevelToString(level);
        stream << QString("[%1] [%2] [%3] %4\n")
                  .arg(timestamp, levelStr, m_componentName, message);
        
        if (!context.isEmpty()) {
            QJsonDocument doc = QJsonDocument::fromVariant(context);
            stream << "  Context: " << doc.toJson(QJsonDocument::Compact) << "\n";
        }
        file.close();
    }
}

bool Logger::sendToAuditService(LogLevel level, const QString& message, const QVariantMap& context) {
    if (!isAuditServiceAvailable()) {
        return false;
    }

    QVariantMap eventData = context;
    eventData["component"] = m_componentName;
    eventData["log_level"] = logLevelToString(level);
    eventData["message"] = message;
    eventData["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QJsonObject eventObj;
    for (auto it = eventData.begin(); it != eventData.end(); ++it) {
        eventObj[it.key()] = QJsonValue::fromVariant(it.value());
    }

    QJsonDocument doc(eventObj);
    QString eventDataJson = QString::fromUtf8(doc.toJson());

    QString eventType = (level == Audit) ? context.value("event_type").toString() : "log_event";
    if (eventType.isEmpty()) {
        eventType = "log_event";
    }

    QDBusReply<QString> reply = m_auditInterface->call("LogEvent", eventDataJson);

    if (!reply.isValid()) {
        if (m_gracefulDegradation) {
            std::cerr << "Failed to log to audit service: " 
                      << reply.error().message().toStdString() << std::endl;
        }
        return false;
    }

    return true;
}

QString Logger::logLevelToString(LogLevel level) const {
    switch (level) {
        case Debug: return "DEBUG";
        case Info: return "INFO";
        case Warning: return "WARNING";
        case Error: return "ERROR";
        case Critical: return "CRITICAL";
        case Audit: return "AUDIT";
        default: return "UNKNOWN";
    }
}

QString Logger::formatMessage(LogLevel level, const QString& message) const {
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    QString levelStr = logLevelToString(level);
    return QString("[%1] [%2] [%3] %4")
           .arg(timestamp, levelStr, m_componentName, message);
}


#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDBusReply>
