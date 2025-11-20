#include "text_messaging.h"
#include "messaging_core.h"
#include <QDebug>

TextMessaging::TextMessaging(QObject* parent)
    : QObject(parent)
    , m_messagingCore(nullptr)
{
}

TextMessaging::~TextMessaging() {
}

bool TextMessaging::initialize() {
    return true;
}

QString TextMessaging::sendTextMessage(const QString& conversationId,
                                      const QString& recipientId,
                                      const QString& text,
                                      TextFormatType formatType) {
    if (!m_messagingCore) {
        return QString();
    }
    
    QString formattedText = formatText(text, formatType);
    
    Message message;
    message.conversationId = conversationId;
    message.recipientId = recipientId;
    message.type = MessageType::Text;
    message.content = formattedText;
    message.status = MessageStatus::Pending;
    
    QString messageId = m_messagingCore->sendMessage(message);
    
    if (!messageId.isEmpty()) {
        emit textMessageSent(messageId);
    }
    
    return messageId;
}

QString TextMessaging::formatText(const QString& text, TextFormatType formatType) const {
    switch (formatType) {
        case TextFormatType::Markdown:
            return formatMarkdown(text);
        case TextFormatType::HTML:
            return formatHTML(text);
        case TextFormatType::Plain:
        default:
            return text;
    }
}

QString TextMessaging::parseFormattedText(const QString& formattedText, TextFormatType formatType) const {
    // In production, would parse formatted text to plain text
    // For now, return as-is
    return formattedText;
}

QString TextMessaging::formatMarkdown(const QString& text) const {
    // In production, would use markdown parser
    // For now, return as-is
    return text;
}

QString TextMessaging::formatHTML(const QString& text) const {
    // In production, would use HTML formatter
    // For now, return as-is
    return text;
}

void TextMessaging::setMessagingCore(MessagingCore* messagingCore) {
    m_messagingCore = messagingCore;
}

