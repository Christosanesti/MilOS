#ifndef EMAIL_CLIENT_H
#define EMAIL_CLIENT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QVariantMap>

/**
 * @brief Email Message
 */
struct EmailMessage {
    QString id;
    QString from;
    QStringList to;
    QStringList cc;
    QStringList bcc;
    QString subject;
    QString body;
    QStringList attachments;
    QDateTime sentAt;
    QDateTime receivedAt;
    bool isRead;
    bool isEncrypted;
    QVariantMap metadata;
};

/**
 * @brief Email Account
 */
struct EmailAccount {
    QString id;
    QString email;
    QString server;
    int port;
    QString protocol;  // "IMAP", "POP3", "SMTP"
    bool useEncryption;
    QString encryptionMethod;
};

/**
 * @brief Email Client
 * 
 * Encrypted email client with end-to-end encryption support.
 */
class EmailClient : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionChanged)

public:
    explicit EmailClient(QObject* parent = nullptr);
    ~EmailClient();

    /**
     * @brief Initialize email client
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Add email account
     * @param account Email account
     * @return true if add successful, false otherwise
     */
    bool addAccount(const EmailAccount& account);

    /**
     * @brief Connect to email server
     * @param accountId Account ID
     * @return true if connect successful, false otherwise
     */
    bool connectToServer(const QString& accountId);

    /**
     * @brief Disconnect from email server
     * @param accountId Account ID
     */
    void disconnectFromServer(const QString& accountId);

    /**
     * @brief Check if connected
     * @return true if connected, false otherwise
     */
    bool isConnected() const { return m_connected; }

    /**
     * @brief Send email
     * @param message Email message
     * @return true if send successful, false otherwise
     */
    bool sendEmail(const EmailMessage& message);

    /**
     * @brief Receive emails
     * @param accountId Account ID
     * @param limit Maximum number of emails
     * @return List of email messages
     */
    QList<EmailMessage> receiveEmails(const QString& accountId, int limit = 50);

    /**
     * @brief Get email
     * @param messageId Message ID
     * @return Email message or empty if not found
     */
    EmailMessage getEmail(const QString& messageId) const;

    /**
     * @brief Encrypt email
     * @param message Email message
     * @return Encrypted email data
     */
    QByteArray encryptEmail(const EmailMessage& message);

    /**
     * @brief Decrypt email
     * @param encryptedData Encrypted email data
     * @return Email message or empty if failed
     */
    EmailMessage decryptEmail(const QByteArray& encryptedData);

Q_SIGNALS:
    /**
     * @brief Emitted when connection state changes
     */
    void connectionChanged(bool connected);

    /**
     * @brief Emitted when email is received
     */
    void emailReceived(const EmailMessage& message);

    /**
     * @brief Emitted when email is sent
     */
    void emailSent(const QString& messageId);

private:
    bool m_connected;
    QMap<QString, EmailAccount> m_accounts;
    QMap<QString, EmailMessage> m_messages;
    QByteArray m_encryptionKey;
    
    bool generateEncryptionKey();
};

Q_DECLARE_METATYPE(EmailMessage)
Q_DECLARE_METATYPE(EmailAccount)

#endif // EMAIL_CLIENT_H

