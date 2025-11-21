#ifndef DEPLOYMENT_ORCHESTRATOR_H
#define DEPLOYMENT_ORCHESTRATOR_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QDateTime>

/**
 * @brief Deployment Status
 */
enum class DeploymentStatus {
    Pending,
    InProgress,
    Completed,
    Failed,
    RolledBack
};

/**
 * @brief Deployment Task
 */
struct DeploymentTask {
    QString id;
    QString name;
    QString target;
    QStringList commands;
    DeploymentStatus status;
    QDateTime startedAt;
    QDateTime completedAt;
    QString errorMessage;
};

/**
 * @brief Deployment Orchestrator
 * 
 * Orchestrates deployments across multiple sites.
 */
class DeploymentOrchestrator : public QObject {
    Q_OBJECT

public:
    explicit DeploymentOrchestrator(QObject* parent = nullptr);
    ~DeploymentOrchestrator();

    /**
     * @brief Initialize deployment orchestrator
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Create deployment
     * @param name Deployment name
     * @param targets List of target sites
     * @param tasks List of deployment tasks
     * @return Deployment ID or empty if failed
     */
    QString createDeployment(const QString& name, const QStringList& targets, const QList<DeploymentTask>& tasks);

    /**
     * @brief Execute deployment
     * @param deploymentId Deployment ID
     * @return true if execute successful, false otherwise
     */
    bool executeDeployment(const QString& deploymentId);

    /**
     * @brief Get deployment status
     * @param deploymentId Deployment ID
     * @return Deployment status
     */
    QVariantMap getDeploymentStatus(const QString& deploymentId) const;

    /**
     * @brief Rollback deployment
     * @param deploymentId Deployment ID
     * @return true if rollback successful, false otherwise
     */
    bool rollbackDeployment(const QString& deploymentId);

Q_SIGNALS:
    /**
     * @brief Emitted when deployment status changes
     */
    void deploymentStatusChanged(const QString& deploymentId, DeploymentStatus status);

private:
    QMap<QString, QVariantMap> m_deployments;
    QString generateDeploymentId() const;
};

Q_DECLARE_METATYPE(DeploymentStatus)
Q_DECLARE_METATYPE(DeploymentTask)

#endif // DEPLOYMENT_ORCHESTRATOR_H

