#include "encryptionsetupscreen.h"

EncryptionSetupScreen::EncryptionSetupScreen(QQuickItem *parent)
    : QQuickItem(parent)
    , m_encryptionPassword("")
    , m_selectedAlgorithm("aes-xts-plain64")
    , m_encryptionInProgress(false)
    , m_encryptionProgress(0.0)
{
}

void EncryptionSetupScreen::setEncryptionPassword(const QString &password)
{
    if (m_encryptionPassword != password) {
        m_encryptionPassword = password;
        emit encryptionPasswordChanged();
    }
}

void EncryptionSetupScreen::setSelectedAlgorithm(const QString &algorithm)
{
    if (m_selectedAlgorithm != algorithm) {
        m_selectedAlgorithm = algorithm;
        emit selectedAlgorithmChanged();
    }
}

void EncryptionSetupScreen::startEncryption()
{
    if (m_encryptionInProgress) {
        return;
    }
    
    m_encryptionInProgress = true;
    emit encryptionInProgressChanged();
    
    // TODO: Implement actual LUKS2 encryption setup
    // This would call EncryptionManager service to set up disk encryption
    
    // Simulate progress for now
    m_encryptionProgress = 100.0;
    emit encryptionProgressChanged();
    
    m_encryptionInProgress = false;
    emit encryptionInProgressChanged();
}

