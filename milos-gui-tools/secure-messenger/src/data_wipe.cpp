#include "data_wipe.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QRandomGenerator>
#include <QCryptographicHash>

DataWipe::DataWipe(QObject* parent)
    : QObject(parent)
{
}

DataWipe::~DataWipe() {
}

bool DataWipe::initialize() {
    return true;
}

bool DataWipe::wipeFile(const QString& filePath, WipeMethod method) {
    if (!QFile::exists(filePath)) {
        return true;  // Already deleted
    }

    emit wipeStarted(filePath);

    bool success = false;
    switch (method) {
        case WipeMethod::Cryptographic:
            success = cryptographicWipe(filePath);
            break;
        case WipeMethod::Overwrite:
            success = overwriteWipe(filePath);
            break;
        case WipeMethod::SecureDelete:
            success = secureDelete(filePath);
            break;
    }

    if (success) {
        emit wipeCompleted(filePath);
    } else {
        emit wipeFailed(filePath, "Wipe failed");
    }

    return success;
}

bool DataWipe::wipeDirectory(const QString& dirPath, WipeMethod method) {
    QDir dir(dirPath);
    if (!dir.exists()) {
        return true;
    }

    emit wipeStarted(dirPath);

    QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo& entry : entries) {
        if (entry.isDir()) {
            if (!wipeDirectory(entry.absoluteFilePath(), method)) {
                return false;
            }
        } else {
            if (!wipeFile(entry.absoluteFilePath(), method)) {
                return false;
            }
        }
    }

    bool success = dir.rmdir(dirPath);
    if (success) {
        emit wipeCompleted(dirPath);
    } else {
        emit wipeFailed(dirPath, "Failed to remove directory");
    }

    return success;
}

bool DataWipe::wipeDatabase(const QString& dbPath, WipeMethod method) {
    return wipeFile(dbPath, method);
}

bool DataWipe::verifyWipe(const QString& filePath) const {
    return !QFile::exists(filePath);
}

bool DataWipe::cryptographicWipe(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadWrite)) {
        return false;
    }

    qint64 fileSize = file.size();
    QByteArray randomData = generateRandomData(static_cast<int>(fileSize));
    
    file.seek(0);
    file.write(randomData);
    file.close();

    return file.remove();
}

bool DataWipe::overwriteWipe(const QString& filePath, int passes) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadWrite)) {
        return false;
    }

    qint64 fileSize = file.size();

    for (int i = 0; i < passes; ++i) {
        QByteArray randomData = generateRandomData(static_cast<int>(fileSize));
        file.seek(0);
        file.write(randomData);
        file.flush();
    }

    file.close();
    return file.remove();
}

bool DataWipe::secureDelete(const QString& filePath) {
    // In production, would use secure deletion system call
    return QFile::remove(filePath);
}

QByteArray DataWipe::generateRandomData(int size) const {
    QByteArray data;
    data.resize(size);
    
    for (int i = 0; i < size; ++i) {
        data[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    }
    
    return data;
}

