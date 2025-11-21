#include "encrypted_email_system.h"
#include <QDebug>
#include <QStandardPaths>
#include <QDir>

EncryptedEmailSystem::EncryptedEmailSystem(QObject* parent)
    : QObject(parent)
    , m_emailClient(new EmailClient(this))
    , m_emailStorage(new EmailStorage(this))
    , m_dbusInterface(new EncryptedEmailDBusInterface(this))
    , m_auditLogger(new AuditLogger(this))
    , m_initialized(false)
{
}

EncryptedEmailSystem::~EncryptedEmailSystem() {
}

bool EncryptedEmailSystem::initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Initialize audit logger
    if (!m_auditLogger->initialize()) {
        qWarning() << "Failed to initialize audit logger";
        return false;
    }
    
    // Initialize email client
    if (!m_emailClient->initialize()) {
        qWarning() << "Failed to initialize email client";
        return false;
    }
    
    // Initialize email storage
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dbPath);
    dbPath += "/emails.db";
    
    if (!m_emailStorage->initialize(dbPath)) {
        qWarning() << "Failed to initialize email storage";
        return false;
    }
    
    // Connect signals for audit logging
    connect(m_emailClient, &EmailClient::emailSent, this, [this](const QString& messageId) {
        EmailMessage message = m_emailClient->getEmail(messageId);
        m_auditLogger->logEmailSent(messageId, message.to.join(", "));
        
        // Store encrypted email
        QByteArray encrypted = m_emailClient->encryptEmail(message);
        m_emailStorage->storeEmail(message, encrypted);
    });
    
    connect(m_emailClient, &EmailClient::emailReceived, this, [this](const EmailMessage& message) {
        m_auditLogger->logEmailReceived(message.id, message.from);
        
        // Store encrypted email
        QByteArray encrypted = m_emailClient->encryptEmail(message);
        m_emailStorage->storeEmail(message, encrypted);
    });
    
    // Initialize D-Bus interface
    m_dbusInterface->setEmailClient(m_emailClient);
    
    if (!m_dbusInterface->initialize()) {
        qWarning() << "Failed to initialize D-Bus interface";
        return false;
    }
    
    m_initialized = true;
    return true;
}

