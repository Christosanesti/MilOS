#include "backup_system.h"
#include "backup_service.h"
#include "dbus_interface.h"
#include "config_parser.h"
#include <iostream>

BackupSystem::BackupSystem(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_running(false)
{
}

BackupSystem::~BackupSystem() {
    stop();
}

bool BackupSystem::initialize() {
    if (m_initialized) {
        return true;
    }

    // Load configuration
    m_configParser = std::make_unique<ConfigParser>();
    if (!m_configParser->load("/etc/milos/backup-system/config.yaml")) {
        std::cerr << "Failed to load configuration, using defaults" << std::endl;
    }

    // Initialize backup service
    m_backupService = std::make_unique<BackupService>();
    if (!m_backupService->initialize()) {
        std::cerr << "Failed to initialize backup service" << std::endl;
        return false;
    }

    // Initialize D-Bus interface
    m_dbusInterface = std::make_unique<DBusInterface>();
    if (!m_dbusInterface->initialize(m_backupService.get())) {
        std::cerr << "Failed to initialize D-Bus interface" << std::endl;
        return false;
    }

    m_initialized = true;
    return true;
}

bool BackupSystem::start() {
    if (!m_initialized) {
        if (!initialize()) {
            return false;
        }
    }

    if (m_running) {
        return true;
    }

    // Start backup service
    if (!m_backupService->start()) {
        std::cerr << "Failed to start backup service" << std::endl;
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
    std::cout << "Backup System started" << std::endl;
    return true;
}

void BackupSystem::stop() {
    if (!m_running) {
        return;
    }

    // Stop backup service
    if (m_backupService) {
        m_backupService->stop();
    }

    // Stop D-Bus interface
    if (m_dbusInterface) {
        m_dbusInterface->stop();
    }

    m_running = false;
    std::cout << "Backup System stopped" << std::endl;
}

