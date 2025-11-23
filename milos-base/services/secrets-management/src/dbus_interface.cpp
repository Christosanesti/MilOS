#include "dbus_interface.h"
#include "secrets_service.h"
#include "secret_storage.h"
#include "secret_rotation.h"
#include "secret_versioning.h"
#include "access_control.h"
#include "application_integration.h"
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QCoreApplication>
#include <QByteArray>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariant>

DBusInterface::DBusInterface(QObject* parent)
    : QObject(parent)
    , m_secretsService(nullptr)
    , m_secretStorage(nullptr)
    , m_secretRotation(nullptr)
    , m_secretVersioning(nullptr)
    , m_accessControl(nullptr)
    , m_applicationIntegration(nullptr)
    , m_dbusConnection(QDBusConnection::systemBus())
    , m_registered(false)
{
}

DBusInterface::~DBusInterface() {
    if (m_registered) {
        m_dbusConnection.unregisterObject("/org/milos/SecretsManagement");
        m_dbusConnection.unregisterService("org.milos.SecretsManagement");
    }
}

bool DBusInterface::initialize() {
    if (m_registered) {
        return true;
    }

    // Register service
    if (!m_dbusConnection.registerService("org.milos.SecretsManagement")) {
        qWarning() << "Failed to register D-Bus service";
        return false;
    }

    // Register object
    if (!m_dbusConnection.registerObject("/org/milos/SecretsManagement", this,
                                         QDBusConnection::ExportAllSlots |
                                         QDBusConnection::ExportAllSignals)) {
        qWarning() << "Failed to register D-Bus object";
        m_dbusConnection.unregisterService("org.milos.SecretsManagement");
        return false;
    }

    m_registered = true;
    qDebug() << "D-Bus interface registered: org.milos.SecretsManagement";
    return true;
}

bool DBusInterface::StoreSecret(const QString& secretId, const QString& secretData,
                                const QString& secretType, const QString& description,
                                const QString& expiresAt) {
    if (!m_secretStorage) {
        return false;
    }

    // Check access control
    QString userId = connection().interface()->service();
    if (m_accessControl && !m_accessControl->hasPermission(userId.toStdString(), secretId.toStdString(), Permission::Write)) {
        m_accessControl->logAccess(userId.toStdString(), secretId.toStdString(), Permission::Write, false);
        return false;
    }

    // Decode base64 secret data
    QByteArray decodedData = QByteArray::fromBase64(secretData.toUtf8());
    std::vector<uint8_t> secretBytes(decodedData.begin(), decodedData.end());

    // Create metadata
    SecretMetadata metadata;
    metadata.secret_id = secretId.toStdString();
    metadata.secret_type = secretType.toStdString();
    metadata.description = description.toStdString();
    metadata.expires_at = expiresAt.toStdString();
    metadata.owner = userId.toStdString();
    metadata.is_encrypted = true;

    bool result = m_secretStorage->storeSecret(secretId.toStdString(), secretBytes, metadata);
    
    if (result && m_accessControl) {
        m_accessControl->logAccess(userId.toStdString(), secretId.toStdString(), Permission::Write, true);
    }

    return result;
}

QString DBusInterface::GetSecret(const QString& secretId, const QString& version) {
    if (!m_secretStorage) {
        return QString();
    }

    // Check access control
    QString userId = connection().interface()->service();
    if (m_accessControl && !m_accessControl->hasPermission(userId.toStdString(), secretId.toStdString(), Permission::Read)) {
        m_accessControl->logAccess(userId.toStdString(), secretId.toStdString(), Permission::Read, false);
        return QString();
    }

    std::vector<uint8_t> secretData;
    
    if (!version.isEmpty() && m_secretVersioning) {
        // Get specific version
        SecretVersion secretVersion = m_secretVersioning->getVersion(secretId.toStdString(), version.toStdString());
        if (!secretVersion.version_id.empty()) {
            secretData = m_secretStorage->decryptSecret(secretVersion.encrypted_data);
        }
    } else {
        // Get latest version
        secretData = m_secretStorage->getSecret(secretId.toStdString());
    }

    if (secretData.empty()) {
        return QString();
    }

    // Encode to base64
    QByteArray encodedData(reinterpret_cast<const char*>(secretData.data()), secretData.size());
    QString base64Data = QString::fromUtf8(encodedData.toBase64());

    if (m_accessControl) {
        m_accessControl->logAccess(userId.toStdString(), secretId.toStdString(), Permission::Read, true);
    }

    return base64Data;
}

bool DBusInterface::RotateSecret(const QString& secretId) {
    if (!m_secretRotation) {
        return false;
    }

    // Check access control
    QString userId = connection().interface()->service();
    if (m_accessControl && !m_accessControl->hasPermission(userId.toStdString(), secretId.toStdString(), Permission::Rotate)) {
        m_accessControl->logAccess(userId.toStdString(), secretId.toStdString(), Permission::Rotate, false);
        return false;
    }

    bool result = m_secretRotation->rotateSecret(secretId.toStdString());
    
    if (result) {
        if (m_accessControl) {
            m_accessControl->logAccess(userId.toStdString(), secretId.toStdString(), Permission::Rotate, true);
        }
        emit SecretRotated(secretId);
    }

    return result;
}

QStringList DBusInterface::ListSecrets() {
    if (!m_secretStorage) {
        return QStringList();
    }

    std::vector<std::string> secretIds = m_secretStorage->listSecrets();
    QStringList result;
    for (const auto& id : secretIds) {
        result << QString::fromStdString(id);
    }
    return result;
}

