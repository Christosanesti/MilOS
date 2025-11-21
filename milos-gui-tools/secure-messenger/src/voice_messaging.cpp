#include "voice_messaging.h"
#include "messaging_core.h"
#include "e2e_encryption.h"
#include <QUuid>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QAudioFormat>
#include <QDebug>

VoiceMessaging::VoiceMessaging(QObject* parent)
    : QObject(parent)
    , m_messagingCore(nullptr)
    , m_e2eEncryption(nullptr)
    , m_recording(false)
    , m_playing(false)
{
}

VoiceMessaging::~VoiceMessaging() {
    if (m_recording) {
        stopRecording();
    }
    if (m_playing) {
        stopPlayback();
    }
}

bool VoiceMessaging::initialize() {
    return true;
}

bool VoiceMessaging::startRecording() {
    if (m_recording) {
        return false;
    }

    m_recording = true;
    m_currentRecordingPath = generateAudioFilePath();

    // In production, would use Qt Multimedia to start recording
    emit recordingStarted();

    return true;
}

VoiceMessageInfo VoiceMessaging::stopRecording() {
    if (!m_recording) {
        return VoiceMessageInfo();
    }

    m_recording = false;

    // In production, would use Qt Multimedia to stop recording and save file
    VoiceMessageInfo info;
    info.audioFilePath = m_currentRecordingPath;
    info.durationSeconds = 0;  // Would be calculated from recording
    info.recordedAt = QDateTime::currentDateTime();
    info.codec = "opus";  // Default codec

    emit recordingStopped();

    return info;
}

QString VoiceMessaging::sendVoiceMessage(const QString& conversationId, const QString& recipientId, const QString& audioFilePath) {
    if (!m_messagingCore) {
        return QString();
    }

    // Read audio file data
    QByteArray audioData = readAudioFileData(audioFilePath);
    if (audioData.isEmpty()) {
        qWarning() << "Failed to read audio file:" << audioFilePath;
        return QString();
    }

    // Encrypt audio data if E2E encryption is available
    QByteArray encryptedData = audioData;
    if (m_e2eEncryption) {
        encryptedData = m_e2eEncryption->encryptMedia(audioData, recipientId);
        if (encryptedData.isEmpty()) {
            qWarning() << "Failed to encrypt audio data";
            return QString();
        }
    }

    QFileInfo fileInfo(audioFilePath);
    Message message;
    message.conversationId = conversationId;
    message.recipientId = recipientId;
    message.type = MessageType::Voice;
    message.content = fileInfo.fileName();
    message.data = encryptedData;
    message.metadata["audio_file"] = audioFilePath;
    message.metadata["codec"] = "opus";
    message.status = MessageStatus::Pending;

    QString messageId = m_messagingCore->sendMessage(message);
    return messageId;
}

void VoiceMessaging::setMessagingCore(MessagingCore* messagingCore) {
    m_messagingCore = messagingCore;
}

void VoiceMessaging::setE2EEncryption(E2EEncryption* e2eEncryption) {
    m_e2eEncryption = e2eEncryption;
}

QByteArray VoiceMessaging::readAudioFileData(const QString& filePath) const {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open audio file for reading:" << filePath;
        return QByteArray();
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    return data;
}

bool VoiceMessaging::playVoiceMessage(const QString& messageId) {
    if (m_playing) {
        return false;
    }

    // In production, would use Qt Multimedia to play audio file
    m_playing = true;
    emit playbackStarted();

    return true;
}

void VoiceMessaging::stopPlayback() {
    if (!m_playing) {
        return;
    }

    m_playing = false;
    emit playbackStopped();
}

QString VoiceMessaging::generateAudioFilePath() const {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return dir + "/voice_" + QUuid::createUuid().toString(QUuid::WithoutBraces) + ".opus";
}

QAudioFormat VoiceMessaging::getAudioFormat() const {
    QAudioFormat format;
    format.setSampleRate(48000);
    format.setChannelCount(1);
    // Note: setSampleFormat may not exist in all Qt versions
    // In production, would use appropriate format setting
    return format;
}

