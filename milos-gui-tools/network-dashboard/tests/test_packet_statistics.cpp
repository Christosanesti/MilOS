#include <QtTest>
#include <QObject>
#include <QString>
#include "../include/packet_statistics.h"
#include "../include/packet_capture.h"

/**
 * Unit tests for Packet Statistics
 */
class PacketStatisticsTest : public QObject
{
    Q_OBJECT

public:
    PacketStatisticsTest();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testInitialize();
    void testUpdateStatistics();
    void testGetTotalPackets();
    void testGetTotalBytes();
    void testGetProtocolStatistics();
    void testReset();

private:
    PacketStatistics* m_statistics;
};

PacketStatisticsTest::PacketStatisticsTest()
    : m_statistics(nullptr)
{
}

void PacketStatisticsTest::initTestCase()
{
    m_statistics = new PacketStatistics();
}

void PacketStatisticsTest::cleanupTestCase()
{
    delete m_statistics;
}

void PacketStatisticsTest::testInitialize()
{
    m_statistics->initialize();
    QCOMPARE(m_statistics->getTotalPackets(), static_cast<uint64_t>(0));
    QCOMPARE(m_statistics->getTotalBytes(), static_cast<uint64_t>(0));
}

void PacketStatisticsTest::testUpdateStatistics()
{
    m_statistics->reset();

    PacketData packet;
    packet.protocol = "TCP";
    packet.length = 100;
    
    m_statistics->updateStatistics(packet);
    QCOMPARE(m_statistics->getTotalPackets(), static_cast<uint64_t>(1));
    QCOMPARE(m_statistics->getTotalBytes(), static_cast<uint64_t>(100));
}

void PacketStatisticsTest::testGetTotalPackets()
{
    m_statistics->reset();

    PacketData packet1;
    packet1.protocol = "TCP";
    packet1.length = 100;
    
    PacketData packet2;
    packet2.protocol = "UDP";
    packet2.length = 200;

    m_statistics->updateStatistics(packet1);
    m_statistics->updateStatistics(packet2);
    
    QCOMPARE(m_statistics->getTotalPackets(), static_cast<uint64_t>(2));
}

void PacketStatisticsTest::testGetTotalBytes()
{
    m_statistics->reset();

    PacketData packet1;
    packet1.length = 100;
    
    PacketData packet2;
    packet2.length = 200;

    m_statistics->updateStatistics(packet1);
    m_statistics->updateStatistics(packet2);
    
    QCOMPARE(m_statistics->getTotalBytes(), static_cast<uint64_t>(300));
}

void PacketStatisticsTest::testGetProtocolStatistics()
{
    m_statistics->reset();

    PacketData tcpPacket;
    tcpPacket.protocol = "TCP";
    tcpPacket.length = 100;

    PacketData udpPacket;
    udpPacket.protocol = "UDP";
    udpPacket.length = 200;

    m_statistics->updateStatistics(tcpPacket);
    m_statistics->updateStatistics(udpPacket);
    m_statistics->updateStatistics(tcpPacket);

    auto stats = m_statistics->getProtocolStatistics();
    QVERIFY(stats.find("TCP") != stats.end());
    QVERIFY(stats.find("UDP") != stats.end());
    
    QCOMPARE(stats["TCP"].packet_count, static_cast<uint64_t>(2));
    QCOMPARE(stats["UDP"].packet_count, static_cast<uint64_t>(1));
}

void PacketStatisticsTest::testReset()
{
    PacketData packet;
    packet.length = 100;
    m_statistics->updateStatistics(packet);
    
    m_statistics->reset();
    QCOMPARE(m_statistics->getTotalPackets(), static_cast<uint64_t>(0));
    QCOMPARE(m_statistics->getTotalBytes(), static_cast<uint64_t>(0));
}

QTEST_MAIN(PacketStatisticsTest)
#include "test_packet_statistics.moc"

