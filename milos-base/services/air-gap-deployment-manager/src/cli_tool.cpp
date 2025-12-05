#include "air_gap_deployment_manager.h"
#include <milos/logging/logger.h>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QFileInfo>
#include <iostream>

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    app.setApplicationName("milos-air-gap-package");
    app.setOrganizationName("MilOS");

    // Initialize logger
    Logger::instance()->initialize("milos-air-gap-package",
                                   "org.milos.AuditService",
                                   "/org/milos/AuditService",
                                   Logger::Info,
                                   false);

    QCommandLineParser parser;
    parser.setApplicationDescription("MilOS Air-Gap Deployment Package Manager");
    parser.addHelpOption();
    parser.addVersionOption();

    // Global options
    QCommandLineOption storageOption(QStringList() << "s" << "storage",
                                    "Package storage path",
                                    "path",
                                    "/var/lib/milos/air-gap-packages");
    parser.addOption(storageOption);

    // Create command
    parser.addPositionalArgument("command",
                                 "Command: create, verify, deploy, list, info");

    // Parse arguments
    parser.process(app);

    const QStringList args = parser.positionalArguments();
    if (args.isEmpty()) {
        parser.showHelp(1);
    }

    QString command = args.at(0);

    // Initialize manager
    AirGapDeploymentManager manager;
    QString storagePath = parser.value(storageOption);
    if (!manager.initialize(storagePath)) {
        LOG_ERROR(QString("Failed to initialize deployment manager: %1").arg(storagePath));
        std::cerr << "Error: Failed to initialize deployment manager" << std::endl;
        return 1;
    }

    // Handle commands
    if (command == "create") {
        if (args.size() < 3) {
            std::cerr << "Usage: milos-air-gap-package create <name> <version> [services...]" << std::endl;
            return 1;
        }

        QString packageName = args.at(1);
        QString version = args.at(2);
        QStringList services;
        for (int i = 3; i < args.size(); ++i) {
            services.append(args.at(i));
        }

        std::cout << "Creating deployment package..." << std::endl;
        std::cout << "  Name: " << packageName.toStdString() << std::endl;
        std::cout << "  Version: " << version.toStdString() << std::endl;

        QString packageId = manager.createDeploymentPackage(packageName, version, services, true);

        if (packageId.isEmpty()) {
            std::cerr << "Error: Failed to create package" << std::endl;
            return 1;
        }

        std::cout << "Package created: " << packageId.toStdString() << std::endl;
        AirGapPackage info = manager.getPackageInfo(packageId);
        std::cout << "  Path: " << info.packagePath.toStdString() << std::endl;
        std::cout << "  Size: " << (info.size / 1024 / 1024) << " MB" << std::endl;
        std::cout << "  Checksum: " << info.checksum.toStdString() << std::endl;

    } else if (command == "verify") {
        if (args.size() < 2) {
            std::cerr << "Usage: milos-air-gap-package verify <package-id>" << std::endl;
            return 1;
        }

        QString packageId = args.at(1);
        std::cout << "Verifying package: " << packageId.toStdString() << std::endl;

        if (manager.verifyPackage(packageId)) {
            std::cout << "Package verified successfully" << std::endl;
        } else {
            std::cerr << "Error: Package verification failed" << std::endl;
            return 1;
        }

    } else if (command == "deploy") {
        if (args.size() < 2) {
            std::cerr << "Usage: milos-air-gap-package deploy <package-id> [target-path]" << std::endl;
            return 1;
        }

        QString packageId = args.at(1);
        QString targetPath = args.size() > 2 ? args.at(2) : "/";

        std::cout << "Deploying package: " << packageId.toStdString() << std::endl;
        std::cout << "  Target: " << targetPath.toStdString() << std::endl;

        if (manager.deployPackage(packageId, targetPath)) {
            std::cout << "Package deployed successfully" << std::endl;
        } else {
            std::cerr << "Error: Package deployment failed" << std::endl;
            return 1;
        }

    } else if (command == "list") {
        QStringList packages = manager.availablePackages();
        if (packages.isEmpty()) {
            std::cout << "No packages available" << std::endl;
        } else {
            std::cout << "Available packages:" << std::endl;
            for (const QString& packageId : packages) {
                AirGapPackage info = manager.getPackageInfo(packageId);
                std::cout << "  " << packageId.toStdString() << std::endl;
                std::cout << "    Name: " << info.packageName.toStdString() << std::endl;
                std::cout << "    Version: " << info.version.toStdString() << std::endl;
                std::cout << "    Size: " << (info.size / 1024 / 1024) << " MB" << std::endl;
                std::cout << "    Status: " << (info.status == PackageStatus::Ready ? "Ready" : "Pending") << std::endl;
            }
        }

    } else if (command == "info") {
        if (args.size() < 2) {
            std::cerr << "Usage: milos-air-gap-package info <package-id>" << std::endl;
            return 1;
        }

        QString packageId = args.at(1);
        AirGapPackage info = manager.getPackageInfo(packageId);

        if (info.packageId.isEmpty()) {
            std::cerr << "Error: Package not found" << std::endl;
            return 1;
        }

        std::cout << "Package Information:" << std::endl;
        std::cout << "  ID: " << info.packageId.toStdString() << std::endl;
        std::cout << "  Name: " << info.packageName.toStdString() << std::endl;
        std::cout << "  Version: " << info.version.toStdString() << std::endl;
        std::cout << "  Path: " << info.packagePath.toStdString() << std::endl;
        std::cout << "  Size: " << (info.size / 1024 / 1024) << " MB" << std::endl;
        std::cout << "  Checksum: " << info.checksum.toStdString() << std::endl;
        std::cout << "  Created: " << info.createdAt.toString(Qt::ISODate).toStdString() << std::endl;
        std::cout << "  Status: " << (info.status == PackageStatus::Ready ? "Ready" : "Pending") << std::endl;

    } else {
        std::cerr << "Unknown command: " << command.toStdString() << std::endl;
        parser.showHelp(1);
    }

    return 0;
}

