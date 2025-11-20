#include "forward_secrecy.h"
#include <QUuid>
#include <QDebug>
#include <QCryptographicHash>

ForwardSecrecy::ForwardSecrecy(QObject* parent)
    : QObject(parent)
    , m_keyRotationIntervalSeconds(3600)  // 1 hour default
{
}

ForwardSecrecy::~ForwardSecrecy() {
}

bool ForwardSecrecy::initialize() {
    return true;
}

SessionKeyInfo ForwardSecrecy::generateSessionKey(const QString& participantId) {
    SessionKeyInfo info;
    info.sessionId = generateSessionId();
    info.participantId = participantId;
    info.createdAt = QDateTime::currentDateTime();
    info.expiresAt = calculateExpiration();

    // In production, would use PQC to generate session key
    // For now, generate placeholder key
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(participantId.toUtf8());
    hash.addData(info.sessionId.toUtf8());
    hash.addData(info.createdAt.toString(Qt::ISODate).toUtf8());
    info.sessionKey = hash.result();

    m_sessionKeys[info.sessionId] = info;
    m_participantSessions[participantId] = info.sessionId;

    emit sessionKeyGenerated(info.sessionId);

    return info;
}

QByteArray ForwardSecrecy::getSessionKey(const QString& sessionId) const {
    if (!m_sessionKeys.contains(sessionId)) {
        return QByteArray();
    }

    SessionKeyInfo info = m_sessionKeys.value(sessionId);
    if (QDateTime::currentDateTime() > info.expiresAt) {
        return QByteArray();  // Expired
    }

    return info.sessionKey;
}

SessionKeyInfo ForwardSecrecy::rotateSessionKey(const QString& participantId) {
    QString oldSessionId = m_participantSessions.value(participantId);
    
    SessionKeyInfo newInfo = generateSessionKey(participantId);
    
    if (!oldSessionId.isEmpty()) {
        emit sessionKeyRotated(oldSessionId, newInfo.sessionId);
    }

    return newInfo;
}

void ForwardSecrecy::expireOldSessionKeys() {
    QDateTime now = QDateTime::currentDateTime();
    QList<QString> expiredSessions;

    for (auto it = m_sessionKeys.begin(); it != m_sessionKeys.end(); ++it) {
        if (now > it.value().expiresAt) {
            expiredSessions.append(it.key());
        }
    }

    for (const QString& sessionId : expiredSessions) {
        m_sessionKeys.remove(sessionId);
    }
}

QString ForwardSecrecy::generateSessionId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QDateTime ForwardSecrecy::calculateExpiration() const {
    return QDateTime::currentDateTime().addSecs(m_keyRotationIntervalSeconds);
}