QVariantMap DBusInterface::GetSecretMetadata(const QString& secretId) {
    QVariantMap metadata;
    
    if (!m_secretStorage) {
        return metadata;
    }

    SecretMetadata secretMetadata = m_secretStorage->getSecretMetadata(secretId.toStdString());
    if (secretMetadata.secret_id.empty()) {
        return metadata;
    }

    metadata["secret_id"] = QString::fromStdString(secretMetadata.secret_id);
    metadata["secret_type"] = QString::fromStdString(secretMetadata.secret_type);
    metadata["description"] = QString::fromStdString(secretMetadata.description);
    metadata["created_at"] = QString::fromStdString(secretMetadata.created_at);
    metadata["updated_at"] = QString::fromStdString(secretMetadata.updated_at);
    metadata["expires_at"] = QString::fromStdString(secretMetadata.expires_at);
    metadata["owner"] = QString::fromStdString(secretMetadata.owner);
    metadata["is_encrypted"] = secretMetadata.is_encrypted;

    QStringList tags;
    for (const auto& tag : secretMetadata.tags) {
        tags << QString::fromStdString(tag);
    }
    metadata["tags"] = tags;

    return metadata;
}

bool DBusInterface::RollbackSecret(const QString& secretId, const QString& versionId) {
    if (!m_secretVersioning) {
        return false;
    }

    // Check access control
    QString userId = connection().interface()->service();
    if (m_accessControl && !m_accessControl->hasPermission(userId.toStdString(), secretId.toStdString(), Permission::Write)) {
        m_accessControl->logAccess(userId.toStdString(), secretId.toStdString(), Permission::Write, false);
        return false;
    }

    bool result = m_secretVersioning->rollbackToVersion(secretId.toStdString(), versionId.toStdString());
    
    if (result && m_accessControl) {
        m_accessControl->logAccess(userId.toStdString(), secretId.toStdString(), Permission::Write, true);
    }

    return result;
}

bool DBusInterface::DeleteSecret(const QString& secretId) {
    if (!m_secretStorage) {
        return false;
    }

    // Check access control
    QString userId = connection().interface()->service();
    if (m_accessControl && !m_accessControl->hasPermission(userId.toStdString(), secretId.toStdString(), Permission::Delete)) {
        m_accessControl->logAccess(userId.toStdString(), secretId.toStdString(), Permission::Delete, false);
        return false;
    }

    bool result = m_secretStorage->deleteSecret(secretId.toStdString());
    
    if (result && m_accessControl) {
        m_accessControl->logAccess(userId.toStdString(), secretId.toStdString(), Permission::Delete, true);
    }

    return result;
}

QString DBusInterface::AuthenticateApplication(const QString& applicationId,
                                               const QString& dbusServiceName,
                                               const QString& certificateHash) {
    QJsonObject result;

    if (!m_applicationIntegration) {
        result["success"] = false;
        result["error"] = "Application integration not initialized";
        return QJsonDocument(result).toJson();
    }

    std::string appId = applicationId.toStdString();
    std::string dbusService = dbusServiceName.toStdString();
    std::string certHash = certificateHash.toStdString();

    auto identity = m_applicationIntegration->authenticateApplication(appId, dbusService, certHash);

    result["success"] = identity.is_authenticated;
    result["application_id"] = QString::fromStdString(identity.application_id);
    result["application_name"] = QString::fromStdString(identity.application_name);
    result["is_authenticated"] = identity.is_authenticated;
    result["user_id"] = QString::fromStdString(identity.user_id);

    return QJsonDocument(result).toJson();
}

QString DBusInterface::GetSecretForApplication(const QString& applicationId,
                                               const QString& secretId,
                                               const QString& version) {
    if (!m_applicationIntegration) {
        return QString();
    }

    std::string appId = applicationId.toStdString();
    std::string secId = secretId.toStdString();
    std::string ver = version.toStdString();

    std::vector<uint8_t> secretData = m_applicationIntegration->getSecretForApplication(appId, secId, ver);

    if (secretData.empty()) {
        return QString();
    }

    // Encode to base64
    QByteArray encodedData(reinterpret_cast<const char*>(secretData.data()), secretData.size());
    return QString::fromUtf8(encodedData.toBase64());
}

QString DBusInterface::InjectSecret(const QString& applicationId,
                                   const QString& secretId,
                                   const QString& injectionMethod,
                                   const QString& targetPath) {
    QJsonObject result;

    if (!m_applicationIntegration) {
        result["success"] = false;
        result["error"] = "Application integration not initialized";
        return QJsonDocument(result).toJson();
    }

    std::string appId = applicationId.toStdString();
    std::string secId = secretId.toStdString();
    std::string method = injectionMethod.toStdString();
    std::string target = targetPath.toStdString();

    auto injectionRequest = m_applicationIntegration->injectSecret(appId, secId, method, target);

    result["success"] = (injectionRequest.status == "injected");
    result["request_id"] = QString::fromStdString(injectionRequest.request_id);
    result["status"] = QString::fromStdString(injectionRequest.status);
    result["injected_at"] = QString::fromStdString(injectionRequest.injected_at);
    result["error_message"] = QString::fromStdString(injectionRequest.error_message);

    return QJsonDocument(result).toJson();
}

bool DBusInterface::RefreshCachedSecret(const QString& applicationId, const QString& secretId) {
    if (!m_applicationIntegration) {
        return false;
    }

    return m_applicationIntegration->refreshCachedSecret(applicationId.toStdString(), secretId.toStdString());
}

bool DBusInterface::RegisterApplication(const QString& applicationId,
                                       const QString& applicationName,
                                       const QString& dbusServiceName) {
    if (!m_applicationIntegration) {
        return false;
    }

    return m_applicationIntegration->registerApplication(
        applicationId.toStdString(),
        applicationName.toStdString(),
        dbusServiceName.toStdString());
}

