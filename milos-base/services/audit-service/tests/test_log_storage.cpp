#include <QtTest>
#include <QObject>
#include <QString>
#include <QFile>
#include <QDir>
#include <QTemporaryDir>
#include <QTextStream>
#include "../src/log_storage.h"
#include "../src/config_parser.h"

/**
 * Unit tests for Log Storage
 * Tests database operations, log entry storage, and retention policies
 */
class LogStorageTest : public QObject
{
    Q_OBJECT

public:
    LogStorageTest();
    ~LogStorageTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testInitialize();
    void testStoreLogEntry();
    void testQueryLogEntries();
    void testGetLogEntry();
    void testGetEventStatistics();
    void testEnforceRetentionPolicies();
    void testGetLastLogHash();
    void testIsHealthy();

private:
    LogStorage* m_logStorage;
    ConfigParser* m_configParser;
    QTemporaryDir* m_tempDir;

    void createConfigFile(const QString& content);
    AuditLogEntry createTestLogEntry(const QString& logId, const QString& eventType);
};

LogStorageTest::LogStorageTest()
    : m_logStorage(nullptr)
    , m_configParser(nullptr)
    , m_tempDir(nullptr)
{
}

LogStorageTest::~LogStorageTest()
{
}

void LogStorageTest::initTestCase()
{
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());
}

void LogStorageTest::cleanupTestCase()
{
    delete m_tempDir;
}

void LogStorageTest::init()
{
    m_configParser = new ConfigParser();
    m_logStorage = new LogStorage();
}

void LogStorageTest::cleanup()
{
    delete m_logStorage;
    delete m_configParser;
}

void LogStorageTest::createConfigFile(const QString& content)
{
    QString configPath = m_tempDir->path() + "/config.yaml";
    QString dbPath = m_tempDir->path() + "/audit.db";
    
    QString fullContent = content;
    fullContent.replace("{{DB_PATH}}", dbPath);
    
    QFile file(configPath);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    QTextStream out(&file);
    out << fullContent;
    file.close();
    QVERIFY(m_configParser->load(configPath.toStdString()));
}

AuditLogEntry LogStorageTest::createTestLogEntry(const QString& logId, const QString& eventType)
{
    AuditLogEntry entry;
    entry.log_id = logId.toStdString();
    entry.event_type = eventType.toStdString();
    entry.event_severity = "INFO";
    entry.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
    entry.user_id = "test_user";
    entry.application_name = "TestApplication";
    entry.event_description = "Test event";
    entry.event_data = "{\"test\":\"data\"}";
    entry.log_hash = "test_hash_" + logId.toStdString();
    entry.previous_log_hash = "previous_hash";
    return entry;
}

void LogStorageTest::testInitialize()
{
    QString configContent = R"(
service:
  enabled: true
  database_type: sqlite
  sqlite_path: "{{DB_PATH}}"
)";
    createConfigFile(configContent);
    QVERIFY(m_logStorage->initialize(m_configParser));
    QVERIFY(m_logStorage->isHealthy());
}

void LogStorageTest::testStoreLogEntry()
{
    QString configContent = R"(
service:
  enabled: true
  database_type: sqlite
  sqlite_path: "{{DB_PATH}}"
)";
    createConfigFile(configContent);
    QVERIFY(m_logStorage->initialize(m_configParser));

    AuditLogEntry entry = createTestLogEntry("test-log-1", "ENCRYPTION");
    QVERIFY(m_logStorage->storeLogEntry(entry));
}

void LogStorageTest::testQueryLogEntries()
{
    QString configContent = R"(
service:
  enabled: true
  database_type: sqlite
  sqlite_path: "{{DB_PATH}}"
)";
    createConfigFile(configContent);
    QVERIFY(m_logStorage->initialize(m_configParser));

    // Store test entries
    AuditLogEntry entry1 = createTestLogEntry("test-log-1", "ENCRYPTION");
    AuditLogEntry entry2 = createTestLogEntry("test-log-2", "NETWORK_TRANSMISSION");
    QVERIFY(m_logStorage->storeLogEntry(entry1));
    QVERIFY(m_logStorage->storeLogEntry(entry2));

    // Query all entries
    std::map<std::string, std::string> filters;
    std::vector<AuditLogEntry> entries = m_logStorage->queryLogEntries(filters, 10, 0);
    QCOMPARE(entries.size(), (size_t)2);

    // Query by event type
    filters["event_type"] = "ENCRYPTION";
    entries = m_logStorage->queryLogEntries(filters, 10, 0);
    QCOMPARE(entries.size(), (size_t)1);
    QCOMPARE(QString::fromStdString(entries[0].event_type), QString("ENCRYPTION"));
}

