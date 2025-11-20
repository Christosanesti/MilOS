#include "secure_messenger.h"
#include "mesh_network.h"
#include "peer_discovery.h"
#include "network_manager.h"
#include "network_health.h"
#include "ethernet_enforcement.h"
#include <QDebug>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QAbstractSocket>

SecureMessenger::SecureMessenger(QObject* parent)
    : QObject(parent)
    , m_usbAuth(new USBAuthorization(this))
    , m_keyGen(new KeyGenerator(this))
    , m_keyMgr(new KeyManager(this))
    , m_enrollment(new UserEnrollment(this))
    , m_roleMgr(new RoleManager(this))
    , m_meshNetwork(new MeshNetwork(this))
    , m_peerDiscovery(new PeerDiscovery(this))
    , m_networkManager(new NetworkManager(this))
    , m_healthMonitor(new NetworkHealthMonitor(this))
    , m_enforcement(new EthernetEnforcement(this))
    , m_dbusInterface(new SecureMessengerDBusInterface(this))
    , m_auditLogger(new AuditLogger(this))
    , m_initialized(false)
    , m_running(false)
{
}

SecureMessenger::~SecureMessenger() {
    stop();
}

bool SecureMessenger::initialize() {
    if (m_initialized) {
        return true;
    }

    // Initialize components
    if (!m_usbAuth->initialize()) {
        qWarning() << "Failed to initialize USB authorization";
        return false;
    }

    if (!m_keyGen->initialize()) {
        qWarning() << "Failed to initialize key generator";
        return false;
    }

    if (!m_keyMgr->initialize()) {
        qWarning() << "Failed to initialize key manager";
        return false;
    }

    if (!m_enrollment->initialize()) {
        qWarning() << "Failed to initialize user enrollment";
        return false;
    }

    if (!m_roleMgr->initialize()) {
        qWarning() << "Failed to initialize role manager";
        return false;
    }

    if (!m_auditLogger->initialize()) {
        qWarning() << "Failed to initialize audit logger";
        return false;
    }

    // Initialize network components
    if (!m_networkManager->initialize()) {
        qWarning() << "Failed to initialize network manager";
        return false;
    }

    if (!m_enforcement->initialize()) {
        qWarning() << "Failed to initialize ethernet enforcement";
        return false;
    }

    // Get local network address
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    QHostAddress localAddress;
    for (const QNetworkInterface& iface : interfaces) {
        if (iface.isUp() && !iface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            QList<QNetworkAddressEntry> entries = iface.addressEntries();
            for (const QNetworkAddressEntry& entry : entries) {
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    localAddress = entry.ip();
                    break;
                }
            }
            if (!localAddress.isNull()) {
                break;
            }
        }
    }

    if (localAddress.isNull()) {
        localAddress = QHostAddress::LocalHost;
    }

    if (!m_meshNetwork->initialize(localAddress, 8888)) {
        qWarning() << "Failed to initialize mesh network";
        return false;
    }

    if (!m_peerDiscovery->initialize("", 8889)) {
        qWarning() << "Failed to initialize peer discovery";
        return false;
    }

    if (!m_healthMonitor->initialize()) {
        qWarning() << "Failed to initialize network health monitor";
        return false;
    }

    // Connect signals for audit logging
    connect(m_keyGen, &KeyGenerator::keyGenerated, this, [this](const QString& keyId) {
        QVariantMap eventData;
        eventData["key_id"] = keyId;
        m_auditLogger->logKeyOperation("key_generated", QString(), eventData);
    });

    connect(m_keyGen, &KeyGenerator::keyWrittenToUSB, this, [this](const QString& keyId, const QString& devicePath) {
        QVariantMap eventData;
        eventData["key_id"] = keyId;
        eventData["device_path"] = devicePath;
        m_auditLogger->logKeyOperation("key_written_to_usb", QString(), eventData);
    });

    connect(m_keyMgr, &KeyManager::keyDistributed, this, [this](const QString& keyId, const QString& devicePath) {
        QVariantMap eventData;
        eventData["key_id"] = keyId;
        eventData["device_path"] = devicePath;
        m_auditLogger->logKeyOperation("key_distributed", QString(), eventData);
    });

    connect(m_keyMgr, &KeyManager::keyRevoked, this, [this](const QString& keyId) {
        QVariantMap eventData;
        eventData["key_id"] = keyId;
        m_auditLogger->logKeyOperation("key_revoked", QString(), eventData);
    });

    connect(m_enrollment, &UserEnrollment::userEnrolled, this, [this](const QString& userId) {
        QVariantMap eventData;
        eventData["user_id"] = userId;
        m_auditLogger->logKeyOperation("user_enrolled", userId, eventData);
    });

    connect(m_enrollment, &UserEnrollment::enrollmentApproved, this, [this](const QString& userId) {
        QVariantMap eventData;
        eventData["user_id"] = userId;
        m_auditLogger->logKeyOperation("enrollment_approved", userId, eventData);
    });

    connect(m_enrollment, &UserEnrollment::enrollmentRejected, this, [this](const QString& userId, const QString& reason) {
        QVariantMap eventData;
        eventData["user_id"] = userId;
        eventData["reason"] = reason;
        m_auditLogger->logKeyOperation("enrollment_rejected", userId, eventData);
    });

    connect(m_meshNetwork, &MeshNetwork::peerAdded, this, [this](const QString& peerId) {
        QVariantMap eventData;
        eventData["peer_id"] = peerId;
        m_auditLogger->logKeyOperation("peer_added", QString(), eventData);
    });

    connect(m_meshNetwork, &MeshNetwork::peerRemoved, this, [this](const QString& peerId) {
        QVariantMap eventData;
        eventData["peer_id"] = peerId;
        m_auditLogger->logKeyOperation("peer_removed", QString(), eventData);
    });

    connect(m_peerDiscovery, &PeerDiscovery::peerDiscovered, this, [this](const QString& peerId, const QHostAddress& address) {
        QVariantMap eventData;
        eventData["peer_id"] = peerId;
        eventData["address"] = address.toString();
        m_auditLogger->logKeyOperation("peer_discovered", QString(), eventData);
    });

    connect(m_enforcement, &EthernetEnforcement::unauthorizedInterfaceDetected, this, [this](const QString& interfaceName) {
        QVariantMap eventData;
        eventData["interface_name"] = interfaceName;
        m_auditLogger->logKeyOperation("unauthorized_interface_detected", QString(), eventData);
    });

    // Initialize D-Bus interface
    m_dbusInterface->setUSBAuthorization(m_usbAuth);
    m_dbusInterface->setKeyGenerator(m_keyGen);
    m_dbusInterface->setKeyManager(m_keyMgr);
    m_dbusInterface->setUserEnrollment(m_enrollment);
    m_dbusInterface->setRoleManager(m_roleMgr);
    m_dbusInterface->setMeshNetwork(m_meshNetwork);
    m_dbusInterface->setPeerDiscovery(m_peerDiscovery);
    m_dbusInterface->setNetworkManager(m_networkManager);
    m_dbusInterface->setNetworkHealthMonitor(m_healthMonitor);
    m_dbusInterface->setEthernetEnforcement(m_enforcement);

    if (!m_dbusInterface->initialize()) {
        qWarning() << "Failed to initialize D-Bus interface";
        return false;
    }

    m_initialized = true;
    return true;
}

bool SecureMessenger::start() {
    if (!m_initialized) {
        if (!initialize()) {
            return false;
        }
    }

    if (m_running) {
        return true;
    }

    // Start network components
    if (!m_peerDiscovery->start()) {
        qWarning() << "Failed to start peer discovery";
        return false;
    }

    if (!m_healthMonitor->start()) {
        qWarning() << "Failed to start network health monitor";
        return false;
    }

    // Enforce ethernet-only operation
    m_enforcement->blockUnauthorizedInterfaces();
    m_enforcement->blockInternetConnectivity();

    m_running = true;
    return true;
}

void SecureMessenger::stop() {
    if (!m_running) {
        return;
    }

    m_running = false;
}

