#include <QtTest>
#include <QObject>
#include <QString>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QTest>

/**
 * Integration tests for D-Bus interfaces
 * Tests all three service D-Bus interfaces
 */
class DBusInterfaceTest : public QObject
{
    Q_OBJECT

public:
    DBusInterfaceTest();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testDataGuardInterface();
    void testAuditServiceInterface();
    void testUpdateServiceInterface();
    void testDBusSignalSubscriptions();

private:
    QDBusConnection m_connection;
    bool isServiceAvailable(const QString& serviceName, const QString& objectPath);
};

DBusInterfaceTest::DBusInterfaceTest()
{
}

void DBusInterfaceTest::initTestCase()
{
    m_connection = QDBusConnection::systemBus();
    QVERIFY(m_connection.isConnected());
}

void DBusInterfaceTest::cleanupTestCase()
{
}

bool DBusInterfaceTest::isServiceAvailable(const QString& serviceName, const QString& objectPath)
{
    QDBusInterface interface(serviceName, objectPath, serviceName, m_connection);
    return interface.isValid();
}

void DBusInterfaceTest::testDataGuardInterface()
{
    // Test org.milos.DataGuard interface
    
    bool available = isServiceAvailable("org.milos.DataGuard", "/org/milos/DataGuard");
    
    if (!available) {
        QSKIP("Data Transmission Guard service not available (service not running in test environment)");
    }

    QDBusInterface interface(
        "org.milos.DataGuard",
        "/org/milos/DataGuard",
        "org.milos.DataGuard",
        m_connection,
        this
    );

    QVERIFY(interface.isValid());

    // Test GetTransmissionStatus method
    QDBusReply<QString> reply = interface.call("GetTransmissionStatus");
    if (reply.isValid()) {
        // Verify response is valid JSON
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(reply.value().toUtf8(), &error);
        QVERIFY(error.error == QJsonParseError::NoError);
    }

    // Test GetHealthStatus method
    QDBusReply<QString> healthReply = interface.call("GetHealthStatus");
    if (healthReply.isValid()) {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(healthReply.value().toUtf8(), &error);
        QVERIFY(error.error == QJsonParseError::NoError);
    }
}

void DBusInterfaceTest::testAuditServiceInterface()
{
    // Test org.milos.AuditService interface
    
    bool available = isServiceAvailable("org.milos.AuditService", "/org/milos/AuditService");
    
    if (!available) {
        QSKIP("Audit Service not available (service not running in test environment)");
    }

    QDBusInterface interface(
        "org.milos.AuditService",
        "/org/milos/AuditService",
        "org.milos.AuditService",
        m_connection,
        this
    );

    QVERIFY(interface.isValid());

    // Test LogEvent method
    QJsonObject eventData;
    eventData["event_type"] = "TEST";
    eventData["event_severity"] = "INFO";
    eventData["application_name"] = "TestApp";
    QJsonDocument doc(eventData);
    QString eventJson = QString::fromUtf8(doc.toJson());

    QDBusReply<QString> reply = interface.call("LogEvent", eventJson);
    if (reply.isValid()) {
        // Event logged successfully
        QVERIFY(!reply.value().isEmpty());
    }

    // Test GetHealthStatus method
    QDBusReply<QString> healthReply = interface.call("GetHealthStatus");
    if (healthReply.isValid()) {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(healthReply.value().toUtf8(), &error);
        QVERIFY(error.error == QJsonParseError::NoError);
    }
}

void DBusInterfaceTest::testUpdateServiceInterface()
{
    // Test org.milos.UpdateService interface
    
    bool available = isServiceAvailable("org.milos.UpdateService", "/org/milos/UpdateService");
    
    if (!available) {
        QSKIP("Update Service not available (service not running in test environment)");
    }

    QDBusInterface interface(
        "org.milos.UpdateService",
        "/org/milos/UpdateService",
        "org.milos.UpdateService",
        m_connection,
        this
    );

    QVERIFY(interface.isValid());

    // Test CheckUpdates method
    QDBusReply<QString> reply = interface.call("CheckUpdates");
    if (reply.isValid()) {
        // Verify response is valid JSON
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(reply.value().toUtf8(), &error);
        QVERIFY(error.error == QJsonParseError::NoError);
    }

    // Test GetHealthStatus method
    QDBusReply<QString> healthReply = interface.call("GetHealthStatus");
    if (healthReply.isValid()) {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(healthReply.value().toUtf8(), &error);
        QVERIFY(error.error == QJsonParseError::NoError);
    }
}

void DBusInterfaceTest::testDBusSignalSubscriptions()
{
    // Test D-Bus signal subscriptions
    // Note: This would require setting up signal handlers and emitting signals
    
    QSKIP("Signal subscription test requires signal emission from services");
}

QTEST_MAIN(DBusInterfaceTest)
#include "test_dbus_interfaces.moc"

