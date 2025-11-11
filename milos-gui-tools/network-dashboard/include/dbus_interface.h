#ifndef DBUS_INTERFACE_H
#define DBUS_INTERFACE_H

#include <QObject>
#include <QString>
#include <QStringList>

class PacketCapture;
class PacketStatistics;

/**
 * @brief D-Bus Interface for Network Dashboard
 * 
 * Implements org.milos.NetworkDashboard D-Bus interface.
 */
class DBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.NetworkDashboard")

public:
    DBusInterface(QObject* parent = nullptr);
    ~DBusInterface();

    /**
     * @brief Initialize D-Bus interface
     * @param packetCapture Packet capture instance
     * @param packetStatistics Packet statistics instance
     * @return true if initialization successful, false otherwise
     */
    bool initialize(PacketCapture* packetCapture, PacketStatistics* packetStatistics);

    /**
     * @brief Start D-Bus interface
     * @return true if start successful, false otherwise
     */
    bool start();

    /**
     * @brief Stop D-Bus interface
     */
    void stop();

    /**
     * @brief Check if D-Bus interface is running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return m_running; }

public Q_SLOTS:
    // D-Bus methods
    /**
     * @brief Start packet capture
     * @param interface Network interface (optional)
     * @param filter BPF filter (optional)
     * @return true if start successful, false otherwise
     */
    bool StartCapture(const QString& interface = QString(), const QString& filter = QString());

    /**
     * @brief Stop packet capture
     * @return true if stop successful, false otherwise
     */
    bool StopCapture();

    /**
     * @brief Get packet statistics
     * @return JSON string with statistics
     */
    QString GetPacketStats();

Q_SIGNALS:
    /**
     * @brief Signal emitted when packet is captured
     * @param packetInfo JSON string with packet information
     */
    void PacketCaptured(const QString& packetInfo);

private:
    bool m_running;
    bool m_initialized;
    PacketCapture* m_packetCapture;
    PacketStatistics* m_packetStatistics;

    /**
     * @brief Register D-Bus interface
     */
    bool registerInterface();

    /**
     * @brief Unregister D-Bus interface
     */
    void unregisterInterface();
};

#endif // DBUS_INTERFACE_H

