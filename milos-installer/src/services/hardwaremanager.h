#ifndef HARDWAREMANAGER_H
#define HARDWAREMANAGER_H

#include <QObject>
#include <QString>
#include <QVariantList>

class HardwareManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool scanning READ scanning NOTIFY scanningChanged)
    Q_PROPERTY(double scanProgress READ scanProgress NOTIFY scanProgressChanged)
    Q_PROPERTY(QVariantList detectedHardware READ detectedHardware NOTIFY detectedHardwareChanged)
    Q_PROPERTY(bool installingDrivers READ installingDrivers NOTIFY installingDriversChanged)
    Q_PROPERTY(double driverProgress READ driverProgress NOTIFY driverProgressChanged)

public:
    explicit HardwareManager(QObject *parent = nullptr);
    
    bool scanning() const { return m_scanning; }
    double scanProgress() const { return m_scanProgress; }
    QVariantList detectedHardware() const { return m_detectedHardware; }
    bool installingDrivers() const { return m_installingDrivers; }
    double driverProgress() const { return m_driverProgress; }
    
    Q_INVOKABLE void scanHardware();
    Q_INVOKABLE void installDrivers();
    Q_INVOKABLE bool checkCompatibility();

signals:
    void scanningChanged();
    void scanProgressChanged();
    void detectedHardwareChanged();
    void installingDriversChanged();
    void driverProgressChanged();
    void scanComplete();
    void driverInstallationComplete();
    void driverInstallationError(const QString &error);
    void compatibilityWarning(const QString &warning);

private:
    bool m_scanning;
    double m_scanProgress;
    QVariantList m_detectedHardware;
    bool m_installingDrivers;
    double m_driverProgress;
    
    QVariantMap detectCPU();
    QVariantList detectGPUs();
    QVariantList detectNetworkCards();
    QVariantList detectStorage();
    QVariantList detectAudio();
};

#endif // HARDWAREMANAGER_H

