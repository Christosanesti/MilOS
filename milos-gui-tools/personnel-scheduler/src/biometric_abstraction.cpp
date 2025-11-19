#include "biometric_abstraction.h"
#include "face_recognition_device.h"
#include "fingerprint_device.h"
#include <QDebug>
#include <QProcess>

std::unique_ptr<IBiometricDevice> BiometricDeviceFactory::createFaceRecognitionDevice(const QString& deviceId) {
    return std::make_unique<FaceRecognitionDevice>(deviceId);
}

std::unique_ptr<IBiometricDevice> BiometricDeviceFactory::createFingerprintDevice(const QString& deviceId) {
    return std::make_unique<FingerprintDevice>(deviceId);
}

QList<QString> BiometricDeviceFactory::detectDevices(BiometricDeviceType type) {
    QList<QString> devices;
    
    if (type == BiometricDeviceType::FaceRecognition) {
        // Detect USB cameras and face recognition devices
        // Check /dev/video* devices
        QProcess process;
        process.start("ls", QStringList() << "/dev/video*");
        if (process.waitForFinished(1000)) {
            QString output = process.readAllStandardOutput();
            QStringList lines = output.split('\n', Qt::SkipEmptyParts);
            for (const QString& line : lines) {
                if (line.startsWith("/dev/video")) {
                    devices.append(line.trimmed());
                }
            }
        }
        
        // If no devices found, add placeholder for testing
        if (devices.isEmpty()) {
            devices.append("/dev/video0");  // Placeholder
        }
    } else if (type == BiometricDeviceType::Fingerprint) {
#ifdef HAVE_LIBFPRINT
        // Use libfprint to detect fingerprint scanners
        struct fp_dscv_dev** discovered_devs;
        struct fp_dscv_dev* dscv_dev;
        
        discovered_devs = fp_discover_devs();
        if (discovered_devs) {
            for (int i = 0; discovered_devs[i]; i++) {
                dscv_dev = discovered_devs[i];
                QString deviceId = QString::number(fp_dscv_dev_get_driver_id(dscv_dev));
                devices.append(deviceId);
            }
            fp_dscv_devs_free(discovered_devs);
        }
#else
        // Placeholder for testing
        devices.append("fingerprint_0");
#endif
    }
    
    return devices;
}


