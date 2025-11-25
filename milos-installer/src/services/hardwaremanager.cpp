#include "hardwaremanager.h"
#include <QVariantMap>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>

HardwareManager::HardwareManager(QObject *parent)
    : QObject(parent)
    , m_scanning(false)
    , m_scanProgress(0.0)
    , m_installingDrivers(false)
    , m_driverProgress(0.0)
{
}

void HardwareManager::scanHardware()
{
    if (m_scanning) {
        return;
    }
    
    m_scanning = true;
    m_scanProgress = 0.0;
    emit scanningChanged();
    emit scanProgressChanged();
    
    // Implement actual hardware detection using /proc and /sys
    QVariantList hardware;
    
    // Detect CPU
    m_scanProgress = 10.0;
    emit scanProgressChanged();
    QVariantMap cpu = detectCPU();
    if (!cpu.isEmpty()) {
        hardware.append(cpu);
    }
    
    // Detect GPU
    m_scanProgress = 30.0;
    emit scanProgressChanged();
    QVariantList gpus = detectGPUs();
    for (const QVariant &gpu : gpus) {
        hardware.append(gpu);
    }
    
    // Detect Network Cards
    m_scanProgress = 50.0;
    emit scanProgressChanged();
    QVariantList networkCards = detectNetworkCards();
    for (const QVariant &card : networkCards) {
        hardware.append(card);
    }
    
    // Detect Storage Devices
    m_scanProgress = 70.0;
    emit scanProgressChanged();
    QVariantList storage = detectStorage();
    for (const QVariant &device : storage) {
        hardware.append(device);
    }
    
    // Detect Audio Devices
    m_scanProgress = 90.0;
    emit scanProgressChanged();
    QVariantList audio = detectAudio();
    for (const QVariant &device : audio) {
        hardware.append(device);
    }
    
    m_detectedHardware = hardware;
    m_scanning = false;
    m_scanProgress = 100.0;
    emit scanningChanged();
    emit scanProgressChanged();
    emit detectedHardwareChanged();
    emit scanComplete();
}

void HardwareManager::installDrivers()
{
    if (m_installingDrivers) {
        return;
    }
    
    m_installingDrivers = true;
    m_driverProgress = 0.0;
    emit installingDriversChanged();
    emit driverProgressChanged();
    
    // Implement actual driver installation using package management
    QStringList driversToInstall;
    
    // Collect drivers needed from detected hardware
    for (const QVariant &item : m_detectedHardware) {
        QVariantMap hw = item.toMap();
        QString driver = hw.value("driver").toString();
        if (!driver.isEmpty() && !driversToInstall.contains(driver)) {
            driversToInstall.append(driver);
        }
    }
    
    if (driversToInstall.isEmpty()) {
        m_installingDrivers = false;
        emit installingDriversChanged();
        emit driverInstallationComplete();
        return;
    }
    
    // Detect package manager
    QProcess checkProcess;
    checkProcess.start("which", QStringList() << "pacman");
    checkProcess.waitForFinished(1000);
    bool isPacman = (checkProcess.exitCode() == 0);
    
    // Install drivers
    QProcess *process = new QProcess(this);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this, process](int exitCode, QProcess::ExitStatus exitStatus) {
        m_installingDrivers = false;
        emit installingDriversChanged();
        
        if (exitStatus == QProcess::NormalExit && exitCode == 0) {
            m_driverProgress = 100.0;
            emit driverProgressChanged();
            emit driverInstallationComplete();
        } else {
            QString error = process->readAllStandardError();
            emit driverInstallationError(QString("Driver installation failed: %1").arg(error));
        }
        process->deleteLater();
    });
    
    if (isPacman) {
        process->start("pacman", QStringList() << "-S" << "--noconfirm" << driversToInstall);
    } else {
        process->start("apt", QStringList() << "install" << "-y" << driversToInstall);
    }
    
    // Update progress (simplified - would need output parsing for real progress)
    m_driverProgress = 50.0;
    emit driverProgressChanged();
}

bool HardwareManager::checkCompatibility()
{
    // Implement hardware compatibility checking
    bool compatible = true;
    
    // Check CPU architecture
    QProcess process;
    process.start("uname", QStringList() << "-m");
    process.waitForFinished(1000);
    QString arch = process.readAllStandardOutput().trimmed();
    
    if (arch != "x86_64" && arch != "aarch64") {
        emit compatibilityWarning("Unsupported architecture. Only x86_64 and aarch64 are supported.");
        compatible = false;
    }
    
    // Check minimum RAM (2GB)
    QFile meminfo("/proc/meminfo");
    if (meminfo.open(QIODevice::ReadOnly)) {
        QTextStream stream(&meminfo);
        QString content = stream.readAll();
        QRegularExpression regex(R"(MemTotal:\s+(\d+)\s+kB)");
        QRegularExpressionMatch match = regex.match(content);
        if (match.hasMatch()) {
            qint64 memKB = match.captured(1).toLongLong();
            qint64 memGB = memKB / 1024 / 1024;
            if (memGB < 2) {
                emit compatibilityWarning(QString("Low memory detected: %1 GB. Minimum 2 GB required.").arg(memGB));
                compatible = false;
            }
        }
        meminfo.close();
    }
    
    // Check disk space (20GB minimum)
    QProcess dfProcess;
    dfProcess.start("df", QStringList() << "-BG" << "/");
    dfProcess.waitForFinished(1000);
    QString dfOutput = dfProcess.readAllStandardOutput();
    QRegularExpression dfRegex(R"(\s+(\d+)G\s+\d+G)");
    QRegularExpressionMatch dfMatch = dfRegex.match(dfOutput);
    if (dfMatch.hasMatch()) {
        int availableGB = dfMatch.captured(1).toInt();
        if (availableGB < 20) {
            emit compatibilityWarning(QString("Low disk space: %1 GB available. Minimum 20 GB required.").arg(availableGB));
            compatible = false;
        }
    }
    
    return compatible;
}

