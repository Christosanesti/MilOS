#ifndef TOKENMANAGER_H
#define TOKENMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QVariantList>
#include <QDBusInterface>
#include <QDBusConnection>

/**
 * @brief Hardware Token Manager
 * 
 * Manages hardware tokens via D-Bus interface
 */
class TokenManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList tokens READ tokens NOTIFY tokensChanged)
    Q_PROPERTY(bool serviceAvailable READ serviceAvailable NOTIFY serviceAvailableChanged)

public:
    explicit TokenManager(QObject* parent = nullptr);
    ~TokenManager();

    QVariantList tokens() const { return m_tokens; }
    bool serviceAvailable() const { return m_serviceAvailable; }

    Q_INVOKABLE void refreshTokens();
    Q_INVOKABLE QVariantMap getToken(const QString& tokenId);
    Q_INVOKABLE void configureToken(const QString& tokenId, const QVariantMap& config);
    Q_INVOKABLE void removeToken(const QString& tokenId);
    Q_INVOKABLE void startEnrollment();
    Q_INVOKABLE void completeEnrollment(const QVariantMap& enrollmentData);

signals:
    void tokensChanged();
    void serviceAvailableChanged();
    void tokenEnrolled(const QString& tokenId);
    void tokenRemoved(const QString& tokenId);
    void tokenConfigured(const QString& tokenId);
    void error(const QString& errorMessage);

private slots:
    void onServiceRegistered(const QString& serviceName, const QString& oldOwner, const QString& newOwner);
    void onServiceUnregistered(const QString& serviceName, const QString& oldOwner, const QString& newOwner);

private:
    void connectToService();
    void disconnectFromService();
    QVariantMap parseTokenData(const QString& jsonData);

    QVariantList m_tokens;
    bool m_serviceAvailable;
    QDBusConnection m_dbusConnection;
    QDBusInterface* m_tokenInterface;
};

#endif // TOKENMANAGER_H

