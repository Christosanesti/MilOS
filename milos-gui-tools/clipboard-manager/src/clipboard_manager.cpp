#include "clipboard_manager.h"
#include "clipboard_service.h"
#include "dbus_interface.h"
#include "config_parser.h"
#include <iostream>

ClipboardManager::ClipboardManager(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_running(false)
{
}

ClipboardManager::~ClipboardManager() {
    stop();
}

bool ClipboardManager::initialize() {
    if (m_initialized) {
        return true;
    }

    // Load configuration
    m_configParser = std::make_unique<ConfigParser>();
    if (!m_configParser->load("/etc/milos/clipboard-manager/config.yaml")) {
        std::cerr << "Failed to load configuration, using defaults" << std::endl;
    }

    // Initialize clipboard service
    m_clipboardService = std::make_unique<ClipboardService>();
    if (!m_clipboardService->initialize()) {
        std::cerr << "Failed to initialize clipboard service" << std::endl;
        return false;
    }

    // Initialize D-Bus interface
    m_dbusInterface = std::make_unique<DBusInterface>();
    if (!m_dbusInterface->initialize(m_clipboardService.get())) {
        std::cerr << "Failed to initialize D-Bus interface" << std::endl;
        return false;
    }

    m_initialized = true;
    return true;
}

bool ClipboardManager::start() {
    if (!m_initialized) {
        if (!initialize()) {
            return false;
        }
    }

    if (m_running) {
        return true;
    }

    // Start clipboard service
    if (!m_clipboardService->start()) {
        std::cerr << "Failed to start clipboard service" << std::endl;
        return false;
    }

    // Start D-Bus interface
    if (m_configParser->getBool("dbus.enabled", true)) {
        if (!m_dbusInterface->start()) {
            std::cerr << "Failed to start D-Bus interface" << std::endl;
            return false;
        }
    }

    m_running = true;
    std::cout << "Clipboard Manager started" << std::endl;
    return true;
}

void ClipboardManager::stop() {
    if (!m_running) {
        return;
    }

    // Stop clipboard service
    if (m_clipboardService) {
        m_clipboardService->stop();
    }

    // Stop D-Bus interface
    if (m_dbusInterface) {
        m_dbusInterface->stop();
    }

    m_running = false;
    std::cout << "Clipboard Manager stopped" << std::endl;
}

