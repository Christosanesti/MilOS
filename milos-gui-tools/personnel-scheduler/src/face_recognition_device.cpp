#include "face_recognition_device.h"
#include <QDebug>
#include <QFileInfo>

#ifdef HAVE_OPENCV
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#endif

FaceRecognitionDevice::FaceRecognitionDevice(const QString& deviceId, QObject* parent)
    : IBiometricDevice(parent)
    , m_deviceId(deviceId)
    , m_vendor("Generic")
    , m_model("USB Camera")
    , m_status(DeviceStatus::Disconnected)
#ifdef HAVE_OPENCV
    , m_camera(nullptr)
    , m_recognizer(nullptr)
    , m_recognizerInitialized(false)
#else
    , m_camera(nullptr)
    , m_recognizerInitialized(false)
#endif
{
}

FaceRecognitionDevice::~FaceRecognitionDevice() {
    cleanupCamera();
}

QString FaceRecognitionDevice::deviceId() const {
    return m_deviceId;
}

BiometricDeviceType FaceRecognitionDevice::deviceType() const {
    return BiometricDeviceType::FaceRecognition;
}

QString FaceRecognitionDevice::vendor() const {
    return m_vendor;
}

QString FaceRecognitionDevice::model() const {
    return m_model;
}

bool FaceRecognitionDevice::initialize() {
    if (m_status == DeviceStatus::Connected) {
        return true;
    }
    
    m_status = DeviceStatus::Initializing;
    emit statusChanged(m_status);
    
    if (initializeCamera()) {
        m_status = DeviceStatus::Connected;
        emit statusChanged(m_status);
        return true;
    } else {
        m_status = DeviceStatus::Error;
        emit statusChanged(m_status);
        emit errorOccurred("Failed to initialize camera");
        return false;
    }
}

bool FaceRecognitionDevice::isConnected() const {
    return m_status == DeviceStatus::Connected;
}

DeviceHealth FaceRecognitionDevice::getHealth() const {
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

QByteArray FaceRecognitionDevice::capture(int timeoutMs) {
    if (!isConnected()) {
        emit errorOccurred("Device not connected");
        return QByteArray();
    }
    
#ifdef HAVE_OPENCV
    if (!m_camera || !m_camera->isOpened()) {
        emit errorOccurred("Camera not available");
        return QByteArray();
    }
    
    cv::Mat frame;
    if (m_camera->read(frame)) {
        std::vector<uchar> buffer;
        cv::imencode(".jpg", frame, buffer);
        QByteArray imageData(reinterpret_cast<const char*>(buffer.data()), buffer.size());
        emit biometricCaptured(imageData);
        return processFaceImage(imageData);
    }
#else
    // Placeholder: Return dummy data for testing
    QByteArray dummyData("face_capture_dummy_data");
    emit biometricCaptured(dummyData);
    return dummyData;
#endif
    
    emit errorOccurred("Failed to capture image");
    return QByteArray();
}

int FaceRecognitionDevice::verify(const QByteArray& templateData, const QByteArray& capturedData) {
    if (!isConnected()) {
        return 0;
    }
    
#ifdef HAVE_OPENCV
    if (!m_recognizerInitialized) {
        return 0;
    }
    
    // Process captured data and compare with template
    // This is a simplified implementation
    // In production, this would use proper face recognition algorithms
    if (templateData == capturedData) {
        return 100;  // Perfect match
    }
    
    // Calculate similarity (simplified)
    int similarity = 0;
    int minLength = qMin(templateData.size(), capturedData.size());
    for (int i = 0; i < minLength; i++) {
        if (templateData[i] == capturedData[i]) {
            similarity++;
        }
    }
    
    return (similarity * 100) / minLength;
#else
    // Placeholder: Return dummy match score
    return 75;
#endif
}

QByteArray FaceRecognitionDevice::enroll(const QByteArray& capturedData) {
    if (!isConnected()) {
        return QByteArray();
    }
    
    // Process captured data to create template
    return processFaceImage(capturedData);
}

QVariantMap FaceRecognitionDevice::getConfiguration() const {
    return m_configuration;
}

bool FaceRecognitionDevice::setConfiguration(const QVariantMap& config) {
    m_configuration = config;
    return true;
}

bool FaceRecognitionDevice::initializeCamera() {
#ifdef HAVE_OPENCV
    // Extract camera index from device ID (e.g., /dev/video0 -> 0)
    int cameraIndex = 0;
    if (m_deviceId.startsWith("/dev/video")) {
        QString indexStr = m_deviceId.mid(10);
        bool ok;
        cameraIndex = indexStr.toInt(&ok);
        if (!ok) {
            cameraIndex = 0;
        }
    }
    
    m_camera = new cv::VideoCapture(cameraIndex);
    if (m_camera->isOpened()) {
        // Initialize face recognizer
        m_recognizer = cv::face::LBPHFaceRecognizer::create();
        m_recognizerInitialized = true;
        return true;
    } else {
        delete m_camera;
        m_camera = nullptr;
        return false;
    }
#else
    // Placeholder: Always succeed for testing
    return true;
#endif
}

void FaceRecognitionDevice::cleanupCamera() {
#ifdef HAVE_OPENCV
    if (m_camera) {
        m_camera->release();
        delete m_camera;
        m_camera = nullptr;
    }
    m_recognizerInitialized = false;
#endif
}

QByteArray FaceRecognitionDevice::processFaceImage(const QByteArray& imageData) {
    // Process face image to create template
    // This is a simplified implementation
    // In production, this would use proper face recognition algorithms
    return imageData;  // Return processed template
}


