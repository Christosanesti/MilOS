#include "location_encryption.h"
#include <QCryptographicHash>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

LocationEncryption::LocationEncryption(QObject* parent)
    : QObject(parent)
{
}

LocationEncryption::~LocationEncryption() {
}

bool LocationEncryption::initialize() {
    return generateEncryptionKey();
}

QByteArray LocationEncryption::encryptLocation(const LocationData& location) {
    // Serialize location data to JSON
    QJsonObject json;
    json["personnel_id"] = location.personnelId;
    json["latitude"] = location.coordinate.latitude();
    json["longitude"] = location.coordinate.longitude();
    json["altitude"] = location.coordinate.altitude();
    json["source"] = static_cast<int>(location.source);
    json["timestamp"] = location.timestamp.toString(Qt::ISODate);
    json["accuracy"] = location.accuracy;
    
    QJsonDocument doc(json);
    QByteArray data = doc.toJson();
    
    // In production, would use proper encryption (AES-256 or PQC)
    // For now, simple XOR encryption as placeholder
    QByteArray encrypted = data;
    for (int i = 0; i < encrypted.size(); ++i) {
        encrypted[i] = encrypted[i] ^ m_encryptionKey[i % m_encryptionKey.size()];
    }
    
    return encrypted;
}

LocationData LocationEncryption::decryptLocation(const QByteArray& encryptedData) {
    if (encryptedData.isEmpty()) {
        emit decryptionFailed("Empty encrypted data");
        return LocationData();
    }
    
    // Decrypt
    QByteArray decrypted = encryptedData;
    for (int i = 0; i < decrypted.size(); ++i) {
        decrypted[i] = decrypted[i] ^ m_encryptionKey[i % m_encryptionKey.size()];
    }
    
    // Parse JSON
    QJsonDocument doc = QJsonDocument::fromJson(decrypted);
    if (doc.isNull()) {
        emit decryptionFailed("Invalid JSON data");
        return LocationData();
    }
    
    QJsonObject json = doc.object();
    LocationData location;
    location.personnelId = json["personnel_id"].toString();
    location.coordinate = QGeoCoordinate(
        json["latitude"].toDouble(),
        json["longitude"].toDouble(),
        json["altitude"].toDouble()
    );
    location.source = static_cast<LocationSource>(json["source"].toInt());
    location.timestamp = QDateTime::fromString(json["timestamp"].toString(), Qt::ISODate);
    location.accuracy = json["accuracy"].toDouble();
    
    return location;
}

bool LocationEncryption::generateEncryptionKey() {
    // In production, would generate proper encryption key
    // For now, use hash of system identifier
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData("milos-location-encryption-key");
    m_encryptionKey = hash.result();
    return true;
}

