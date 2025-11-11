#include "milos/pqc/dbus_interface.h"
#include "milos/pqc/algorithm_factory.h"
#include "milos/pqc/algorithm_registry.h"
#include "milos/pqc/hybrid_encryption.h"
#include "milos/pqc/fallback_manager.h"
#include <QDBusConnection>
#include <QDBusError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>
#include <iostream>

DBusInterface::DBusInterface(QObject* parent)
    : QObject(parent)
    , m_running(false)
    , m_initialized(false)
    , m_algorithmFactory(nullptr)
    , m_algorithmRegistry(nullptr)
    , m_hybridEncryption(nullptr)
    , m_fallbackManager(nullptr)
{
}

DBusInterface::~DBusInterface() {
    stop();
}

bool DBusInterface::initialize(
    AlgorithmFactory* algorithmFactory,
    AlgorithmRegistry* algorithmRegistry,
    HybridEncryption* hybridEncryption,
    FallbackManager* fallbackManager
) {
    if (m_initialized) {
        return true;
    }

    m_algorithmFactory = algorithmFactory;
    m_algorithmRegistry = algorithmRegistry;
    m_hybridEncryption = hybridEncryption;
    m_fallbackManager = fallbackManager;

    m_initialized = true;
    return true;
}

bool DBusInterface::start() {
    if (!m_initialized) {
        std::cerr << "D-Bus interface not initialized" << std::endl;
        return false;
    }

    if (m_running) {
        return true;
    }

    // Register D-Bus interface
    if (!registerInterface()) {
        std::cerr << "Failed to register D-Bus interface" << std::endl;
        return false;
    }

    m_running = true;
    std::cout << "D-Bus interface started" << std::endl;
    return true;
}

void DBusInterface::stop() {
    if (!m_running) {
        return;
    }

    unregisterInterface();
    m_running = false;
    std::cout << "D-Bus interface stopped" << std::endl;
}

bool DBusInterface::isHealthy() const {
    if (!m_running) {
        return false;
    }

    // Check D-Bus connection health
    QDBusConnection connection = QDBusConnection::systemBus();
    if (!connection.isConnected()) {
        return false;
    }

    return true;
}

QString DBusInterface::GetAvailableAlgorithms() {
    if (!m_algorithmRegistry) {
        std::cerr << "Algorithm registry not available" << std::endl;
        return QString("{\"error\":\"Algorithm registry not available\"}");
    }

    // Get all algorithms
    std::vector<AlgorithmMetadata> algorithms = m_algorithmRegistry->getAllAlgorithms();

    // Convert to JSON
    QJsonArray algorithmsArray;
    for (const auto& alg : algorithms) {
        QJsonObject algObj;
        algObj["algorithm_id"] = QString::fromStdString(alg.algorithm_id);
        algObj["name"] = QString::fromStdString(alg.name);
        algObj["version"] = QString::fromStdString(alg.version);
        algObj["description"] = QString::fromStdString(alg.description);
        algObj["is_nist_approved"] = alg.is_nist_approved;
        algObj["is_hybrid_supported"] = alg.is_hybrid_supported;
        algObj["key_size"] = static_cast<int>(alg.key_size);
        algObj["performance_score"] = alg.performance_score;
        
        QJsonArray capabilitiesArray;
        for (const auto& cap : alg.capabilities) {
            QString capStr;
            switch (cap) {
                case AlgorithmCapability::KEY_ENCAPSULATION:
                    capStr = "KEY_ENCAPSULATION";
                    break;
                case AlgorithmCapability::DIGITAL_SIGNATURE:
                    capStr = "DIGITAL_SIGNATURE";
                    break;
                case AlgorithmCapability::HASH_BASED_SIGNATURE:
                    capStr = "HASH_BASED_SIGNATURE";
                    break;
                case AlgorithmCapability::SYMMETRIC_ENCRYPTION:
                    capStr = "SYMMETRIC_ENCRYPTION";
                    break;
            }
            capabilitiesArray.append(capStr);
        }
        algObj["capabilities"] = capabilitiesArray;
        
        algorithmsArray.append(algObj);
    }

    QJsonObject result;
    result["algorithms"] = algorithmsArray;
    result["count"] = static_cast<int>(algorithms.size());

    QJsonDocument doc(result);
    return QString::fromUtf8(doc.toJson());
}

