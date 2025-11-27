#include "security_status_dashboard.h"
#include "status_aggregator.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include "milos/logging/logger.h"
#include "milos/ui/crash_handler.h"
#include "milos/ui/error_handler.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>

SecurityStatusDashboard::SecurityStatusDashboard(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_app(nullptr)
    , m_engine(nullptr)
    , m_statusAggregator(nullptr)
{
}

SecurityStatusDashboard::~SecurityStatusDashboard() {
}

bool SecurityStatusDashboard::initialize() {
    if (m_initialized) {
        return true;
    }

    // Initialize crash handler FIRST
    CrashHandler::instance()->initialize("milos-security-status-dashboard", "1.0.0");
    CrashHandler::instance()->installCrashHandlers();
    
    // Initialize logger
    Logger::instance()->initialize("milos-security-status-dashboard");
    
    // Initialize error handler
    ErrorHandler::instance()->initialize("milos-security-status-dashboard");
    
    // Create QApplication
    int argc = 0;
    m_app = new QApplication(argc, nullptr);
    m_app->setApplicationName("MilOS Security Status Dashboard");
    m_app->setOrganizationName("MilOS");

    // Create QML engine
    m_engine = new QQmlApplicationEngine(this);
    
    // Expose error handler and crash handler to QML
    m_engine->rootContext()->setContextProperty("errorHandler", ErrorHandler::instance());
    m_engine->rootContext()->setContextProperty("crashHandler", CrashHandler::instance());

    // Initialize status aggregator
    m_statusAggregator = new StatusAggregator(this);
    if (!m_statusAggregator->initialize()) {
        LOG_ERROR("Failed to initialize status aggregator");
        ErrorHandler::instance()->handleError(
            "INITIALIZATION_FAILED",
            "Failed to initialize status aggregator",
            ErrorHandler::Critical,
            ErrorHandler::Initialization
        );
        return false;
    }

    // Initialize D-Bus interface
    DBusInterface* dbusInterface = new DBusInterface(this);
    dbusInterface->setStatusAggregator(m_statusAggregator);
    if (!dbusInterface->initialize()) {
        LOG_WARNING("Failed to initialize D-Bus interface (continuing without D-Bus)");
    }

    // Initialize audit logger
    AuditLogger* auditLogger = new AuditLogger(this);
    if (!auditLogger->initialize()) {
        LOG_WARNING("Failed to initialize audit logger (continuing without audit logging)");
    }

    // Register with QML
    m_engine->rootContext()->setContextProperty("statusAggregator", m_statusAggregator);

    // Load QML
    const QUrl qmlUrl(QStringLiteral("qrc:/ui/main.qml"));
    m_engine->load(qmlUrl);

    if (m_engine->rootObjects().isEmpty()) {
        LOG_ERROR("Failed to load QML");
        ErrorHandler::instance()->handleError(
            "QML_LOAD_FAILED",
            "Failed to load QML interface",
            ErrorHandler::Error,
            ErrorHandler::UI
        );
        return false;
    }

    m_initialized = true;
    return true;
}

int SecurityStatusDashboard::run() {
    if (!m_initialized) {
        if (!initialize()) {
            return 1;
        }
    }

    return m_app->exec();
}


    m_engine->load(qmlUrl);

    if (m_engine->rootObjects().isEmpty()) {
        LOG_ERROR("Failed to load QML");
        ErrorHandler::instance()->handleError(
            "QML_LOAD_FAILED",
            "Failed to load QML interface",
            ErrorHandler::Error,
            ErrorHandler::UI
        );
        return false;
    }

    m_initialized = true;
    return true;
}

int SecurityStatusDashboard::run() {
    if (!m_initialized) {
        if (!initialize()) {
            return 1;
        }
    }

    return m_app->exec();
}

