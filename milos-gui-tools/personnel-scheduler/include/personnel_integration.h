#ifndef PERSONNEL_INTEGRATION_H
#define PERSONNEL_INTEGRATION_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QList>

/**
 * @brief Personnel Integration
 * 
 * Integrates with external personnel management systems.
 */
class PersonnelIntegration : public QObject {
    Q_OBJECT

public:
    explicit PersonnelIntegration(QObject* parent = nullptr);
    ~PersonnelIntegration();

    /**
     * @brief Initialize integration
     * @param apiEndpoint API endpoint URL
     * @param apiKey API key
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const QString& apiEndpoint, const QString& apiKey);

    /**
     * @brief Sync personnel data
     * @return true if sync successful, false otherwise
     */
    bool syncPersonnelData();

    /**
     * @brief Get personnel information
     * @param personnelId Personnel ID
     * @return Personnel information map
     */
    QVariantMap getPersonnelInfo(const QString& personnelId) const;

    /**
     * @brief Get all personnel
     * @return List of personnel information
     */
    QList<QVariantMap> getAllPersonnel() const;

Q_SIGNALS:
    /**
     * @brief Emitted when personnel data is synced
     */
    void personnelDataSynced();

    /**
     * @brief Emitted when sync fails
     */
    void syncFailed(const QString& error);

private:
    QString m_apiEndpoint;
    QString m_apiKey;
    QList<QVariantMap> m_personnelData;
    bool m_initialized;
};

#endif // PERSONNEL_INTEGRATION_H

