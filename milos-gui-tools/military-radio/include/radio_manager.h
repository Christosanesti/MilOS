#ifndef RADIO_MANAGER_H
#define RADIO_MANAGER_H

#include "radio_protocol.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>

/**
 * @brief Radio Manager
 * 
 * Manages multiple radio protocols and channels.
 */
class RadioManager : public QObject {
    Q_OBJECT

public:
    explicit RadioManager(QObject* parent = nullptr);
    ~RadioManager();

    /**
     * @brief Initialize radio manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Add radio protocol
     * @param protocol Radio protocol
     * @return true if add successful, false otherwise
     */
    bool addProtocol(RadioProtocol* protocol);

    /**
     * @brief Send message via protocol
     * @param protocolType Protocol type
     * @param message Radio message
     * @return true if send successful, false otherwise
     */
    bool sendMessage(RadioProtocolType protocolType, const RadioMessage& message);

    /**
     * @brief Get available protocols
     * @return List of protocol types
     */
    QList<RadioProtocolType> getAvailableProtocols() const;

Q_SIGNALS:
    /**
     * @brief Emitted when message is received
     */
    void messageReceived(const RadioMessage& message);

private:
    QList<RadioProtocol*> m_protocols;
    
    RadioProtocol* getProtocol(RadioProtocolType type) const;
};

#endif // RADIO_MANAGER_H

