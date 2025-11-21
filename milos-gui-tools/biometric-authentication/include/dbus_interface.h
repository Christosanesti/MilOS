#ifndef BIOMETRIC_AUTHENTICATION_DBUS_INTERFACE_H
#define BIOMETRIC_AUTHENTICATION_DBUS_INTERFACE_H

#include "biometric_authentication_system.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>

class BiometricAuthenticationSystem;

/**
 * @brief Biometric Authentication D-Bus Interface
 */
class BiometricAuthenticationDBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.BiometricAuthentication")

public:
    explicit BiometricAuthenticationDBusInterface(QObject* parent = nullptr);
    ~BiometricAuthenticationDBusInterface();

    bool initialize();
    void setSystem(BiometricAuthenticationSystem* system);

public slots:
    bool EnrollUser(const QString& userId, int biometricType);
    bool AuthenticateUser(const QString& userId, int biometricType);
    bool IsUserEnrolled(const QString& userId, int biometricType);
    QStringList GetEnrolledTypes(const QString& userId);
    bool AuthenticateMFA(const QString& userId, const QStringList& factors);

private:
    BiometricAuthenticationSystem* m_system;
};

#endif // BIOMETRIC_AUTHENTICATION_DBUS_INTERFACE_H

