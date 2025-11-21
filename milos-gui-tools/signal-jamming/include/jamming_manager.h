#ifndef JAMMING_MANAGER_H
#define JAMMING_MANAGER_H

#include "jamming_device.h"
#include "spectrum_analyzer.h"
#include <QObject>
#include <QString>
#include <QList>

/**
 * @brief Jamming Manager
 * 
 * Manages multiple jamming devices and patterns.
 */
class JammingManager : public QObject {
    Q_OBJECT

public:
    explicit JammingManager(QObject* parent = nullptr);
    ~JammingManager();

    /**
     * @brief Initialize jamming manager
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Add jamming device
     * @param device Jamming device
     * @return true if add successful, false otherwise
     */
    bool addDevice(JammingDevice* device);

    /**
     * @brief Remove jamming device
     * @param deviceId Device ID
     * @return true if remove successful, false otherwise
     */
    bool removeDevice(const QString& deviceId);

    /**
     * @brief Get all devices
     * @return List of device IDs
     */
    QStringList getAllDevices() const;

    /**
     * @brief Get device
     * @param deviceId Device ID
     * @return Device or nullptr if not found
     */
    JammingDevice* getDevice(const QString& deviceId) const;

    /**
     * @brief Create jamming pattern
     * @param name Pattern name
     * @param frequencyStart Start frequency (MHz)
     * @param frequencyEnd End frequency (MHz)
     * @param power Power (dBm)
     * @param patternType Pattern type
     * @return Pattern ID or empty if failed
     */
    QString createPattern(const QString& name, double frequencyStart, double frequencyEnd,
                         double power, const QString& patternType);

    /**
     * @brief Get pattern
     * @param patternId Pattern ID
     * @return Pattern or empty if not found
     */
    JammingPattern getPattern(const QString& patternId) const;

    /**
     * @brief Get all patterns
     * @return List of patterns
     */
    QList<JammingPattern> getAllPatterns() const;

    /**
     * @brief Start jamming on device with pattern
     * @param deviceId Device ID
     * @param patternId Pattern ID
     * @return true if start successful, false otherwise
     */
    bool startJamming(const QString& deviceId, const QString& patternId);

    /**
     * @brief Stop jamming on device
     * @param deviceId Device ID
     * @return true if stop successful, false otherwise
     */
    bool stopJamming(const QString& deviceId);

    /**
     * @brief Get spectrum analyzer
     * @return Spectrum analyzer
     */
    SpectrumAnalyzer* getSpectrumAnalyzer() const { return m_spectrumAnalyzer; }

Q_SIGNALS:
    /**
     * @brief Emitted when device is added
     */
    void deviceAdded(const QString& deviceId);

    /**
     * @brief Emitted when device is removed
     */
    void deviceRemoved(const QString& deviceId);

    /**
     * @brief Emitted when jamming starts
     */
    void jammingStarted(const QString& deviceId, const QString& patternId);

    /**
     * @brief Emitted when jamming stops
     */
    void jammingStopped(const QString& deviceId);

private:
    QMap<QString, JammingDevice*> m_devices;
    QMap<QString, JammingPattern> m_patterns;
    SpectrumAnalyzer* m_spectrumAnalyzer;
    
    QString generatePatternId() const;
};

#endif // JAMMING_MANAGER_H

