#include "security_status_dashboard.h"
#include "status_aggregator.h"
#include "dbus_interface.h"
#include "audit_logger.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <iostream>

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

    // Create QApplication
    int argc = 0;
    m_app = new QApplication(argc, nullptr);
    m_app->setApplicationName("MilOS Security Status Dashboard");
    m_app->setOrganizationName("MilOS");

    // Create QML engine
    m_engine = new QQmlApplicationEngine(this);

    // Initialize status aggregator
    m_statusAggregator = new StatusAggregator(this);
    if (!m_statusAggregator->initialize()) {
        std::cerr << "Failed to initialize status aggregator" << std::endl;
        return false;
    }

    // Initialize D-Bus interface
    DBusInterface* dbusInterface = new DBusInterface(this);
    dbusInterface->setStatusAggregator(m_statusAggregator);
    if (!dbusInterface->initialize()) {
        std::cerr << "Warning: Failed to initialize D-Bus interface (continuing without D-Bus)" << std::endl;
    }

    // Initialize audit logger
    AuditLogger* auditLogger = new AuditLogger(this);
    if (!auditLogger->initialize()) {
        std::cerr << "Warning: Failed to initialize audit logger (continuing without audit logging)" << std::endl;
    }

    // Register with QML
    m_engine->rootContext()->setContextProperty("statusAggregator", m_statusAggregator);

    // Load QML
    const QUrl qmlUrl(QStringLiteral("qrc:/ui/main.qml"));
    m_engine->load(qmlUrl);

    if (m_engine->rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
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

