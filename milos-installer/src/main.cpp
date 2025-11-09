#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include "ui/mainwindow.h"
#include "ui/encryptionsetupscreen.h"
#include "ui/useraccountscreen.h"
#include "ui/networkconfigurationscreen.h"
#include "ui/hardwaredetectionscreen.h"
#include "services/encryptionmanager.h"
#include "services/passwordvalidator.h"
#include "services/useraccountmanager.h"
#include "services/networkmanager.h"
#include "services/hardwaremanager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    app.setApplicationName("MilOS Installer");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("MilOS");
    
    // Register QML types
    qmlRegisterType<MainWindow>("MilosInstaller", 1, 0, "MainWindow");
    qmlRegisterType<EncryptionSetupScreen>("MilosInstaller", 1, 0, "EncryptionSetupScreen");
    qmlRegisterType<UserAccountScreen>("MilosInstaller", 1, 0, "UserAccountScreen");
    qmlRegisterType<NetworkConfigurationScreen>("MilosInstaller", 1, 0, "NetworkConfigurationScreen");
    qmlRegisterType<HardwareDetectionScreen>("MilosInstaller", 1, 0, "HardwareDetectionScreen");
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose services to QML
    EncryptionManager encryptionManager;
    PasswordValidator passwordValidator;
    UserAccountManager userAccountManager;
    NetworkManager networkManager;
    HardwareManager hardwareManager;
    engine.rootContext()->setContextProperty("encryptionManager", &encryptionManager);
    engine.rootContext()->setContextProperty("passwordValidator", &passwordValidator);
    engine.rootContext()->setContextProperty("userAccountManager", &userAccountManager);
    engine.rootContext()->setContextProperty("networkManager", &networkManager);
    engine.rootContext()->setContextProperty("hardwareManager", &hardwareManager);
    
    // Set QML import paths
    QStringList importPaths = engine.importPathList();
    importPaths.prepend(QDir::currentPath() + "/src/ui");
    importPaths.prepend(QDir::currentPath() + "/../../milos-kde-theme/libraries/milos-component-library/src/qml");
    engine.setImportPathList(importPaths);
    
    // Load main QML file (development mode - load from source)
    QDir appDir(QCoreApplication::applicationDirPath());
    QString qmlPath = appDir.absoluteFilePath("../src/ui/mainwindow.qml");
    if (!QFile::exists(qmlPath)) {
        // Fallback to current directory
        qmlPath = QDir::currentPath() + "/src/ui/mainwindow.qml";
    }
    
    const QUrl url = QUrl::fromLocalFile(qmlPath);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    return app.exec();
}

