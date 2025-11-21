#ifndef TEXT_MESSAGING_H
#define TEXT_MESSAGING_H

#include "messaging_core.h"
#include <QObject>
#include <QString>

/**
 * @brief Text Formatting Type
 */
enum class TextFormatType {
    Plain,
    Markdown,
    HTML
};

/**
 * @brief Text Messaging
 * 
 * Handles text messaging with rich formatting support.
 */
class TextMessaging : public QObject {
    Q_OBJECT

public:
    explicit TextMessaging(QObject* parent = nullptr);
    ~TextMessaging();

    /**
     * @brief Initialize text messaging
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Send text message
     * @param conversationId Conversation ID
     * @param recipientId Recipient ID
     * @param text Text content
     * @param formatType Format type
     * @return Message ID or empty string if failed
     */
    QString sendTextMessage(const QString& conversationId,
                           const QString& recipientId,
                           const QString& text,
                           TextFormatType formatType = TextFormatType::Plain);

    /**
     * @brief Format text
     * @param text Text content
     * @param formatType Format type
     * @return Formatted text
     */
    QString formatText(const QString& text, TextFormatType formatType) const;

    /**
     * @brief Parse formatted text
     * @param formattedText Formatted text
     * @param formatType Format type
     * @return Plain text
     */
    QString parseFormattedText(const QString& formattedText, TextFormatType formatType) const;

    /**
     * @brief Set messaging core
     */
    void setMessagingCore(MessagingCore* messagingCore);

    /**
     * @brief Set E2E encryption
     */
    void setE2EEncryption(class E2EEncryption* e2eEncryption);

Q_SIGNALS:
    /**
     * @brief Emitted when text message is sent
     */
    void textMessageSent(const QString& messageId);

private:
    MessagingCore* m_messagingCore;
    class E2EEncryption* m_e2eEncryption;
    QString formatMarkdown(const QString& text) const;
    QString formatHTML(const QString& text) const;
};

#endif // TEXT_MESSAGING_H

