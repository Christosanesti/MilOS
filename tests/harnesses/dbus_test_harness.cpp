#include "dbus_test_harness.h"
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusContext>
#include <QDebug>

DBusTestHarness::DBusTestHarness(QObject* parent)
    : QObject(parent)
    , m_sessionBusName("test-bus")
{
}

DBusTestHarness::~DBusTestHarness()
{
    // Stop all mock services
    for (auto it = m_mockServices.begin(); it != m_mockServices.end(); ++it) {
        stopMockService(it.key());
    }
}

bool DBusTestHarness::startMockService(const QString& serviceName, const QString& objectPath)
{
    if (m_mockServices.contains(serviceName)) {
        qWarning() << "Mock service already started:" << serviceName;
        return true;
    }

    MockService service;
    service.serviceName = serviceName;
    service.objectPath = objectPath.isEmpty() ? generateObjectPath(serviceName) : objectPath;
    service.interface = generateInterface(serviceName);
    service.available = true;

    // Use session bus for testing
    QDBusConnection connection = QDBusConnection::sessionBus();
    if (!connection.isConnected()) {
        qWarning() << "D-Bus session bus not available";
        return false;
    }

    service.connection = connection;

    // Register service (simplified - in real implementation would register object)
    if (!connection.registerService(serviceName)) {
        qWarning() << "Failed to register mock service:" << serviceName;
        return false;
    }

    m_mockServices[serviceName] = service;
    qDebug() << "Mock D-Bus service started:" << serviceName;
    return true;
}

void DBusTestHarness::stopMockService(const QString& serviceName)
{
    if (!m_mockServices.contains(serviceName)) {
        return;
    }

    MockService& service = m_mockServices[serviceName];
    service.connection.unregisterService(serviceName);
    m_mockServices.remove(serviceName);
    qDebug() << "Mock D-Bus service stopped:" << serviceName;
}

bool DBusTestHarness::isServiceAvailable(const QString& serviceName) const
{
    if (!m_mockServices.contains(serviceName)) {
        return false;
    }
    return m_mockServices[serviceName].available;
}

void DBusTestHarness::setMockResponse(const QString& serviceName,
                                      const QString& methodName,
                                      const QVariant& response)
{
    if (!m_mockServices.contains(serviceName)) {
        qWarning() << "Service not started:" << serviceName;
        return;
    }

    m_mockServices[serviceName].responses[methodName] = response;
}

QList<QVariantList> DBusTestHarness::getCallHistory(const QString& serviceName,
                                                    const QString& methodName) const
{
    if (!m_mockServices.contains(serviceName)) {
        return QList<QVariantList>();
    }

    return m_mockServices[serviceName].callHistory.value(methodName);
}

void DBusTestHarness::clearCallHistory()
{
    for (auto it = m_mockServices.begin(); it != m_mockServices.end(); ++it) {
        it.value().callHistory.clear();
    }
}

void DBusTestHarness::simulateServiceUnavailable(const QString& serviceName)
{
    if (m_mockServices.contains(serviceName)) {
        m_mockServices[serviceName].available = false;
    }
}

void DBusTestHarness::simulateServiceError(const QString& serviceName,
                                          const QString& errorMessage)
{
    if (m_mockServices.contains(serviceName)) {
        m_mockServices[serviceName].errorMessage = errorMessage;
        m_mockServices[serviceName].available = false;
    }
}

void DBusTestHarness::onMethodCall(const QDBusMessage& message)
{
    QString serviceName = message.service();
    QString methodName = message.member();

    if (!m_mockServices.contains(serviceName)) {
        return;
    }

    MockService& service = m_mockServices[serviceName];

    // Record call
    QVariantList args;
    for (const QVariant& arg : message.arguments()) {
        args.append(arg);
    }
    service.callHistory[methodName].append(args);

    // Return mock response if set
    if (service.responses.contains(methodName)) {
        QVariant response = service.responses[methodName];
        // In real implementation, would send reply
    }
}

QString DBusTestHarness::generateObjectPath(const QString& serviceName) const
{
    // Convert service name to object path
    // e.g., "org.milos.AuditService" -> "/org/milos/AuditService"
    QString path = "/" + serviceName;
    path.replace(".", "/");
    return path;
}

QString DBusTestHarness::generateInterface(const QString& serviceName) const
{
    // Use service name as interface
    return serviceName;
}

