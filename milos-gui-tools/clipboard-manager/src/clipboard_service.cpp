#include "clipboard_service.h"
#include "clipboard_encryption.h"
#include "clipboard_history.h"
#include "clipboard_integration.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QUuid>
#include <QCoreApplication>
#include <iostream>

ClipboardService::ClipboardService(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_running(false)
    , m_isEncrypted(false)
    , m_timeoutSeconds(300)  // 5 minutes default
    , m_timeoutTimer(nullptr)
    , m_monitorTimer(nullptr)
{
}

ClipboardService::~ClipboardService() {
    stop();
}

bool ClipboardService::initialize() {
    if (m_initialized) {
        return true;
    }

    // Initialize encryption
    m_encryption = std::make_unique<ClipboardEncryption>();
    if (!m_encryption->initialize()) {
        std::cerr << "Failed to initialize clipboard encryption" << std::endl;
        return false;
    }

    // Initialize history
    m_history = std::make_unique<ClipboardHistoryManager>();
    std::string dbPath = "/var/lib/milos/clipboard-manager/history.db";
    if (!m_history->initialize(dbPath)) {
        std::cerr << "Failed to initialize clipboard history" << std::endl;
        return false;
    }

    // Initialize system clipboard integration
    m_integration = std::make_unique<ClipboardIntegration>();
    if (!m_integration->initialize()) {
        std::cerr << "Failed to initialize clipboard integration" << std::endl;
        return false;
    }

    // Create timers
    m_timeoutTimer = new QTimer(this);
    connect(m_timeoutTimer, &QTimer::timeout, this, &ClipboardService::onTimeout);
    m_timeoutTimer->setSingleShot(true);

    m_monitorTimer = new QTimer(this);
    connect(m_monitorTimer, &QTimer::timeout, this, &ClipboardService::monitorSystemClipboard);
    m_monitorTimer->setInterval(1000);  // Check every second

    // Connect to system clipboard changes
    connect(m_integration.get(), &ClipboardIntegration::clipboardChanged,
            this, &ClipboardService::updateClipboardFromSystem);

    m_initialized = true;
    return true;
}

bool ClipboardService::start() {
    if (!m_initialized) {
        if (!initialize()) {
            return false;
        }
    }

    if (m_running) {
        return true;
    }

    // Start monitoring system clipboard
    m_monitorTimer->start();
    updateClipboardFromSystem();

    m_running = true;
    return true;
}

void ClipboardService::stop() {
    if (!m_running) {
        return;
    }

    if (m_timeoutTimer) {
        m_timeoutTimer->stop();
    }

    if (m_monitorTimer) {
        m_monitorTimer->stop();
    }

    m_running = false;
}

bool ClipboardService::copyToClipboard(const QString& data) {
    if (!m_initialized || !m_encryption) {
        return false;
    }

    // Encrypt data
    std::vector<uint8_t> plaintext(data.toUtf8().begin(), data.toUtf8().end());
    std::vector<uint8_t> encrypted = m_encryption->encrypt(plaintext);

    // Store in history
    ClipboardEntry entry;
    entry.id = QUuid::createUuid().toString().toStdString();
    entry.encrypted_data = encrypted;
    entry.data_type = "text";
    entry.timestamp = QDateTime::currentSecsSinceEpoch();
    entry.timeout = m_timeoutSeconds;
    entry.is_active = true;

    if (!m_history->addEntry(entry)) {
        std::cerr << "Failed to add clipboard entry to history" << std::endl;
        return false;
    }

    // Update system clipboard
    if (!m_integration->setClipboardText(data)) {
        std::cerr << "Failed to set system clipboard" << std::endl;
        return false;
    }

    // Update current clipboard
    m_currentClipboard = data;
    m_isEncrypted = true;

    // Start timeout timer
    if (m_timeoutTimer) {
        m_timeoutTimer->start(m_timeoutSeconds * 1000);
    }

    emit clipboardChanged();
    emit encryptionStatusChanged();
    return true;
}

QString ClipboardService::getClipboard() {
    return m_currentClipboard;
}

bool ClipboardService::clearClipboard() {
    if (!m_integration) {
        return false;
    }

    m_currentClipboard.clear();
    m_isEncrypted = false;

    if (m_timeoutTimer) {
        m_timeoutTimer->stop();
    }

    if (!m_integration->clearClipboard()) {
        return false;
    }

    emit clipboardChanged();
    emit encryptionStatusChanged();
    return true;
}

QString ClipboardService::getClipboardHistory() const {
    if (!m_history) {
        return QString("[]");
    }

    auto entries = m_history->getEntries(50);
    QJsonArray historyArray;

    for (const auto& entry : entries) {
        QJsonObject entryObj;
        entryObj["id"] = QString::fromStdString(entry.id);
        entryObj["data_type"] = QString::fromStdString(entry.data_type);
        entryObj["timestamp"] = static_cast<qint64>(entry.timestamp);
        entryObj["timeout"] = static_cast<qint64>(entry.timeout);
        entryObj["is_active"] = entry.is_active;
        historyArray.append(entryObj);
    }

    QJsonDocument doc(historyArray);
    return QString::fromUtf8(doc.toJson());
}

void ClipboardService::onTimeout() {
    clearClipboard();
    emit clipboardTimeout();
}

void ClipboardService::monitorSystemClipboard() {
    if (!m_integration) {
        return;
    }

    QString systemClipboard = m_integration->getClipboardText();
    if (systemClipboard != m_currentClipboard && !systemClipboard.isEmpty()) {
        // New clipboard data from system - encrypt and store
        copyToClipboard(systemClipboard);
    }
}

void ClipboardService::updateClipboardFromSystem() {
    if (!m_integration) {
        return;
    }

    QString systemClipboard = m_integration->getClipboardText();
    if (systemClipboard != m_currentClipboard) {
        if (!systemClipboard.isEmpty()) {
            copyToClipboard(systemClipboard);
        } else {
            clearClipboard();
        }
    }
}

void ClipboardService::updateSystemClipboard(const QString& data) {
    if (m_integration) {
        m_integration->setClipboardText(data);
    }
}

