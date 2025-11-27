#include "launcher.h"
#include "tool_manager.h"
#include "dbus_interface.h"
#include <milos/logging/logger.h>
#include <milos/ui/error_handler.h>
#include <milos/ui/crash_handler.h>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>

Launcher::Launcher(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_app(nullptr)
    , m_engine(nullptr)
    , m_toolManager(nullptr)
{
}

Launcher::~Launcher() {
}

bool Launcher::initialize() {
    if (m_initialized) {
        return true;
    }

    // Create QApplication
    int argc = 0;
    m_app = new QApplication(argc, nullptr);
    m_app->setApplicationName("MilOS BlackArch Launcher");
    m_app->setOrganizationName("MilOS");

    // Create QML engine
    m_engine = new QQmlApplicationEngine(this);

    // Initialize tool manager
    m_toolManager = new ToolManager(this);
    if (!m_toolManager->initialize()) {
        LOG_ERROR("Failed to initialize tool manager");
        milos::ui::ErrorHandler::instance()->handleError("Initialization failed",
                                                          "Failed to initialize tool manager",
                                                          milos::ui::ErrorCategory::System,
                                                          milos::ui::ErrorSeverity::Critical);
        return false;
    }

    // Initialize D-Bus interface
    DBusInterface* dbusInterface = new DBusInterface(this);
    dbusInterface->setToolManager(m_toolManager);
    if (!dbusInterface->initialize()) {
        LOG_WARNING("Failed to initialize D-Bus interface (continuing without D-Bus)");
    }

    // Register tool manager with QML
    m_engine->rootContext()->setContextProperty("toolManager", m_toolManager);
    
    // Register error handler and crash handler with QML
    m_engine->rootContext()->setContextProperty("errorHandler", milos::ui::ErrorHandler::instance());
    m_engine->rootContext()->setContextProperty("crashHandler", milos::ui::CrashHandler::instance());

    // Load QML
    const QUrl qmlUrl(QStringLiteral("qrc:/ui/main.qml"));
    m_engine->load(qmlUrl);

    if (m_engine->rootObjects().isEmpty()) {
        LOG_ERROR("Failed to load QML");
        milos::ui::ErrorHandler::instance()->handleError("QML Load Failed",
                                                          "Failed to load QML interface",
                                                          milos::ui::ErrorCategory::UI,
                                                          milos::ui::ErrorSeverity::Critical);
        return false;
    }

    m_initialized = true;
    return true;
}

int Launcher::run() {
    if (!m_initialized) {
        if (!initialize()) {
            return 1;
        }
    }

    return m_app->exec();
}

