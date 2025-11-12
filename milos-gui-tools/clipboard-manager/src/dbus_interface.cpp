#include "dbus_interface.h"
#include "clipboard_service.h"
#include <QDBusConnection>
#include <QDBusError>
#include <iostream>

DBusInterface::DBusInterface(QObject* parent)
    : QObject(parent)
    , m_running(false)
    , m_initialized(false)
    , m_clipboardService(nullptr)
{
}

DBusInterface::~DBusInterface() {
    stop();
}

bool DBusInterface::initialize(ClipboardService* clipboardService) {
    if (m_initialized) {
        return true;
    }

    m_clipboardService = clipboardService;

    if (!m_clipboardService) {
        std::cerr << "Clipboard service is null" << std::endl;
        return false;
    }

    // Connect to clipboard service signals
    connect(m_clipboardService, &ClipboardService::clipboardChanged,
            this, [this]() {
                QString data = m_clipboardService->getClipboard();
                emit ClipboardChanged(data);
            });

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

    QDBusConnection connection = QDBusConnection::sessionBus();

    if (!connection.isConnected()) {
        std::cerr << "Cannot connect to D-Bus session bus: "
                  << connection.lastError().message().toStdString() << std::endl;
        return false;
    }

    QString objectPath = "/org/milos/ClipboardManager";
    if (!connection.registerObject(objectPath, this)) {
        std::cerr << "Failed to register D-Bus object: "
                  << connection.lastError().message().toStdString() << std::endl;
        return false;
    }

    QString serviceName = "org.milos.ClipboardManager";
    if (!connection.registerService(serviceName)) {
        std::cerr << "Failed to register D-Bus service: "
                  << connection.lastError().message().toStdString() << std::endl;
        connection.unregisterObject(objectPath);
        return false;
    }

    std::cout << "D-Bus interface registered: " << serviceName.toStdString()
              << " at " << objectPath.toStdString() << std::endl;

    m_running = true;
    return true;
}

void DBusInterface::stop() {
    if (!m_running) {
        return;
    }

    QDBusConnection connection = QDBusConnection::sessionBus();

    QString objectPath = "/org/milos/ClipboardManager";
    QString serviceName = "org.milos.ClipboardManager";

    connection.unregisterObject(objectPath);
    connection.unregisterService(serviceName);

    m_running = false;
    std::cout << "D-Bus interface stopped" << std::endl;
}

bool DBusInterface::CopyToClipboard(const QString& data) {
    if (!m_clipboardService) {
        return false;
    }

    return m_clipboardService->copyToClipboard(data);
}

QString DBusInterface::GetClipboard() {
    if (!m_clipboardService) {
        return QString();
    }

    return m_clipboardService->getClipboard();
}

bool DBusInterface::ClearClipboard() {
    if (!m_clipboardService) {
        return false;
    }

    bool result = m_clipboardService->clearClipboard();
    if (result) {
        emit ClipboardCleared();
    }
    return result;
}

QString DBusInterface::GetClipboardHistory() {
    if (!m_clipboardService) {
        return QString("[]");
    }

    return m_clipboardService->getClipboardHistory();
}

