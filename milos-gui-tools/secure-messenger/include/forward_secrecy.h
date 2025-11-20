#ifndef FORWARD_SECRECY_H
#define FORWARD_SECRECY_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QDateTime>
#include <QMap>

/**
 * @brief Session Key Info
 */
struct SessionKeyInfo {
    QString sessionId;
    QByteArray sessionKey;
    QDateTime createdAt;
    QDateTime expiresAt;
    QString participantId;
};

/**
 * @brief Forward Secrecy
 * 
 * Implements forward secrecy with key rotation and session key management.
 */
class ForwardSecrecy : public QObject {
    Q_OBJECT

public:
    explicit ForwardSecrecy(QObject* parent = nullptr);
    ~ForwardSecrecy();

    /**
     * @brief Initialize forward secrecy
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Generate session key
     * @param participantId Participant ID
     * @return Session key info or empty if failed
     */
    SessionKeyInfo generateSessionKey(const QString& participantId);

    /**
     * @brief Get session key
     * @param sessionId Session ID
     * @return Session key or empty if not found/expired
     */
    QByteArray getSessionKey(const QString& sessionId) const;

    /**
     * @brief Rotate session key
     * @param participantId Participant ID
     * @return New session key info or empty if failed
     */
    SessionKeyInfo rotateSessionKey(const QString& participantId);

    /**
     * @brief Expire old session keys
     */
    void expireOldSessionKeys();

Q_SIGNALS:
    /**
     * @brief Emitted when session key is generated
     */
    void sessionKeyGenerated(const QString& sessionId);

    /**
     * @brief Emitted when session key is rotated
     */
    void sessionKeyRotated(const QString& oldSessionId, const QString& newSessionId);

private:
    QMap<QString, SessionKeyInfo> m_sessionKeys;  // Session ID -> Session Key Info
    QMap<QString, QString> m_participantSessions;  // Participant ID -> Current Session ID
    int m_keyRotationIntervalSeconds;
    
    QString generateSessionId() const;
    QDateTime calculateExpiration() const;
};

#endif // FORWARD_SECRECY_H

