#include "radio_manager.h"
#include <QDebug>

RadioManager::RadioManager(QObject* parent)
    : QObject(parent)
{
}

RadioManager::~RadioManager() {
}

bool RadioManager::initialize() {
    return true;
}

bool RadioManager::addProtocol(RadioProtocol* protocol) {
    if (!protocol) {
        return false;
    }
    
    m_protocols.append(protocol);
    
    // Connect signals
    connect(protocol, &RadioProtocol::messageReceived, this, &RadioManager::messageReceived);
    
    return true;
}

bool RadioManager::sendMessage(RadioProtocolType protocolType, const RadioMessage& message) {
    RadioProtocol* protocol = getProtocol(protocolType);
    if (!protocol) {
        return false;
    }
    
    return protocol->sendMessage(message);
}

QList<RadioProtocolType> RadioManager::getAvailableProtocols() const {
    QList<RadioProtocolType> protocols;
    
    for (RadioProtocol* protocol : m_protocols) {
        protocols.append(protocol->protocolType());
    }
    
    return protocols;
}

RadioProtocol* RadioManager::getProtocol(RadioProtocolType type) const {
    for (RadioProtocol* protocol : m_protocols) {
        if (protocol->protocolType() == type) {
            return protocol;
        }
    }
    return nullptr;
}

