#include <iostream>
#include <QCoreApplication>
#include <QStringList>
#include "batchcommand.h"

// CLI11 header-only library
// Note: In production, this would be included as a proper dependency
// For now, we'll use a simple argument parser or include CLI11 header
// Since CLI11 is header-only, we can include it directly if available
// Otherwise, we'll implement a simple argument parser for MVP

// Simple argument parser for MVP (CLI11 will be integrated later)
class SimpleArgParser {
public:
    QStringList parseArguments(int argc, char *argv[]) {
        QStringList args;
        for (int i = 1; i < argc; ++i) {
            args << QString::fromLocal8Bit(argv[i]);
        }
        return args;
    }

    bool hasSubcommand(const QStringList &args, const QString &subcommand) {
        return args.contains(subcommand);
    }

    QStringList getPositionalArgs(const QStringList &args, const QString &subcommand) {
        QStringList positional;
        bool foundSubcommand = false;
        
        for (const QString &arg : args) {
            if (arg == subcommand) {
                foundSubcommand = true;
                continue;
            }
            
            if (foundSubcommand) {
                if (arg.startsWith("--")) {
                    // Skip flags for positional args
                    continue;
                }
                positional << arg;
            }
        }
        
        return positional;
    }

    QString getFlagValue(const QStringList &args, const QString &flag) {
        int index = args.indexOf(flag);
        if (index >= 0 && index < args.size() - 1) {
            return args[index + 1];
        }
        return QString();
    }

    bool hasFlag(const QStringList &args, const QString &flag) {
        return args.contains(flag);
    }
};

void printUsage() {
    std::cout << "Usage: milos-encrypt batch [OPTIONS] <file1> <file2> ... [file5]" << std::endl;
    std::cout << std::endl;
    std::cout << "Encrypt multiple files in batch mode." << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --algorithm <alg>    Encryption algorithm (CRYSTALS-Kyber, etc.)" << std::endl;
    std::cout << "  --key-id <key_id>    Encryption key ID (UUID)" << std::endl;
    std::cout << "  --help               Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  milos-encrypt batch --algorithm CRYSTALS-Kyber --key-id <uuid> file1.txt file2.txt" << std::endl;
    std::cout << "  milos-encrypt batch --algorithm CRYSTALS-Kyber --key-id <uuid> file1.txt file2.txt file3.txt file4.txt file5.txt" << std::endl;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    app.setApplicationName("milos-encrypt");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("MilOS");

    SimpleArgParser parser;
    QStringList args = parser.parseArguments(argc, argv);

    // Check for help flag
    if (args.isEmpty() || parser.hasFlag(args, "--help") || parser.hasFlag(args, "-h")) {
        printUsage();
        return 0;
    }

    // Check for batch subcommand
    if (!parser.hasSubcommand(args, "batch")) {
        std::cerr << "Error: 'batch' subcommand required" << std::endl;
        std::cerr << "Use --help for usage information" << std::endl;
        return 1;
    }

    // Get algorithm and key-id flags
    QString algorithm = parser.getFlagValue(args, "--algorithm");
    QString keyId = parser.getFlagValue(args, "--key-id");

    if (algorithm.isEmpty()) {
        std::cerr << "Error: --algorithm flag is required" << std::endl;
        return 1;
    }

    if (keyId.isEmpty()) {
        std::cerr << "Error: --key-id flag is required" << std::endl;
        return 1;
    }

    // Get positional arguments (files)
    QStringList filePaths = parser.getPositionalArgs(args, "batch");

    if (filePaths.isEmpty()) {
        std::cerr << "Error: At least one file must be specified" << std::endl;
        return 1;
    }

    if (filePaths.size() > 5) {
        std::cerr << "Error: Maximum 5 files allowed for batch operations" << std::endl;
        std::cerr << "Received " << filePaths.size() << " files" << std::endl;
        return 1;
    }

    // Create and execute batch command
    BatchCommand batchCommand;
    int exitCode = batchCommand.execute(filePaths, algorithm, keyId);

    return exitCode;
}