void LogStorageTest::testGetLogEntry()
{
    QString configContent = R"(
service:
  enabled: true
  database_type: sqlite
  sqlite_path: "{{DB_PATH}}"
)";
    createConfigFile(configContent);
    QVERIFY(m_logStorage->initialize(m_configParser));

    AuditLogEntry entry = createTestLogEntry("test-log-1", "ENCRYPTION");
    QVERIFY(m_logStorage->storeLogEntry(entry));

    std::unique_ptr<AuditLogEntry> retrieved = m_logStorage->getLogEntry("test-log-1");
    QVERIFY(retrieved != nullptr);
    QCOMPARE(QString::fromStdString(retrieved->log_id), QString("test-log-1"));
    QCOMPARE(QString::fromStdString(retrieved->event_type), QString("ENCRYPTION"));
}

void LogStorageTest::testGetEventStatistics()
{
    QString configContent = R"(
service:
  enabled: true
  database_type: sqlite
  sqlite_path: "{{DB_PATH}}"
)";
    createConfigFile(configContent);
    QVERIFY(m_logStorage->initialize(m_configParser));

    // Store test entries
    AuditLogEntry entry1 = createTestLogEntry("test-log-1", "ENCRYPTION");
    AuditLogEntry entry2 = createTestLogEntry("test-log-2", "ENCRYPTION");
    AuditLogEntry entry3 = createTestLogEntry("test-log-3", "NETWORK_TRANSMISSION");
    QVERIFY(m_logStorage->storeLogEntry(entry1));
    QVERIFY(m_logStorage->storeLogEntry(entry2));
    QVERIFY(m_logStorage->storeLogEntry(entry3));

    // Get statistics
    std::map<std::string, std::string> filters;
    std::map<std::string, int> stats = m_logStorage->getEventStatistics(filters);
    QVERIFY(stats.find("ENCRYPTION") != stats.end());
    QCOMPARE(stats["ENCRYPTION"], 2);
    QVERIFY(stats.find("NETWORK_TRANSMISSION") != stats.end());
    QCOMPARE(stats["NETWORK_TRANSMISSION"], 1);
}

void LogStorageTest::testEnforceRetentionPolicies()
{
    QString configContent = R"(
service:
  enabled: true
  database_type: sqlite
  sqlite_path: "{{DB_PATH}}"
retention:
  retention_days: 1
)";
    createConfigFile(configContent);
    QVERIFY(m_logStorage->initialize(m_configParser));

    // Store test entry
    AuditLogEntry entry = createTestLogEntry("test-log-1", "ENCRYPTION");
    QVERIFY(m_logStorage->storeLogEntry(entry));

    // Retention policies will be enforced (entries older than 1 day will be deleted)
    // For this test, we verify the method runs without error
    int deleted = m_logStorage->enforceRetentionPolicies();
    QVERIFY(deleted >= 0);
}

void LogStorageTest::testGetLastLogHash()
{
    QString configContent = R"(
service:
  enabled: true
  database_type: sqlite
  sqlite_path: "{{DB_PATH}}"
)";
    createConfigFile(configContent);
    QVERIFY(m_logStorage->initialize(m_configParser));

    // Initially empty
    std::string lastHash = m_logStorage->getLastLogHash();
    QVERIFY(lastHash.empty());

    // Store entry
    AuditLogEntry entry = createTestLogEntry("test-log-1", "ENCRYPTION");
    QVERIFY(m_logStorage->storeLogEntry(entry));

    // Get last hash
    lastHash = m_logStorage->getLastLogHash();
    QCOMPARE(QString::fromStdString(lastHash), QString("test_hash_test-log-1"));
}

void LogStorageTest::testIsHealthy()
{
    QString configContent = R"(
service:
  enabled: true
  database_type: sqlite
  sqlite_path: "{{DB_PATH}}"
)";
    createConfigFile(configContent);
    QVERIFY(m_logStorage->initialize(m_configParser));
    QVERIFY(m_logStorage->isHealthy());
}

QTEST_MAIN(LogStorageTest)
#include "test_log_storage.moc"

