#include "video_messaging.h"
#include "messaging_core.h"
#include "e2e_encryption.h"
#include <QUuid>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QSize>
#include <QDebug>

VideoMessaging::VideoMessaging(QObject* parent)
    : QObject(parent)
    , m_messagingCore(nullptr)
    , m_e2eEncryption(nullptr)
    , m_recording(false)
    , m_playing(false)
{
}

VideoMessaging::~VideoMessaging() {
    if (m_recording) {
        stopRecording();
    }
    if (m_playing) {
        stopPlayback();
    }
}

bool VideoMessaging::initialize() {
    return true;
}

bool VideoMessaging::startRecording() {
    if (m_recording) {
        return false;
    }

    m_recording = true;
    m_currentRecordingPath = generateVideoFilePath();

    // In production, would use Qt Multimedia to start recording
    emit recordingStarted();

    return true;
}

VideoMessageInfo VideoMessaging::stopRecording() {
    if (!m_recording) {
        return VideoMessageInfo();
    }

    m_recording = false;

    // In production, would use Qt Multimedia to stop recording and save file
    VideoMessageInfo info;
    info.videoFilePath = m_currentRecordingPath;
    info.durationSeconds = 0;  // Would be calculated from recording
    info.resolution = getVideoResolution();
    info.recordedAt = QDateTime::currentDateTime();
    info.codec = "vp9";  // Default codec

    emit recordingStopped();

    return info;
}

QString VideoMessaging::sendVideoMessage(const QString& conversationId, const QString& recipientId, const QString& videoFilePath) {
    if (!m_messagingCore) {
        return QString();
    }

    // Read video file data
    QByteArray videoData = readVideoFileData(videoFilePath);
    if (videoData.isEmpty()) {
        qWarning() << "Failed to read video file:" << videoFilePath;
        return QString();
    }

    // Encrypt video data if E2E encryption is available
    QByteArray encryptedData = videoData;
    if (m_e2eEncryption) {
        encryptedData = m_e2eEncryption->encryptMedia(videoData, recipientId);
        if (encryptedData.isEmpty()) {
            qWarning() << "Failed to encrypt video data";
            return QString();
        }
    }

    QFileInfo fileInfo(videoFilePath);
    Message message;
    message.conversationId = conversationId;
    message.recipientId = recipientId;
    message.type = MessageType::Video;
    message.content = fileInfo.fileName();
    message.data = encryptedData;
    message.metadata["video_file"] = videoFilePath;
    message.metadata["codec"] = "vp9";
    message.metadata["resolution"] = getVideoResolution();
    message.status = MessageStatus::Pending;

    QString messageId = m_messagingCore->sendMessage(message);
    return messageId;
}

bool VideoMessaging::playVideoMessage(const QString& messageId) {
    if (m_playing) {
        return false;
    }

    // In production, would use Qt Multimedia to play video file
    m_playing = true;
    emit playbackStarted();

    return true;
}

void VideoMessaging::stopPlayback() {
    if (!m_playing) {
        return;
    }

    m_playing = false;
}

void VideoMessaging::setMessagingCore(MessagingCore* messagingCore) {
    m_messagingCore = messagingCore;
}

void VideoMessaging::setE2EEncryption(E2EEncryption* e2eEncryption) {
    m_e2eEncryption = e2eEncryption;
}

QByteArray VideoMessaging::readVideoFileData(const QString& filePath) const {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open video file for reading:" << filePath;
        return QByteArray();
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    return data;
}
    emit playbackStopped();
}

QString VideoMessaging::generateVideoFilePath() const {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return dir + "/video_" + QUuid::createUuid().toString(QUuid::WithoutBraces) + ".webm";
}

QSize VideoMessaging::getVideoResolution() const {
    return QSize(1280, 720);  // Default HD resolution
}