bool DBusInterface::SelectAlgorithm(const QString& algorithmId) {
    if (!m_algorithmFactory) {
        std::cerr << "Algorithm factory not available" << std::endl;
        return false;
    }

    // Check if algorithm is registered
    if (!m_algorithmFactory->isAlgorithmRegistered(algorithmId.toStdString())) {
        std::cerr << "Algorithm not registered: " << algorithmId.toStdString() << std::endl;
        return false;
    }

    m_selectedAlgorithm = algorithmId;
    return true;
}

QString DBusInterface::GetAlgorithmInfo(const QString& algorithmId) {
    if (!m_algorithmRegistry) {
        std::cerr << "Algorithm registry not available" << std::endl;
        return QString("{\"error\":\"Algorithm registry not available\"}");
    }

    auto metadata = m_algorithmRegistry->getAlgorithmMetadata(algorithmId.toStdString());
    if (!metadata) {
        return QString("{\"error\":\"Algorithm not found\"}");
    }

    // Convert to JSON
    QJsonObject algObj;
    algObj["algorithm_id"] = QString::fromStdString(metadata->algorithm_id);
    algObj["name"] = QString::fromStdString(metadata->name);
    algObj["version"] = QString::fromStdString(metadata->version);
    algObj["description"] = QString::fromStdString(metadata->description);
    algObj["is_nist_approved"] = metadata->is_nist_approved;
    algObj["is_hybrid_supported"] = metadata->is_hybrid_supported;
    algObj["key_size"] = static_cast<int>(metadata->key_size);
    algObj["ciphertext_overhead"] = static_cast<int>(metadata->ciphertext_overhead);
    algObj["performance_score"] = metadata->performance_score;

    QJsonDocument doc(algObj);
    return QString::fromUtf8(doc.toJson());
}

QString DBusInterface::Encrypt(const QString& data, const QString& algorithmId) {
    if (!m_algorithmFactory) {
        std::cerr << "Algorithm factory not available" << std::endl;
        return QString();
    }

    // Determine which algorithm to use
    QString algoId = algorithmId.isEmpty() ? m_selectedAlgorithm : algorithmId;
    if (algoId.isEmpty()) {
        std::cerr << "No algorithm selected" << std::endl;
        return QString();
    }

    // Create algorithm instance
    auto algorithm = m_algorithmFactory->createAlgorithm(algoId.toStdString());
    if (!algorithm) {
        std::cerr << "Failed to create algorithm: " << algoId.toStdString() << std::endl;
        return QString();
    }

    // Decode base64 input
    QByteArray inputData = QByteArray::fromBase64(data.toUtf8());
    std::vector<uint8_t> plaintext(inputData.begin(), inputData.end());

    // Encrypt
    EncryptionResult result = algorithm->encrypt(plaintext);

    // Convert result to JSON
    QJsonObject resultObj;
    resultObj["success"] = result.success;
    if (result.success) {
        QByteArray ciphertext(reinterpret_cast<const char*>(result.ciphertext.data()), 
                             result.ciphertext.size());
        resultObj["ciphertext"] = QString::fromUtf8(ciphertext.toBase64());
        if (!result.key.empty()) {
            QByteArray key(reinterpret_cast<const char*>(result.key.data()), result.key.size());
            resultObj["key"] = QString::fromUtf8(key.toBase64());
        }
        resultObj["algorithm_used"] = QString::fromStdString(result.algorithm_used);
        resultObj["algorithm_version"] = QString::fromStdString(result.algorithm_version);
    } else {
        resultObj["error_message"] = QString::fromStdString(result.error_message);
    }

    QJsonDocument doc(resultObj);
    return QString::fromUtf8(doc.toJson());
}

