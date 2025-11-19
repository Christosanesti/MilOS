#ifndef FACE_RECOGNITION_DEVICE_H
#define FACE_RECOGNITION_DEVICE_H

#include "biometric_abstraction.h"
#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVariantMap>

#ifdef HAVE_OPENCV
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#endif

/**
 * @brief Face Recognition Device Implementation
 * 
 * Implements face recognition device interface using OpenCV or USB camera.
 */
class FaceRecognitionDevice : public IBiometricDevice {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param deviceId Device identifier (e.g., /dev/video0, USB device path)
     */
    explicit FaceRecognitionDevice(const QString& deviceId, QObject* parent = nullptr);
    ~FaceRecognitionDevice();

    QString deviceId() const override;
    BiometricDeviceType deviceType() const override;
    QString vendor() const override;
    QString model() const override;
    bool initialize() override;
    bool isConnected() const override;
    DeviceHealth getHealth() const override;
    QByteArray capture(int timeoutMs = 5000) override;
    int verify(const QByteArray& templateData, const QByteArray& capturedData) override;
    QByteArray enroll(const QByteArray& capturedData) override;
    QVariantMap getConfiguration() const override;
    bool setConfiguration(const QVariantMap& config) override;

private:
    QString m_deviceId;
    QString m_vendor;
    QString m_model;
    DeviceStatus m_status;
    QVariantMap m_configuration;
    
#ifdef HAVE_OPENCV
    cv::VideoCapture* m_camera;
    cv::Ptr<cv::face::LBPHFaceRecognizer> m_recognizer;
    bool m_recognizerInitialized;
#else
    void* m_camera;  // Placeholder for camera handle
    bool m_recognizerInitialized;
#endif

    bool initializeCamera();
    void cleanupCamera();
    QByteArray processFaceImage(const QByteArray& imageData);
};

#endif // FACE_RECOGNITION_DEVICE_H


