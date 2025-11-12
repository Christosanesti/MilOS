#include "launcher.h"
#include "tool_manager.h"
#include "dbus_interface.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <iostream>

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
        std::cerr << "Failed to initialize tool manager" << std::endl;
        return false;
    }

    // Initialize D-Bus interface
    DBusInterface* dbusInterface = new DBusInterface(this);
    dbusInterface->setToolManager(m_toolManager);
    if (!dbusInterface->initialize()) {
        std::cerr << "Warning: Failed to initialize D-Bus interface (continuing without D-Bus)" << std::endl;
    }

    // Register tool manager with QML
    m_engine->rootContext()->setContextProperty("toolManager", m_toolManager);

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

int Launcher::run() {
    if (!m_initialized) {
        if (!initialize()) {
            return 1;
        }
    }

    return m_app->exec();
}

