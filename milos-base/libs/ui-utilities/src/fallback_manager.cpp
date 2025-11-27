#include "milos/ui/fallback_manager.h"
#include "milos/logging/logger.h"
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QHash>

FallbackManager* FallbackManager::s_instance = nullptr;

FallbackManager::FallbackManager(QObject* parent)
    : QObject(parent)
    , m_xenonEffectsAvailable(false)
    , m_hardwareAccelerationAvailable(false)
    , m_networkAvailable(false)
    , m_serviceAvailable(false)
{
}

FallbackManager::~FallbackManager() {
}

FallbackManager* FallbackManager::instance() {
    if (!s_instance) {
        s_instance = new FallbackManager();
    }
    return s_instance;
}

bool FallbackManager::initialize() {
    if (!Logger::instance()->isInitialized()) {
        Logger::instance()->initialize("FallbackManager");
    }
    
    // Check all features
    checkXenonEffects();
    checkHardwareAcceleration();
    checkNetwork();
    checkServices();
    
    LOG_INFO("Fallback Manager initialized");
    return true;
}

bool FallbackManager::isFeatureAvailable(const QString& featureName) const {
    if (featureName == "xenon_effects") {
        return m_xenonEffectsAvailable;
    } else if (featureName == "hardware_acceleration") {
        return m_hardwareAccelerationAvailable;
    } else if (featureName == "network") {
        return m_networkAvailable;
    } else if (featureName == "service") {
        return m_serviceAvailable;
    }
    
    // Check registered fallbacks
    return !m_fallbacks.contains(featureName) || m_fallbacks.value(featureName).toBool();
}

QVariant FallbackManager::getFallbackValue(const QString& featureName, const QVariant& defaultValue) const {
    if (m_fallbacks.contains(featureName)) {
        return m_fallbacks.value(featureName);
    }
    return defaultValue;
}

void FallbackManager::registerFallback(const QString& featureName, const QVariant& fallbackValue) {
    m_fallbacks[featureName] = fallbackValue;
    LOG_INFO(QString("Registered fallback for %1").arg(featureName));
}

void FallbackManager::checkXenonEffects() {
    QOpenGLContext context;
    bool available = false;
    
    if (context.create()) {
        QOpenGLFunctions* gl = context.functions();
        if (gl) {
            const char* extensions = reinterpret_cast<const char*>(gl->glGetString(GL_EXTENSIONS));
            if (extensions && QString::fromLatin1(extensions).contains("GL_ARB_shader_objects")) {
                available = true;
            }
        }
    }
    
    if (m_xenonEffectsAvailable != available) {
        m_xenonEffectsAvailable = available;
        emit xenonEffectsChanged();
        
        if (!available) {
            emit featureUnavailable("xenon_effects", "OpenGL shader support not available");
            LOG_WARNING("Xenon effects unavailable - using fallback styling");
        }
    }
}

void FallbackManager::checkHardwareAcceleration() {
    QOpenGLContext context;
    bool available = context.create();
    
    if (m_hardwareAccelerationAvailable != available) {
        m_hardwareAccelerationAvailable = available;
        emit hardwareAccelerationChanged();
        
        if (!available) {
            emit featureUnavailable("hardware_acceleration", "OpenGL context creation failed");
            LOG_WARNING("Hardware acceleration unavailable");
        }
    }
}

void FallbackManager::checkNetwork() {
    QNetworkAccessManager manager;
    // Simple check - in production would ping a known endpoint
    bool available = true;  // Assume available, will be updated on actual network operations
    
    if (m_networkAvailable != available) {
        m_networkAvailable = available;
        emit networkChanged();
    }
}

void FallbackManager::checkServices() {
    QDBusConnection connection = QDBusConnection::systemBus();
    bool available = connection.isConnected();
    
    if (m_serviceAvailable != available) {
        m_serviceAvailable = available;
        emit serviceChanged();
        
        if (!available) {
            emit featureUnavailable("service", "D-Bus connection not available");
            LOG_WARNING("D-Bus services unavailable");
        }
    }
}


