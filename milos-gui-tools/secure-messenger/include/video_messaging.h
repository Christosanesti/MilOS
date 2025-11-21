#ifndef VIDEO_MESSAGING_H
#define VIDEO_MESSAGING_H

#include "messaging_core.h"
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QSize>

/**
 * @brief Video Message Info
 */
struct VideoMessageInfo {
    QString messageId;
    QString conversationId;
    QString senderId;
    QString recipientId;
    QString videoFilePath;
    int durationSeconds;
    QSize resolution;
    QDateTime recordedAt;
    QString codec;
};

/**
 * @brief Video Messaging
 * 
 * Handles video messaging with recording and playback.
 */
class VideoMessaging : public QObject {
    Q_OBJECT

public:
    explicit VideoMessaging(QObject* parent = nullptr);
    ~VideoMessaging();

    /**
     * @brief Initialize video messaging
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
     * @return Video message info or empty if failed
     */
    VideoMessageInfo stopRecording();

    /**
     * @brief Send video message
     * @param conversationId Conversation ID
     * @param recipientId Recipient ID
     * @param videoFilePath Video file path
     * @return Message ID or empty string if failed
     */
    QString sendVideoMessage(const QString& conversationId, const QString& recipientId, const QString& videoFilePath);

    /**
     * @brief Play video message
     * @param messageId Message ID
     * @return true if play successful, false otherwise
     */
    bool playVideoMessage(const QString& messageId);

    /**
     * @brief Stop playback
     */
    void stopPlayback();

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
    class E2EEncryption* m_e2eEncryption;
    bool m_recording;
    bool m_playing;
    QString m_currentRecordingPath;
    
    QString generateVideoFilePath() const;
    QSize getVideoResolution() const;
    QByteArray readVideoFileData(const QString& filePath) const;
};

#endif // VIDEO_MESSAGING_H

