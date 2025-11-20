#include "key_exchange.h"
#include <QUuid>
#include <QDebug>
#include <QCryptographicHash>

KeyExchange::KeyExchange(QObject* parent)
    : QObject(parent)
{
}

KeyExchange::~KeyExchange() {
}

bool KeyExchange::initialize() {
    if (!generateKeyPair()) {
        return false;
    }
    return true;
}

QString KeyExchange::initiateKeyExchange(const QString& participantId) {
    KeyExchangeInfo info;
    info.exchangeId = generateExchangeId();
    info.participantId = participantId;
    info.publicKey = m_localPublicKey;
    info.status = KeyExchangeStatus::Pending;
    info.initiatedAt = QDateTime::currentDateTime();

    m_exchanges[info.exchangeId] = info;

    emit keyExchangeInitiated(info.exchangeId);

    return info.exchangeId;
}

bool KeyExchange::completeKeyExchange(const QString& exchangeId, const QByteArray& publicKey) {
    if (!m_exchanges.contains(exchangeId)) {
        emit keyExchangeFailed(exchangeId, "Exchange not found");
        return false;
    }

    KeyExchangeInfo& info = m_exchanges[exchangeId];
    
    if (info.status != KeyExchangeStatus::Pending) {
        emit keyExchangeFailed(exchangeId, "Exchange already completed or failed");
        return false;
    }

    // Validate public key
    if (publicKey.isEmpty()) {
        info.status = KeyExchangeStatus::Failed;
        emit keyExchangeFailed(exchangeId, "Invalid public key");
        return false;
    }

    info.publicKey = publicKey;
    info.status = KeyExchangeStatus::Completed;
    info.completedAt = QDateTime::currentDateTime();

    emit keyExchangeCompleted(exchangeId);

    return true;
}

KeyExchangeInfo KeyExchange::getExchangeInfo(const QString& exchangeId) const {
    return m_exchanges.value(exchangeId, KeyExchangeInfo());
}

bool KeyExchange::validateKeyExchange(const QString& exchangeId) const {
    if (!m_exchanges.contains(exchangeId)) {
        return false;
    }

    KeyExchangeInfo info = m_exchanges.value(exchangeId);
    return info.status == KeyExchangeStatus::Completed;
}

QString KeyExchange::generateExchangeId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

bool KeyExchange::generateKeyPair() {
    // In production, would use PQC Abstraction Layer to generate key pair
    // For now, generate placeholder keys
    m_localPrivateKey = QUuid::createUuid().toByteArray();
    m_localPublicKey = QUuid::createUuid().toByteArray();
    return true;
}

