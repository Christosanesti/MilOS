#include "fingerprint_device.h"
#include <QDebug>

FingerprintDevice::FingerprintDevice(const QString& deviceId, QObject* parent)
    : IBiometricDevice(parent)
    , m_deviceId(deviceId)
    , m_vendor("Generic")
    , m_model("Fingerprint Scanner")
    , m_status(DeviceStatus::Disconnected)
#ifdef HAVE_LIBFPRINT
    , m_device(nullptr)
    , m_deviceInitialized(false)
#else
    , m_device(nullptr)
    , m_deviceInitialized(false)
#endif
{
}

FingerprintDevice::~FingerprintDevice() {
    cleanupLibFprint();
}

QString FingerprintDevice::deviceId() const {
    return m_deviceId;
}

BiometricDeviceType FingerprintDevice::deviceType() const {
    return BiometricDeviceType::Fingerprint;
}

QString FingerprintDevice::vendor() const {
    return m_vendor;
}

QString FingerprintDevice::model() const {
    return m_model;
}

bool FingerprintDevice::initialize() {
    if (m_status == DeviceStatus::Connected) {
        return true;
    }
    
    m_status = DeviceStatus::Initializing;
    emit statusChanged(m_status);
    
    if (initializeLibFprint()) {
        m_status = DeviceStatus::Connected;
        emit statusChanged(m_status);
        return true;
    } else {
        m_status = DeviceStatus::Error;
        emit statusChanged(m_status);
        emit errorOccurred("Failed to initialize fingerprint scanner");
        return false;
    }
}

bool FingerprintDevice::isConnected() const {
    return m_status == DeviceStatus::Connected;
}

DeviceHealth FingerprintDevice::getHealth() const {
    DeviceHealth health;
    health.status = m_status;
    health.healthScore = (m_status == DeviceStatus::Connected) ? 100 : 0;
    
    if (m_status == DeviceStatus::Error) {
        health.errorMessage = "Device error";
    }
    
    QVariantMap metrics;
    metrics["connected"] = (m_status == DeviceStatus::Connected);
    health.metrics = metrics;
    
    return health;
}

QByteArray FingerprintDevice::capture(int timeoutMs) {
    if (!isConnected()) {
        emit errorOccurred("Device not connected");
        return QByteArray();
    }
    
#ifdef HAVE_LIBFPRINT
    if (!m_device || !m_deviceInitialized) {
        emit errorOccurred("Fingerprint scanner not available");
        return QByteArray();
    }
    
    // Capture fingerprint using libfprint
    struct fp_print_data* print_data = nullptr;
    int result = fp_acquire_fingerprint(m_device, &print_data);
    
    if (result == 0 && print_data) {
        // Convert print data to byte array
        size_t dataSize = fp_print_data_get_data(print_data);
        const unsigned char* data = fp_print_data_get_data(print_data);
        QByteArray fingerprintData(reinterpret_cast<const char*>(data), dataSize);
        
        fp_print_data_free(print_data);
        emit biometricCaptured(fingerprintData);
        return processFingerprintData(fingerprintData);
    } else {
        emit errorOccurred("Failed to capture fingerprint");
        return QByteArray();
    }
#else
    // Placeholder: Return dummy data for testing
    QByteArray dummyData("fingerprint_capture_dummy_data");
    emit biometricCaptured(dummyData);
    return dummyData;
#endif
}

int FingerprintDevice::verify(const QByteArray& templateData, const QByteArray& capturedData) {
    if (!isConnected()) {
        return 0;
    }
    
#ifdef HAVE_LIBFPRINT
    if (!m_deviceInitialized) {
        return 0;
    }
    
    // Use libfprint to verify fingerprint
    struct fp_print_data* template_print = fp_print_data_from_data(templateData.constData(), templateData.size());
    struct fp_print_data* captured_print = fp_print_data_from_data(capturedData.constData(), capturedData.size());
    
    if (template_print && captured_print) {
        int result = fp_print_data_compare(template_print, captured_print);
        fp_print_data_free(template_print);
        fp_print_data_free(captured_print);
        
        // Convert libfprint result to match score (0-100)
        return (result > 0) ? 100 : 0;
    }
    
    return 0;
#else
    // Placeholder: Return dummy match score
    if (templateData == capturedData) {
        return 100;
    }
    return 50;
#endif
}

QByteArray FingerprintDevice::enroll(const QByteArray& capturedData) {
    if (!isConnected()) {
        return QByteArray();
    }
    
    // Process captured data to create template
    return processFingerprintData(capturedData);
}

QVariantMap FingerprintDevice::getConfiguration() const {
    return m_configuration;
}

bool FingerprintDevice::setConfiguration(const QVariantMap& config) {
    m_configuration = config;
    return true;
}

bool FingerprintDevice::initializeLibFprint() {
#ifdef HAVE_LIBFPRINT
    // Initialize libfprint
    if (fp_init() != 0) {
        return false;
    }
    
    // Discover devices
    struct fp_dscv_dev** discovered_devs = fp_discover_devs();
    if (!discovered_devs) {
        return false;
    }
    
    // Find device matching deviceId
    bool found = false;
    for (int i = 0; discovered_devs[i]; i++) {
        struct fp_dscv_dev* dscv_dev = discovered_devs[i];
        QString discoveredId = QString::number(fp_dscv_dev_get_driver_id(dscv_dev));
        
        if (discoveredId == m_deviceId) {
            m_device = fp_dev_open(dscv_dev);
            if (m_device) {
                m_vendor = QString::fromUtf8(fp_dscv_dev_get_driver(dscv_dev)->full_name);
                m_model = QString::fromUtf8(fp_dscv_dev_get_driver(dscv_dev)->name);
                m_deviceInitialized = true;
                found = true;
            }
            break;
        }
    }
    
    fp_dscv_devs_free(discovered_devs);
    return found;
#else
    // Placeholder: Always succeed for testing
    m_deviceInitialized = true;
    return true;
#endif
}

void FingerprintDevice::cleanupLibFprint() {
#ifdef HAVE_LIBFPRINT
    if (m_device) {
        fp_dev_close(m_device);
        m_device = nullptr;
    }
    fp_exit();
    m_deviceInitialized = false;
#endif
}

QByteArray FingerprintDevice::processFingerprintData(const QByteArray& rawData) {
    // Process fingerprint data to create template
    // This is a simplified implementation
    // In production, this would use proper fingerprint recognition algorithms
    return rawData;  // Return processed template
}