QString DBusInterface::Decrypt(const QString& encryptedData, const QString& algorithmId) {
    if (!m_algorithmFactory) {
        std::cerr << "Algorithm factory not available" << std::endl;
        return QString();
    }

    // Parse encrypted data JSON
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(encryptedData.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        std::cerr << "Invalid encrypted data format" << std::endl;
        return QString();
    }

    QJsonObject encryptedObj = doc.object();
    if (!encryptedObj.contains("ciphertext")) {
        std::cerr << "Missing ciphertext in encrypted data" << std::endl;
        return QString();
    }

    // Determine which algorithm to use
    QString algoId = algorithmId.isEmpty() ? 
        (encryptedObj.contains("algorithm_used") ? 
         encryptedObj["algorithm_used"].toString() : m_selectedAlgorithm) : 
        algorithmId;
    
    if (algoId.isEmpty()) {
        std::cerr << "No algorithm specified" << std::endl;
        return QString();
    }

    // Create algorithm instance
    auto algorithm = m_algorithmFactory->createAlgorithm(algoId.toStdString());
    if (!algorithm) {
        std::cerr << "Failed to create algorithm: " << algoId.toStdString() << std::endl;
        return QString();
    }

    // Decode base64 ciphertext
    QByteArray ciphertextData = QByteArray::fromBase64(
        encryptedObj["ciphertext"].toString().toUtf8()
    );
    std::vector<uint8_t> ciphertext(ciphertextData.begin(), ciphertextData.end());

    // Decrypt
    DecryptionResult result = algorithm->decrypt(ciphertext);

    // Convert result to JSON
    QJsonObject resultObj;
    resultObj["success"] = result.success;
    if (result.success) {
        QByteArray plaintext(reinterpret_cast<const char*>(result.plaintext.data()), 
                            result.plaintext.size());
        resultObj["plaintext"] = QString::fromUtf8(plaintext.toBase64());
        if (!result.key.empty()) {
            QByteArray key(reinterpret_cast<const char*>(result.key.data()), result.key.size());
            resultObj["key"] = QString::fromUtf8(key.toBase64());
        }
        resultObj["algorithm_used"] = QString::fromStdString(result.algorithm_used);
        resultObj["algorithm_version"] = QString::fromStdString(result.algorithm_version);
    } else {
        resultObj["error_message"] = QString::fromStdString(result.error_message);
    }

    QJsonDocument resultDoc(resultObj);
    return QString::fromUtf8(resultDoc.toJson());
}

bool DBusInterface::registerInterface() {
    QDBusConnection connection = QDBusConnection::systemBus();

    if (!connection.isConnected()) {
        std::cerr << "Cannot connect to D-Bus system bus: "
                  << connection.lastError().message().toStdString() << std::endl;
        return false;
    }

    // Register object at path /org/milos/PQCAbstraction
    QString objectPath = "/org/milos/PQCAbstraction";
    if (!connection.registerObject(objectPath, this)) {
        std::cerr << "Failed to register D-Bus object: "
                  << connection.lastError().message().toStdString() << std::endl;
        return false;
    }

    // Register service name org.milos.PQCAbstraction
    QString serviceName = "org.milos.PQCAbstraction";
    if (!connection.registerService(serviceName)) {
        std::cerr << "Failed to register D-Bus service: "
                  << connection.lastError().message().toStdString() << std::endl;
        connection.unregisterObject(objectPath);
        return false;
    }

    std::cout << "D-Bus interface registered: " << serviceName.toStdString()
              << " at " << objectPath.toStdString() << std::endl;
    return true;
}

void DBusInterface::unregisterInterface() {
    QDBusConnection connection = QDBusConnection::systemBus();

    QString objectPath = "/org/milos/PQCAbstraction";
    QString serviceName = "org.milos.PQCAbstraction";

    connection.unregisterObject(objectPath);
    connection.unregisterService(serviceName);
}

