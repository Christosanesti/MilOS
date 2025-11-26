#ifndef ANONYMITYCONTROLLER_H
#define ANONYMITYCONTROLLER_H

#include <QObject>
#include <QString>
#include <QDBusConnection>
#include <QDBusInterface>

/**
 * @brief Anonymity Controller
 * 
 * Manages Tor/I2P connection state and toggling
 */
class AnonymityController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool torConnected READ torConnected NOTIFY torStatusChanged)
    Q_PROPERTY(bool i2pConnected READ i2pConnected NOTIFY i2pStatusChanged)
    Q_PROPERTY(QString connectionStatus READ connectionStatus NOTIFY statusChanged)

public:
    explicit AnonymityController(QObject* parent = nullptr);
    
    bool torConnected() const { return m_torConnected; }
    bool i2pConnected() const { return m_i2pConnected; }
    QString connectionStatus() const;

    Q_INVOKABLE void toggleTor();
    Q_INVOKABLE void toggleI2P();
    Q_INVOKABLE void updateStatus();

signals:
    void torStatusChanged(bool connected);
    void i2pStatusChanged(bool connected);
    void statusChanged();

private:
    bool m_torConnected;
    bool m_i2pConnected;
    QDBusConnection m_dbusConnection;
};

#endif // ANONYMITYCONTROLLER_H

