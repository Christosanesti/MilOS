#ifndef KEY_EXCHANGE_H
#define KEY_EXCHANGE_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QDateTime>

/**
 * @brief Key Exchange Status
 */
enum class KeyExchangeStatus {
    Pending,
    Completed,
    Failed,
    Expired
};

/**
 * @brief Key Exchange Info
 */
struct KeyExchangeInfo {
    QString exchangeId;
    QString participantId;
    QByteArray publicKey;
    KeyExchangeStatus status;
    QDateTime initiatedAt;
    QDateTime completedAt;
};

/**
 * @brief Secure Key Exchange
 * 
 * Implements secure key exchange protocol for establishing encryption keys.
 */
class KeyExchange : public QObject {
    Q_OBJECT

public:
    explicit KeyExchange(QObject* parent = nullptr);
    ~KeyExchange();

    /**
     * @brief Initialize key exchange
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Initiate key exchange
     * @param participantId Participant ID
     * @return Exchange ID or empty string if failed
     */
    QString initiateKeyExchange(const QString& participantId);

    /**
     * @brief Complete key exchange
     * @param exchangeId Exchange ID
     * @param publicKey Public key from participant
     * @return true if completion successful, false otherwise
     */
    bool completeKeyExchange(const QString& exchangeId, const QByteArray& publicKey);

    /**
     * @brief Get exchange info
     * @param exchangeId Exchange ID
     * @return Exchange info or empty if not found
     */
    KeyExchangeInfo getExchangeInfo(const QString& exchangeId) const;

    /**
     * @brief Validate key exchange
     * @param exchangeId Exchange ID
     * @return true if valid, false otherwise
     */
    bool validateKeyExchange(const QString& exchangeId) const;

Q_SIGNALS:
    /**
     * @brief Emitted when key exchange is initiated
     */
    void keyExchangeInitiated(const QString& exchangeId);

    /**
     * @brief Emitted when key exchange is completed
     */
    void keyExchangeCompleted(const QString& exchangeId);

    /**
     * @brief Emitted when key exchange fails
     */
    void keyExchangeFailed(const QString& exchangeId, const QString& error);

private:
    QMap<QString, KeyExchangeInfo> m_exchanges;
    QByteArray m_localPublicKey;
    QByteArray m_localPrivateKey;
    
    QString generateExchangeId() const;
    bool generateKeyPair();
};

#endif // KEY_EXCHANGE_H

