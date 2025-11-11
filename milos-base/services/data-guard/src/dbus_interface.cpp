#include "dbus_interface.h"
#include "config_parser.h"
#include "policy_manager.h"
#include "network_enforcement.h"
#include <iostream>

DBusInterface::DBusInterface()
    : m_running(false)
    , m_initialized(false)
    , m_configParser(nullptr)
    , m_policyManager(nullptr)
    , m_networkEnforcement(nullptr)
    , m_dbusConnection(nullptr)
{
}

DBusInterface::~DBusInterface() {
    stop();
}

bool DBusInterface::initialize(
    ConfigParser* configParser,
    PolicyManager* policyManager,
    NetworkEnforcement* networkEnforcement
) {
    if (m_initialized) {
        return true;
    }

    m_configParser = configParser;
    m_policyManager = policyManager;
    m_networkEnforcement = networkEnforcement;

    m_initialized = true;
    return true;
}

bool DBusInterface::start() {
    if (!m_initialized) {
        std::cerr << "D-Bus interface not initialized" << std::endl;
        return false;
    }

    if (m_running) {
        return true;
    }

    // Register D-Bus interface
    if (!registerInterface()) {
        std::cerr << "Failed to register D-Bus interface" << std::endl;
        return false;
    }

    m_running = true;
    std::cout << "D-Bus interface started" << std::endl;
    return true;
}

void DBusInterface::stop() {
    if (!m_running) {
        return;
    }

    unregisterInterface();
    m_running = false;
    std::cout << "D-Bus interface stopped" << std::endl;
}

bool DBusInterface::isHealthy() const {
    if (!m_running) {
        return false;
    }

    // TODO: Check D-Bus connection health
    return true;
}

std::string DBusInterface::getTransmissionStatus() {
    if (!m_networkEnforcement) {
        return "{\"status\":\"error\",\"message\":\"Network enforcement not available\"}";
    }

    // TODO: Return JSON status
    return "{\"status\":\"running\",\"blocked\":" + 
           std::to_string(m_networkEnforcement->getBlockedCount()) +
           ",\"allowed\":" + 
           std::to_string(m_networkEnforcement->getAllowedCount()) + "}";
}

std::vector<std::string> DBusInterface::getBlockedTransmissions() {
    // TODO: Return list of blocked transmissions
    return {};
}

bool DBusInterface::configurePolicy(const std::string& policy) {
    // TODO: Parse policy JSON/YAML and configure
    return false;
}

std::string DBusInterface::getPolicyStatus(const std::string& policyId) {
    if (!m_policyManager) {
        return "{\"status\":\"error\",\"message\":\"Policy manager not available\"}";
    }

    const NetworkPolicy* policy = m_policyManager->getPolicy(policyId);
    if (!policy) {
        return "{\"status\":\"not_found\"}";
    }

    return "{\"status\":\"found\",\"enabled\":" + 
           std::string(policy->enabled ? "true" : "false") + "}";
}

bool DBusInterface::registerInterface() {
    // TODO: Implement D-Bus interface registration
    // Using Qt D-Bus or libdbus
    return true;
}

void DBusInterface::unregisterInterface() {
    // TODO: Implement D-Bus interface unregistration
    m_dbusConnection = nullptr;
}

