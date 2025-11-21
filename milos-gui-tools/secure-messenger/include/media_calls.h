#ifndef MEDIA_CALLS_H
#define MEDIA_CALLS_H

#include "mesh_network.h"
#include <QObject>
#include <QString>
#include <QDateTime>

/**
 * @brief Call Type
 */
enum class CallType {
    Voice,
    Video
};

/**
     * @brief Call Status
     */
enum class CallStatus {
    Idle,
    Calling,
    Ringing,
    Active,
    Ended,
    Failed
};

/**
 * @brief Call Info
 */
struct CallInfo {
    QString callId;
    CallType type;
    CallStatus status;
    QString callerId;
    QString calleeId;
    QDateTime startedAt;
    QDateTime endedAt;
    int durationSeconds;
};

/**
 * @brief Media Calls
 * 
 * Handles peer-to-peer voice and video calls.
 */
class MediaCalls : public QObject {
    Q_OBJECT

public:
    explicit MediaCalls(QObject* parent = nullptr);
    ~MediaCalls();

    /**
     * @brief Initialize media calls
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Start call
     * @param type Call type
     * @param recipientId Recipient ID
     * @return Call ID or empty string if failed
     */
    QString startCall(CallType type, const QString& recipientId);

    /**
     * @brief Accept call
     * @param callId Call ID
     * @return true if accept successful, false otherwise
     */
    bool acceptCall(const QString& callId);

    /**
     * @brief Reject call
     * @param callId Call ID
     * @return true if reject successful, false otherwise
     */
    bool rejectCall(const QString& callId);

    /**
     * @brief End call
     * @param callId Call ID
     * @return true if end successful, false otherwise
     */
    bool endCall(const QString& callId);

    /**
     * @brief Get call info
     * @param callId Call ID
     * @return Call info or empty if not found
     */
    CallInfo getCallInfo(const QString& callId) const;

    /**
     * @brief Set mesh network
     */
    void setMeshNetwork(MeshNetwork* meshNetwork);

Q_SIGNALS:
    /**
     * @brief Emitted when incoming call is received
     */
    void incomingCall(const QString& callId, CallType type, const QString& callerId);

    /**
     * @brief Emitted when call is accepted
     */
    void callAccepted(const QString& callId);

    /**
     * @brief Emitted when call is rejected
     */
    void callRejected(const QString& callId);

    /**
     * @brief Emitted when call is ended
     */
    void callEnded(const QString& callId);

private:
    MeshNetwork* m_meshNetwork;
    QMap<QString, CallInfo> m_calls;
    
    QString generateCallId() const;
};

#endif // MEDIA_CALLS_H