QVariantMap HardwareManager::detectCPU()
{
    QVariantMap cpu;
    
    // Read CPU info from /proc/cpuinfo
    QFile cpuinfo("/proc/cpuinfo");
    if (cpuinfo.open(QIODevice::ReadOnly)) {
        QTextStream stream(&cpuinfo);
        QString vendor, model;
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            if (line.startsWith("vendor_id")) {
                vendor = line.split(":").value(1).trimmed();
            } else if (line.startsWith("model name")) {
                model = line.split(":").value(1).trimmed();
                break;
            }
        }
        cpuinfo.close();
        
        if (!model.isEmpty()) {
            cpu["name"] = model;
            cpu["type"] = "CPU";
            cpu["vendor"] = vendor;
            cpu["model"] = model;
            cpu["driver"] = "intel-microcode"; // Default, could be detected
            cpu["compatible"] = true;
        }
    }
    
    return cpu;
}

QVariantList HardwareManager::detectGPUs()
{
    QVariantList gpus;
    
    // Check for NVIDIA GPU
    QProcess nvidiaProcess;
    nvidiaProcess.start("lspci", QStringList() << "-n" << "-d" << "10de::");
    nvidiaProcess.waitForFinished(1000);
    if (nvidiaProcess.exitCode() == 0 && !nvidiaProcess.readAllStandardOutput().isEmpty()) {
        QVariantMap gpu;
        gpu["name"] = "NVIDIA GPU";
        gpu["type"] = "GPU";
        gpu["vendor"] = "NVIDIA";
        gpu["model"] = "NVIDIA Graphics";
        gpu["driver"] = "nvidia";
        gpu["compatible"] = true;
        gpus.append(gpu);
    }
    
    // Check for AMD GPU
    QProcess amdProcess;
    amdProcess.start("lspci", QStringList() << "-n" << "-d" << "1002::");
    amdProcess.waitForFinished(1000);
    if (amdProcess.exitCode() == 0 && !amdProcess.readAllStandardOutput().isEmpty()) {
        QVariantMap gpu;
        gpu["name"] = "AMD GPU";
        gpu["type"] = "GPU";
        gpu["vendor"] = "AMD";
        gpu["model"] = "AMD Graphics";
        gpu["driver"] = "mesa";
        gpu["compatible"] = true;
        gpus.append(gpu);
    }
    
    // Check for Intel GPU
    QProcess intelProcess;
    intelProcess.start("lspci", QStringList() << "-n" << "-d" << "8086::");
    intelProcess.waitForFinished(1000);
    QString intelOutput = intelProcess.readAllStandardOutput();
    if (intelOutput.contains("VGA") || intelOutput.contains("Display")) {
        QVariantMap gpu;
        gpu["name"] = "Intel GPU";
        gpu["type"] = "GPU";
        gpu["vendor"] = "Intel";
        gpu["model"] = "Intel Graphics";
        gpu["driver"] = "mesa";
        gpu["compatible"] = true;
        gpus.append(gpu);
    }
    
    return gpus;
}

QVariantList HardwareManager::detectNetworkCards()
{
    QVariantList cards;
    
    // Use lspci to detect network cards
    QProcess process;
    process.start("lspci");
    process.waitForFinished(1000);
    QString output = process.readAllStandardOutput();
    
    QStringList lines = output.split("\n");
    for (const QString &line : lines) {
        if (line.contains("Ethernet controller") || line.contains("Network controller")) {
            QVariantMap card;
            card["name"] = line;
            card["type"] = line.contains("Ethernet") ? "Ethernet" : "Wireless";
            card["vendor"] = "Unknown";
            card["model"] = line;
            card["driver"] = QString();
            card["compatible"] = true;
            cards.append(card);
        }
    }
    
    return cards;
}

QVariantList HardwareManager::detectStorage()
{
    QVariantList storage;
    
    // List block devices
    QDir devDir("/sys/block");
    QStringList devices = devDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    
    for (const QString &device : devices) {
        if (device.startsWith("loop") || device.startsWith("ram")) {
            continue;
        }
        
        QVariantMap deviceInfo;
        deviceInfo["name"] = device;
        deviceInfo["type"] = "Storage";
        deviceInfo["vendor"] = "Unknown";
        deviceInfo["model"] = device;
        deviceInfo["driver"] = QString();
        deviceInfo["compatible"] = true;
        storage.append(deviceInfo);
    }
    
    return storage;
}

QVariantList HardwareManager::detectAudio()
{
    QVariantList audio;
    
    // Check for audio devices using lspci
    QProcess process;
    process.start("lspci");
    process.waitForFinished(1000);
    QString output = process.readAllStandardOutput();
    
    if (output.contains("Audio device")) {
        QVariantMap audioDevice;
        audioDevice["name"] = "Audio Device";
        audioDevice["type"] = "Audio";
        audioDevice["vendor"] = "Unknown";
        audioDevice["model"] = "Audio Device";
        audioDevice["driver"] = "alsa";
        audioDevice["compatible"] = true;
        audio.append(audioDevice);
    }
    
    return audio;
}

