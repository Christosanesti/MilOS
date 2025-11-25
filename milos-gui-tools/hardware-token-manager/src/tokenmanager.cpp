#include "tokenmanager.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDBusReply>

TokenManager::TokenManager(QObject* parent)
    : QObject(parent)
    , m_serviceAvailable(false)
    , m_dbusConnection(QDBusConnection::systemBus())
    , m_tokenInterface(nullptr)
{
    connectToService();
    
    // Monitor service registration
    m_dbusConnection.connect(
        "org.freedesktop.DBus",
        "/org/freedesktop/DBus",
        "org.freedesktop.DBus",
        "NameOwnerChanged",
        QStringList() << "org.milos.HardwareToken",
        "sss",
        this,
        SLOT(onServiceRegistered(QString,QString,QString))
    );
    
    refreshTokens();
}

TokenManager::~TokenManager()
{
    disconnectFromService();
    if (m_tokenInterface) {
        delete m_tokenInterface;
    }
}

void TokenManager::connectToService()
{
    if (m_tokenInterface) {
        return;
    }
    
    m_tokenInterface = new QDBusInterface(
        "org.milos.HardwareToken",
        "/org/milos/HardwareToken",
        "org.milos.HardwareToken",
        m_dbusConnection,
        this
    );
    
    if (m_tokenInterface->isValid()) {
        m_serviceAvailable = true;
        emit serviceAvailableChanged(m_serviceAvailable);
    } else {
        m_serviceAvailable = false;
        emit serviceAvailableChanged(m_serviceAvailable);
        qWarning() << "Cannot connect to Hardware Token service:" << m_tokenInterface->lastError().message();
    }
}

void TokenManager::disconnectFromService()
{
    if (m_tokenInterface) {
        delete m_tokenInterface;
        m_tokenInterface = nullptr;
    }
    m_serviceAvailable = false;
    emit serviceAvailableChanged(m_serviceAvailable);
}

void TokenManager::refreshTokens()
{
    if (!m_tokenInterface || !m_tokenInterface->isValid()) {
        return;
    }
    
    QDBusReply<QString> reply = m_tokenInterface->call("ListTokens");
    if (reply.isValid()) {
        QJsonDocument doc = QJsonDocument::fromJson(reply.value().toUtf8());
        if (doc.isArray()) {
            m_tokens.clear();
            QJsonArray array = doc.array();
            for (const QJsonValue& value : array) {
                if (value.isObject()) {
                    QVariantMap token = parseTokenData(QString::fromUtf8(QJsonDocument(value.toObject()).toJson()));
                    m_tokens.append(token);
                }
            }
            emit tokensChanged();
        }
    } else {
        emit error("Failed to refresh tokens: " + reply.error().message());
    }
}

QVariantMap TokenManager::getToken(const QString& tokenId)
{
    for (const QVariant& var : m_tokens) {
        QVariantMap token = var.toMap();
        if (token.value("id").toString() == tokenId) {
            return token;
        }
    }
    return QVariantMap();
}

void TokenManager::configureToken(const QString& tokenId, const QVariantMap& config)
{
    if (!m_tokenInterface || !m_tokenInterface->isValid()) {
        emit error("Hardware Token service not available");
        return;
    }
    
    QJsonDocument doc = QJsonDocument::fromVariant(config);
    QString jsonData = QString::fromUtf8(doc.toJson());
    
    QDBusReply<bool> reply = m_tokenInterface->call("ConfigureToken", tokenId, jsonData);
    if (reply.isValid() && reply.value()) {
        refreshTokens();
        emit tokenConfigured(tokenId);
    } else {
        emit error("Failed to configure token: " + (reply.isValid() ? "Unknown error" : reply.error().message()));
    }
}

void TokenManager::removeToken(const QString& tokenId)
{
    if (!m_tokenInterface || !m_tokenInterface->isValid()) {
        emit error("Hardware Token service not available");
        return;
    }
    
    QDBusReply<bool> reply = m_tokenInterface->call("RemoveToken", tokenId);
    if (reply.isValid() && reply.value()) {
        refreshTokens();
        emit tokenRemoved(tokenId);
    } else {
        emit error("Failed to remove token: " + (reply.isValid() ? "Unknown error" : reply.error().message()));
    }
}

void TokenManager::startEnrollment()
{
    if (!m_tokenInterface || !m_tokenInterface->isValid()) {
        emit error("Hardware Token service not available");
        return;
    }
    
    QDBusReply<QString> reply = m_tokenInterface->call("StartEnrollment");
    if (!reply.isValid()) {
        emit error("Failed to start enrollment: " + reply.error().message());
    }
}

void TokenManager::completeEnrollment(const QVariantMap& enrollmentData)
{
    if (!m_tokenInterface || !m_tokenInterface->isValid()) {
        emit error("Hardware Token service not available");
        return;
    }
    
    QJsonDocument doc = QJsonDocument::fromVariant(enrollmentData);
    QString jsonData = QString::fromUtf8(doc.toJson());
    
    QDBusReply<QString> reply = m_tokenInterface->call("CompleteEnrollment", jsonData);
    if (reply.isValid()) {
        QString tokenId = reply.value();
        refreshTokens();
        emit tokenEnrolled(tokenId);
    } else {
        emit error("Failed to complete enrollment: " + reply.error().message());
    }
}

QVariantMap TokenManager::parseTokenData(const QString& jsonData)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonData.toUtf8());
    if (doc.isObject()) {
        return doc.object().toVariantMap();
    }
    return QVariantMap();
}

void TokenManager::onServiceRegistered(const QString& serviceName, const QString& oldOwner, const QString& newOwner)
{
    Q_UNUSED(oldOwner)
    if (serviceName == "org.milos.HardwareToken" && !newOwner.isEmpty()) {
        connectToService();
        refreshTokens();
    }
}

void TokenManager::onServiceUnregistered(const QString& serviceName, const QString& oldOwner, const QString& newOwner)
{
    Q_UNUSED(oldOwner)
    Q_UNUSED(newOwner)
    if (serviceName == "org.milos.HardwareToken") {
        disconnectFromService();
    }
}

