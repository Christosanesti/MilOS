#ifndef VOICE_MESSAGING_H
#define VOICE_MESSAGING_H

#include "messaging_core.h"
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QAudioFormat>

/**
 * @brief Voice Message Info
 */
struct VoiceMessageInfo {
    QString messageId;
    QString conversationId;
    QString senderId;
    QString recipientId;
    QString audioFilePath;
    int durationSeconds;
    QDateTime recordedAt;
    QString codec;
};

/**
 * @brief Voice Messaging
 * 
 * Handles voice messaging with recording and playback.
 */
class VoiceMessaging : public QObject {
    Q_OBJECT

public:
    explicit VoiceMessaging(QObject* parent = nullptr);
    ~VoiceMessaging();

    /**
     * @brief Initialize voice messaging
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Start recording
     * @return true if start successful, false otherwise
     */
    bool startRecording();

    /**
     * @brief Stop recording
     * @return Voice message info or empty if failed
     */
    VoiceMessageInfo stopRecording();

    /**
     * @brief Send voice message
     * @param conversationId Conversation ID
     * @param recipientId Recipient ID
     * @param audioFilePath Audio file path
     * @return Message ID or empty string if failed
     */
    QString sendVoiceMessage(const QString& conversationId, const QString& recipientId, const QString& audioFilePath);

    /**
     * @brief Play voice message
     * @param messageId Message ID
     * @return true if play successful, false otherwise
     */
    bool playVoiceMessage(const QString& messageId);

    /**
     * @brief Stop playback
     */
    void stopPlayback();

Q_SIGNALS:
    /**
     * @brief Emitted when recording is started
     */
    void recordingStarted();

    /**
     * @brief Emitted when recording is stopped
     */
    void recordingStopped();

    /**
     * @brief Emitted when playback is started
     */
    void playbackStarted();

    /**
     * @brief Emitted when playback is stopped
     */
    void playbackStopped();

private:
    MessagingCore* m_messagingCore;
    bool m_recording;
    bool m_playing;
    QString m_currentRecordingPath;
    
    QString generateAudioFilePath() const;
    QAudioFormat getAudioFormat() const;
};

#endif // VOICE_MESSAGING_H

