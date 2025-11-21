#include "media_calls.h"
#include "mesh_network.h"
#include <QUuid>
#include <QDebug>

MediaCalls::MediaCalls(QObject* parent)
    : QObject(parent)
    , m_meshNetwork(nullptr)
{
}

MediaCalls::~MediaCalls() {
}

bool MediaCalls::initialize() {
    return true;
}

QString MediaCalls::startCall(CallType type, const QString& recipientId) {
    CallInfo call;
    call.callId = generateCallId();
    call.type = type;
    call.status = CallStatus::Calling;
    call.callerId = "";  // Would be set from current user
    call.calleeId = recipientId;
    call.startedAt = QDateTime::currentDateTime();

    m_calls[call.callId] = call;

    // In production, would initiate call via mesh network
    return call.callId;
}

bool MediaCalls::acceptCall(const QString& callId) {
    if (!m_calls.contains(callId)) {
        return false;
    }

    CallInfo& call = m_calls[callId];
    call.status = CallStatus::Active;

    emit callAccepted(callId);

    return true;
}

bool MediaCalls::rejectCall(const QString& callId) {
    if (!m_calls.contains(callId)) {
        return false;
    }

    CallInfo& call = m_calls[callId];
    call.status = CallStatus::Ended;
    call.endedAt = QDateTime::currentDateTime();

    emit callRejected(callId);

    return true;
}

bool MediaCalls::endCall(const QString& callId) {
    if (!m_calls.contains(callId)) {
        return false;
    }

    CallInfo& call = m_calls[callId];
    call.status = CallStatus::Ended;
    call.endedAt = QDateTime::currentDateTime();
    call.durationSeconds = call.startedAt.secsTo(call.endedAt);

    emit callEnded(callId);

    return true;
}

CallInfo MediaCalls::getCallInfo(const QString& callId) const {
    return m_calls.value(callId, CallInfo());
}

QString MediaCalls::generateCallId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

void MediaCalls::setMeshNetwork(MeshNetwork* meshNetwork) {
    m_meshNetwork = meshNetwork;
}

