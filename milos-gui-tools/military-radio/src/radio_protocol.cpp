#include "radio_protocol.h"
#include <QUuid>
#include <QDebug>

RadioProtocol::RadioProtocol(QObject* parent)
    : QObject(parent)
    , m_protocolType(RadioProtocolType::VHF)
    , m_frequency(0.0)
{
}

RadioProtocol::~RadioProtocol() {
}

bool RadioProtocol::initialize(RadioProtocolType protocolType, double frequency) {
    m_protocolType = protocolType;
    m_frequency = frequency;
    
    // In production, would initialize actual radio hardware
    return true;
}

bool RadioProtocol::sendMessage(const RadioMessage& message) {
    // Encrypt if needed
    QByteArray payload = message.payload;
    if (message.encrypted && !m_encryptionKey.isEmpty()) {
        payload = encryptPayload(payload);
    }
    
    // In production, would send via actual radio hardware
    qDebug() << "Sending radio message via" << message.protocol << "to" << message.to;
    
    emit messageSent(message.id);
    
    return true;
}

RadioMessage RadioProtocol::receiveMessage() {
    RadioMessage message;
    
    // In production, would receive from actual radio hardware
    // For now, return empty message
    
    return message;
}

bool RadioProtocol::setEncryptionKey(const QByteArray& key) {
    m_encryptionKey = key;
    return true;
}

QByteArray RadioProtocol::encryptPayload(const QByteArray& payload) const {
    // In production, would use proper encryption
    // For now, simple XOR (not secure, just placeholder)
    QByteArray encrypted = payload;
    for (int i = 0; i < encrypted.size(); ++i) {
        encrypted[i] = encrypted[i] ^ m_encryptionKey[i % m_encryptionKey.size()];
    }
    return encrypted;
}

QByteArray RadioProtocol::decryptPayload(const QByteArray& encryptedPayload) const {
    // XOR is symmetric
    return encryptPayload(encryptedPayload);
}

