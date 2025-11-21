#include "dbus_interface.h"
#include "military_radio_system.h"
#include "radio_protocol.h"
#include <QDebug>
#include <QDBusConnection>
#include <QUuid>

MilitaryRadioDBusInterface::MilitaryRadioDBusInterface(QObject* parent)
    : QObject(parent)
    , m_system(nullptr)
{
}

MilitaryRadioDBusInterface::~MilitaryRadioDBusInterface() {
}

bool MilitaryRadioDBusInterface::initialize() {
    if (!QDBusConnection::sessionBus().registerService("org.milos.MilitaryRadio")) {
        qWarning() << "Failed to register D-Bus service";
        return false;
    }
    
    if (!QDBusConnection::sessionBus().registerObject("/org/milos/MilitaryRadio", this,
                                                        QDBusConnection::ExportAllSlots |
                                                        QDBusConnection::ExportAllSignals)) {
        qWarning() << "Failed to register D-Bus object";
        return false;
    }
    
    return true;
}

void MilitaryRadioDBusInterface::setSystem(MilitaryRadioSystem* system) {
    m_system = system;
}

bool MilitaryRadioDBusInterface::InitializeProtocol(int protocolType, double frequency) {
    if (!m_system || !m_system->getRadioManager()) {
        return false;
    }
    
    RadioProtocol* protocol = new RadioProtocol();
    if (!protocol->initialize(static_cast<RadioProtocolType>(protocolType), frequency)) {
        delete protocol;
        return false;
    }
    
    return m_system->getRadioManager()->addProtocol(protocol);
}

bool MilitaryRadioDBusInterface::SendMessage(int protocolType, const QString& to, const QByteArray& payload, int priority) {
    if (!m_system || !m_system->getRadioManager()) {
        return false;
    }
    
    RadioMessage message;
    message.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    message.to = to;
    message.payload = payload;
    message.priority = priority;
    message.timestamp = QDateTime::currentDateTime();
    message.encrypted = true;
    
    QString protocolName;
    switch (static_cast<RadioProtocolType>(protocolType)) {
        case RadioProtocolType::SINCGARS: protocolName = "SINCGARS"; break;
        case RadioProtocolType::HAVE_QUICK: protocolName = "HAVE_QUICK"; break;
        case RadioProtocolType::Link16: protocolName = "Link16"; break;
        default: protocolName = "VHF"; break;
    }
    message.protocol = protocolName;
    
    return m_system->getRadioManager()->sendMessage(static_cast<RadioProtocolType>(protocolType), message);
}

QVariantMap MilitaryRadioDBusInterface::ReceiveMessage(int protocolType) {
    QVariantMap result;
    
    if (!m_system || !m_system->getRadioManager()) {
        return result;
    }
    
    // In production, would receive actual message
    // For now, return empty result
    
    return result;
}

bool MilitaryRadioDBusInterface::SetEncryptionKey(const QByteArray& key) {
    if (!m_system || !m_system->getRadioEncryption()) {
        return false;
    }
    
    // Set key on all protocols
    // In production, would manage keys per protocol
    
    return true;
}

QStringList MilitaryRadioDBusInterface::GetAvailableProtocols() {
    QStringList result;
    
    if (!m_system || !m_system->getRadioManager()) {
        return result;
    }
    
    QList<RadioProtocolType> protocols = m_system->getRadioManager()->getAvailableProtocols();
    for (RadioProtocolType type : protocols) {
        result.append(QString::number(static_cast<int>(type)));
    }
    
    return result;
}

