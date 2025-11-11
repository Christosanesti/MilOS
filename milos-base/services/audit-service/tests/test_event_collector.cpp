#include <QtTest>
#include <QObject>
#include <QString>
#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include "../src/event_collector.h"
#include "../src/config_parser.h"
#include "../src/log_storage.h"
#include "../src/hash_chain.h"

/**
 * Unit tests for Event Collector
 * Tests event collection, validation, and queuing
 */
class EventCollectorTest : public QObject
{
    Q_OBJECT

public:
    EventCollectorTest();
    ~EventCollectorTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testInitialize();
    void testCollectEvent();
    void testCollectEventInvalid();
    void testEventValidation();
    void testEventQueuing();
    void testIsHealthy();

private:
    EventCollector* m_eventCollector;
    ConfigParser* m_configParser;
    LogStorage* m_logStorage;
    HashChain* m_hashChain;
    QTemporaryDir* m_tempDir;

    void createConfigFile(const QString& content);
    QString createTestEventJson(const QString& eventType);
};

EventCollectorTest::EventCollectorTest()
    : m_eventCollector(nullptr)
    , m_configParser(nullptr)
    , m_logStorage(nullptr)
    , m_hashChain(nullptr)
    , m_tempDir(nullptr)
{
}

EventCollectorTest::~EventCollectorTest()
{
}

void EventCollectorTest::initTestCase()
{
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());
}

void EventCollectorTest::cleanupTestCase()
{
    delete m_tempDir;
}

void EventCollectorTest::init()
{
    m_configParser = new ConfigParser();
    m_logStorage = new LogStorage();
    m_hashChain = new HashChain();
    m_eventCollector = new EventCollector();
}

void EventCollectorTest::cleanup()
{
    if (m_eventCollector) {
        m_eventCollector->stop();
    }
    delete m_eventCollector;
    delete m_hashChain;
    delete m_logStorage;
    delete m_configParser;
}

void EventCollectorTest::createConfigFile(const QString& content)
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
    QVERIFY(m_logStorage->initialize(m_configParser));
    QVERIFY(m_hashChain->initialize(m_configParser, m_logStorage));
}

QString EventCollectorTest::createTestEventJson(const QString& eventType)
{
    QJsonObject event;
    event["event_type"] = eventType;
    event["event_severity"] = "INFO";
    event["application_name"] = "TestApplication";
    event["event_description"] = "Test event";
    event["event_data"] = QJsonObject{{"test", "data"}};
    
    QJsonDocument doc(event);
    return QString::fromUtf8(doc.toJson());
}

void EventCollectorTest::testInitialize()
{
    QString configContent = R"(
service:
  enabled: true
  database_type: sqlite
  sqlite_path: "{{DB_PATH}}"
event_collection:
  max_queue_size: 1000
  batch_size: 10
  flush_interval: 1
)";
    createConfigFile(configContent);
    QVERIFY(m_eventCollector->initialize(m_configParser, m_logStorage, m_hashChain));
}

void EventCollectorTest::testCollectEvent()
{
    QString configContent = R"(
service:
  enabled: true
  database_type: sqlite
  sqlite_path: "{{DB_PATH}}"
event_collection:
  max_queue_size: 1000
  batch_size: 10
  flush_interval: 1
)";
    createConfigFile(configContent);
    QVERIFY(m_eventCollector->initialize(m_configParser, m_logStorage, m_hashChain));
    QVERIFY(m_eventCollector->start());

    QString eventJson = createTestEventJson("ENCRYPTION");
    QVERIFY(m_eventCollector->collectEvent(eventJson.toStdString()));

    // Wait for processing
    QTest::qWait(2000);

    m_eventCollector->stop();
}

void EventCollectorTest::testCollectEventInvalid()
{
    QString configContent = R"(
service:
  enabled: true
  database_type: sqlite
  sqlite_path: "{{DB_PATH}}"
event_collection:
  max_queue_size: 1000
  batch_size: 10
  flush_interval: 1
)";
    createConfigFile(configContent);
    QVERIFY(m_eventCollector->initialize(m_configParser, m_logStorage, m_hashChain));
    QVERIFY(m_eventCollector->start());

    // Invalid JSON
    QVERIFY(!m_eventCollector->collectEvent("invalid json"));

    // Missing required fields
    QJsonObject invalidEvent;
    invalidEvent["event_type"] = "ENCRYPTION";
    // Missing event_severity and application_name
    QJsonDocument doc(invalidEvent);
    QString invalidJson = QString::fromUtf8(doc.toJson());
    QVERIFY(!m_eventCollector->collectEvent(invalidJson.toStdString()));

    m_eventCollector->stop();
}

void EventCollectorTest::testEventValidation()
{
    QString configContent = R"(
service:
  enabled: true
  database_type: sqlite
  sqlite_path: "{{DB_PATH}}"
)";
    createConfigFile(configContent);
    QVERIFY(m_eventCollector->initialize(m_configParser, m_logStorage, m_hashChain));

    // Valid event
    QString validEvent = createTestEventJson("ENCRYPTION");
    // Validation is tested through collectEvent
    QVERIFY(m_eventCollector->collectEvent(validEvent.toStdString()));
}

void EventCollectorTest::testEventQueuing()
{
    QString configContent = R"(
service:
  enabled: true
  database_type: sqlite
  sqlite_path: "{{DB_PATH}}"
event_collection:
  max_queue_size: 10
  batch_size: 5
  flush_interval: 1
)";
    createConfigFile(configContent);
    QVERIFY(m_eventCollector->initialize(m_configParser, m_logStorage, m_hashChain));
    QVERIFY(m_eventCollector->start());

    // Queue multiple events
    for (int i = 0; i < 5; ++i) {
        QString eventJson = createTestEventJson("ENCRYPTION");
        QVERIFY(m_eventCollector->collectEvent(eventJson.toStdString()));
    }

    // Wait for processing
    QTest::qWait(2000);

    m_eventCollector->stop();
}

void EventCollectorTest::testIsHealthy()
{
    QString configContent = R"(
service:
  enabled: true
  database_type: sqlite
  sqlite_path: "{{DB_PATH}}"
event_collection:
  max_queue_size: 1000
  batch_size: 10
  flush_interval: 1
)";
    createConfigFile(configContent);
    QVERIFY(m_eventCollector->initialize(m_configParser, m_logStorage, m_hashChain));
    QVERIFY(m_eventCollector->start());
    QVERIFY(m_eventCollector->isHealthy());
    m_eventCollector->stop();
}

QTEST_MAIN(EventCollectorTest)
#include "test_event_collector.moc"

