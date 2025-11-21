#ifndef RADIO_PROTOCOL_H
#define RADIO_PROTOCOL_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVariantMap>

/**
 * @brief Radio Protocol Type
 */
enum class RadioProtocolType {
    SINCGARS,
    HAVE_QUICK,
    Link16,
    VHF,
    UHF,
    HF,
    Custom
};

/**
 * @brief Radio Message
 */
struct RadioMessage {
    QString id;
    QString from;
    QString to;
    QByteArray payload;
    QDateTime timestamp;
    bool encrypted;
    int priority;  // 0-9, 9 is highest
    QString protocol;
};

/**
 * @brief Radio Protocol Handler
 * 
 * Handles military radio protocol communication.
 */
class RadioProtocol : public QObject {
    Q_OBJECT

public:
    explicit RadioProtocol(QObject* parent = nullptr);
    ~RadioProtocol();

    /**
     * @brief Initialize radio protocol
     * @param protocolType Protocol type
     * @param frequency Frequency in MHz
     * @return true if initialization successful, false otherwise
     */
    bool initialize(RadioProtocolType protocolType, double frequency);

    /**
     * @brief Send message
     * @param message Radio message
     * @return true if send successful, false otherwise
     */
    bool sendMessage(const RadioMessage& message);

    /**
     * @brief Receive message
     * @return Radio message or empty if none
     */
    RadioMessage receiveMessage();

    /**
     * @brief Get protocol type
     * @return Protocol type
     */
    RadioProtocolType protocolType() const { return m_protocolType; }

    /**
     * @brief Get frequency
     * @return Frequency in MHz
     */
    double frequency() const { return m_frequency; }

    /**
     * @brief Set encryption key
     * @param key Encryption key
     * @return true if set successful, false otherwise
     */
    bool setEncryptionKey(const QByteArray& key);

Q_SIGNALS:
    /**
     * @brief Emitted when message is received
     */
    void messageReceived(const RadioMessage& message);

    /**
     * @brief Emitted when message is sent
     */
    void messageSent(const QString& messageId);

private:
    RadioProtocolType m_protocolType;
    double m_frequency;
    QByteArray m_encryptionKey;
    
    QByteArray encryptPayload(const QByteArray& payload) const;
    QByteArray decryptPayload(const QByteArray& encryptedPayload) const;
};

Q_DECLARE_METATYPE(RadioProtocolType)
Q_DECLARE_METATYPE(RadioMessage)

#endif // RADIO_PROTOCOL_H

