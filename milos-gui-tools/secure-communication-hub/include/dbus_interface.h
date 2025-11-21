#ifndef SECURE_COMMUNICATION_HUB_DBUS_INTERFACE_H
#define SECURE_COMMUNICATION_HUB_DBUS_INTERFACE_H

#include "communication_hub.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

class CommunicationHub;

/**
 * @brief Secure Communication Hub D-Bus Interface
 */
class SecureCommunicationHubDBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.SecureCommunicationHub")

public:
    explicit SecureCommunicationHubDBusInterface(QObject* parent = nullptr);
    ~SecureCommunicationHubDBusInterface();

    bool initialize();
    void setHub(CommunicationHub* hub);

public slots:
    bool SendMessage(const QString& type, const QString& to, const QString& content);
    QStringList ReceiveMessages(const QString& type, int limit);
    QVariantMap GetMessage(const QString& messageId);
    bool MarkAsRead(const QString& messageId);
    int GetUnreadCount();
    QString CreateGroup(const QString& name, const QStringList& participants);

private:
    CommunicationHub* m_hub;
};

#endif // SECURE_COMMUNICATION_HUB_DBUS_INTERFACE_H