#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QHash>

FallbackManager* FallbackManager::s_instance = nullptr;

FallbackManager::FallbackManager(QObject* parent)
    : QObject(parent)
    , m_xenonEffectsAvailable(false)
    , m_hardwareAccelerationAvailable(false)
    , m_networkAvailable(false)
    , m_serviceAvailable(false)
{
}

FallbackManager::~FallbackManager() {
}

FallbackManager* FallbackManager::instance() {
    if (!s_instance) {
        s_instance = new FallbackManager();
    }
    return s_instance;
}

bool FallbackManager::initialize() {
    if (!Logger::instance()->isInitialized()) {
        Logger::instance()->initialize("FallbackManager");
    }
    
    // Check all features
    checkXenonEffects();
    checkHardwareAcceleration();
    checkNetwork();
    checkServices();
    
    LOG_INFO("Fallback Manager initialized");
    return true;
}

bool FallbackManager::isFeatureAvailable(const QString& featureName) const {
    if (featureName == "xenon_effects") {
        return m_xenonEffectsAvailable;
    } else if (featureName == "hardware_acceleration") {
        return m_hardwareAccelerationAvailable;
    } else if (featureName == "network") {
        return m_networkAvailable;
    } else if (featureName == "service") {
        return m_serviceAvailable;
    }
    
    // Check registered fallbacks
    return !m_fallbacks.contains(featureName) || m_fallbacks.value(featureName).toBool();
}

QVariant FallbackManager::getFallbackValue(const QString& featureName, const QVariant& defaultValue) const {
    if (m_fallbacks.contains(featureName)) {
        return m_fallbacks.value(featureName);
    }
    return defaultValue;
}

void FallbackManager::registerFallback(const QString& featureName, const QVariant& fallbackValue) {
    m_fallbacks[featureName] = fallbackValue;
    LOG_INFO(QString("Registered fallback for %1").arg(featureName));
}

void FallbackManager::checkXenonEffects() {
    QOpenGLContext context;
    bool available = false;
    
    if (context.create()) {
        QOpenGLFunctions* gl = context.functions();
        if (gl) {
            const char* extensions = reinterpret_cast<const char*>(gl->glGetString(GL_EXTENSIONS));
            if (extensions && QString::fromLatin1(extensions).contains("GL_ARB_shader_objects")) {
                available = true;
            }
        }
    }
    
    if (m_xenonEffectsAvailable != available) {
        m_xenonEffectsAvailable = available;
        emit xenonEffectsChanged();
        
        if (!available) {
            emit featureUnavailable("xenon_effects", "OpenGL shader support not available");
            LOG_WARNING("Xenon effects unavailable - using fallback styling");
        }
    }
}

void FallbackManager::checkHardwareAcceleration() {
    QOpenGLContext context;
    bool available = context.create();
    
    if (m_hardwareAccelerationAvailable != available) {
        m_hardwareAccelerationAvailable = available;
        emit hardwareAccelerationChanged();
        
        if (!available) {
            emit featureUnavailable("hardware_acceleration", "OpenGL context creation failed");
            LOG_WARNING("Hardware acceleration unavailable");
        }
    }
}

void FallbackManager::checkNetwork() {
    QNetworkAccessManager manager;
    // Simple check - in production would ping a known endpoint
    bool available = true;  // Assume available, will be updated on actual network operations
    
    if (m_networkAvailable != available) {
        m_networkAvailable = available;
        emit networkChanged();
    }
}

void FallbackManager::checkServices() {
    QDBusConnection connection = QDBusConnection::systemBus();
    bool available = connection.isConnected();
    
    if (m_serviceAvailable != available) {
        m_serviceAvailable = available;
        emit serviceChanged();
        
        if (!available) {
            emit featureUnavailable("service", "D-Bus connection not available");
            LOG_WARNING("D-Bus services unavailable");
        }
    }
}

