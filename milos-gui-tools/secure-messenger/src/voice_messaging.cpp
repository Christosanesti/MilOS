#include "voice_messaging.h"
#include "messaging_core.h"
#include <QUuid>
#include <QStandardPaths>
#include <QDir>
#include <QAudioFormat>
#include <QDebug>

VoiceMessaging::VoiceMessaging(QObject* parent)
    : QObject(parent)
    , m_messagingCore(nullptr)
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

    Message message;
    message.conversationId = conversationId;
    message.recipientId = recipientId;
    message.type = MessageType::Voice;
    message.content = audioFilePath;
    message.status = MessageStatus::Pending;

    QString messageId = m_messagingCore->sendMessage(message);
    return messageId;
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

