#ifndef SECURE_DEV_DBUS_INTERFACE_H
#define SECURE_DEV_DBUS_INTERFACE_H

#include "secure_dev_system.h"
#include <QObject>
#include <QString>
#include <QVariantMap>

class SecureDevSystem;

/**
 * @brief Secure Development D-Bus Interface
 */
class SecureDevDBusInterface : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.milos.SecureDevelopment")

public:
    explicit SecureDevDBusInterface(QObject* parent = nullptr);
    ~SecureDevDBusInterface();

    bool initialize();
    void setSystem(SecureDevSystem* system);

public slots:
    QVariantMap AnalyzeFile(const QString& filePath);
    QVariantMap AnalyzeDirectory(const QString& directoryPath);
    QVariantMap ScanDependencies(const QString& projectPath);
    QVariantMap GetSecurityStatistics();

private:
    SecureDevSystem* m_system;
};

#endif // SECURE_DEV_DBUS_INTERFACE_H






